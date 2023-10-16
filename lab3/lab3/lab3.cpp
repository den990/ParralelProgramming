#include <windows.h>
#include <string>
#include <iostream>
#include <fstream>
#include <ctime>
#include <vector>
#pragma comment(lib, "winmm.lib")
#define _CRT_SECURE_NO_WARNINGS

struct ThreadData {
    int threadNumber;
    FILE* outputFile;
};

DWORD WINAPI ThreadProc(CONST LPVOID lpParam)
{
    ThreadData* threadData = static_cast<ThreadData*>(lpParam);
    int threadNumber = threadData->threadNumber;
    int operations = 100;

    for (size_t i = 0; i < operations; ++i)
    {
        DWORD startTime = timeGetTime();

        Sleep(10);

        DWORD endTime = timeGetTime();
        double executionTime = endTime - startTime - 10;

        // Записываем время выполнения и номер потока в файл
        fprintf(threadData->outputFile, "%d|%lf\n", threadNumber, executionTime);
    }

    ExitThread(0);
}

int main(int argc, char* argv[])
{
    HANDLE handles[2];
    int threadNumbers[2] = { 1, 2 };

    FILE* outputFile;
    fopen_s(&outputFile, "output.txt", "w");
    std::vector<ThreadData> threadData(2);
    int a;
    std::cin >> a;

    for (int i = 0; i < 2; ++i)
    {
        threadData[i].threadNumber = threadNumbers[i];
        threadData[i].outputFile = outputFile;
        handles[i] = CreateThread(NULL, 0, &ThreadProc, &threadData[i], CREATE_SUSPENDED, NULL);
    }

    for (int i = 0; i < 2; ++i)
    {
        ResumeThread(handles[i]);
    }

    WaitForMultipleObjects(2, handles, TRUE, INFINITE);

    return 0;
}
