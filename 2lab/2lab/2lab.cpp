#include "Bitmap.h"
#include <algorithm>
#include <ctime>
#include <fstream>
#include <iostream>
#include <string>
#define _USE_MATH_DEFINES
#include <math.h>

struct Params
{
    Bitmap* in;         // Указатель на изображение
    uint32_t startHeight; // Начальная высота области для размытия
    uint32_t endHeight;   // Конечная высота области для размытия
    uint32_t startWidth;  // Начальная ширина области для размытия
    uint32_t endWidth;    // Конечная ширина области для размытия
};

// Функция Blur выполняет размытие изображения
void Blur(int radius, Params* params)
{
    // Вычисление нескольких параметров для размытия
    float rs = ceil(radius * 3);
    float sigma = radius / 2.0;
    float sigmaSquare = sigma * sigma;
    float twoPiSigmaSquare = 2.0 * M_PI * sigmaSquare;

    // Перебор пикселей в области для размытия
    for (auto i = params->startHeight; i < params->endHeight; ++i)
    {
        for (auto j = params->startWidth; j < params->endWidth; ++j)
        {
            // Инициализация переменных для хранения результатов размытия
            double r = 0, g = 0, b = 0;
            double weightSum = 0;

            // Перебор пикселей в окрестности текущего пикселя
            for (int iy = i - rs; iy < i + rs + 1; ++iy)
            {
                for (int ix = j - rs; ix < j + rs + 1; ++ix)
                {
                    // Обработка граничных случаев, чтобы не выйти за границы изображения
                    auto x = min(static_cast<int>(params->endWidth) - 1, max(0, ix));
                    auto y = min(static_cast<int>(params->endHeight) - 1, max(0, iy));

                    // Вычисление квадрата расстояния между пикселями
                    float distanceSquare = ((ix - j) * (ix - j)) + ((iy - i) * (iy - i));
                    // Вычисление веса для текущего пикселя
                    float weight = exp(-distanceSquare / (2.0 * sigmaSquare)) / twoPiSigmaSquare;

                    // Получение цветового пикселя из изображения
                    rgb32* pixel = params->in->GetPixel(x, y);

                    // Накопление сумм цветовых компонент с весами
                    r += pixel->r * weight;
                    g += pixel->g * weight;
                    b += pixel->b * weight;
                    weightSum += weight;
                }
            }

            // Получение цветового пикселя из изображения для записи результата
            rgb32* pixel = params->in->GetPixel(j, i);

            // Запись размытого цвета в пиксель
            pixel->r = std::round(r / weightSum);
            pixel->g = std::round(g / weightSum);
            pixel->b = std::round(b / weightSum);
        }
    }
}


DWORD WINAPI StartThreads(CONST LPVOID lpParam)
{
    struct Params* params = (struct Params*)lpParam; // Получаем параметры изображения
    Blur(4, params); // Вызываем функцию размытия
    ExitThread(0); // Завершаем поток
}

// Функция StartThreads создает и запускает несколько потоков для обработки изображения
void StartThreads(Bitmap* in, int threadsCount, int coreCount)
{
    int partHeight = in->GetHeight() / threadsCount;
    int heightRemaining = in->GetHeight() % threadsCount;

    Params* arrayParams = new Params[threadsCount];
    for (int i = 0; i < threadsCount; i++)
    {
        Params params;
        params.in = in;
        params.startWidth = 0;
        params.endWidth = in->GetWidth();
        params.startHeight = partHeight * i;
        params.endHeight = partHeight * (i + 1) + ((i == threadsCount - 1) ? heightRemaining : 0);
        arrayParams[i] = params;
    }

    HANDLE* handles = new HANDLE[threadsCount];
    for (int i = 0; i < threadsCount; i++)
    {
        handles[i] = CreateThread(NULL, i, &StartThreads, &arrayParams[i], CREATE_SUSPENDED, NULL);
        SetThreadAffinityMask(handles[i], (1 << coreCount) - 1);
    }

    for (int i = 0; i < threadsCount; i++)
    {
        ResumeThread(handles[i]);
    }

    WaitForMultipleObjects(threadsCount, handles, true, INFINITE);

    delete[] arrayParams;
    delete[] handles;
}
  
int main(int argc, const char** argv)
{
    double start = clock();

    Bitmap bmp{ "2.bmp" };
    StartThreads(&bmp, 16, 4);
    bmp.Save("bluredImage.bmp");

    std::cout << (double)(clock() - start) / CLOCKS_PER_SEC << " s." << std::endl;

    return 0;
}