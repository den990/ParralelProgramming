#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>

using namespace std;

#pragma pack(push, 1)
struct Pixel {
    uint8_t blue;
    uint8_t green;
    uint8_t red;
};
#pragma pack(pop)

// Функция для загрузки изображения BMP
vector<Pixel> loadBMP(const char* filename, int& width, int& height) {
    vector<Pixel> image;
    ifstream file(filename, ios::binary);

    if (!file) {
        cerr << "Не удалось открыть файл: " << filename << endl;
        return image;
    }

    char header[54];
    file.read(header, 54);
    width = *(int*)&header[18];
    height = *(int*)&header[22];

    int padding = (4 - (width * sizeof(Pixel)) % 4) % 4;
    image.resize(width * height);
    for (int row = height - 1; row >= 0; --row) {
        file.read(reinterpret_cast<char*>(&image[row * width]), width * sizeof(Pixel));
        file.seekg(padding, ios::cur);
    }

    return image;
}

// Функция для сохранения изображения BMP
void saveBMP(const char* filename, const vector<Pixel>& image, int width, int height) {
    ofstream file(filename, ios::binary);

    if (!file) {
        cerr << "Не удалось создать файл: " << filename << endl;
        return;
    }

    int padding = (4 - (width * sizeof(Pixel)) % 4) % 4;
    int fileSize = 54 + (width * sizeof(Pixel) + padding) * height;

    char header[54] = {
        0x42, 0x4D,                    // BMP signature
        0, 0, 0, 0,                    // File size
        0, 0, 0, 0,                    // Reserved
        54, 0, 0, 0,                   // Data offset
        40, 0, 0, 0,                   // Header size
        static_cast<char>(width), 0, 0, 0,  // Image width
        static_cast<char>(width >> 8), 0, 0, 0,
        static_cast<char>(width >> 16), 0, 0, 0,
        static_cast<char>(width >> 24), 0, 0, 0,
        static_cast<char>(height), 0, 0, 0, // Image height
        static_cast<char>(height >> 8), 0, 0, 0,
        static_cast<char>(height >> 16), 0, 0, 0,
        static_cast<char>(height >> 24), 0, 0, 0,
        1, 0,                         // Planes
        24, 0,                        // Bits per pixel (24-bit RGB)
        0, 0, 0, 0,                    // Compression (none)
        0, 0, 0, 0,                    // Image size
        0, 0, 0, 0,                    // X pixels per meter
        0, 0, 0, 0,                    // Y pixels per meter
        0, 0, 0, 0,                    // Colors used
        0, 0, 0, 0                    // Important colors
    };

    // Записываем заголовок BMP
    file.write(header, 54);

    // Записываем пиксели изображения
    for (int row = height - 1; row >= 0; --row) {
        file.write(reinterpret_cast<const char*>(&image[row * width]), width * sizeof(Pixel));
        file.seekp(padding, ios::cur);
    }
}

// Функция для применения эффекта размытия
void applyBlur(vector<Pixel>& image, int width, int height, int blurRadius) {
    vector<Pixel> blurredImage = image;

    for (int row = 0; row < height; ++row) {
        for (int col = 0; col < width; ++col) {
            int sumR = 0, sumG = 0, sumB = 0, count = 0;

            for (int i = -blurRadius; i <= blurRadius; ++i) {
                for (int j = -blurRadius; j <= blurRadius; ++j) {
                    int newRow = row + i;
                    int newCol = col + j;

                    if (newRow >= 0 && newRow < height && newCol >= 0 && newCol < width) {
                        sumR += image[newRow * width + newCol].red;
                        sumG += image[newRow * width + newCol].green;
                        sumB += image[newRow * width + newCol].blue;
                        count++;
                    }
                }
            }

            blurredImage[row * width + col].red = sumR / count;
            blurredImage[row * width + col].green = sumG / count;
            blurredImage[row * width + col].blue = sumB / count;
        }
    }

    image = blurredImage;
}

int main(int argc, char** argv) {
    if (argc != 5) {
        cout << "Использование: " << argv[0] << " <input.bmp> <output.bmp> <размер размытия> <количество потоков>" << endl;
        return -1;
    }

    const char* inputFileName = argv[1];
    const char* outputFileName = argv[2];
    int blurRadius = atoi(argv[3]);
    int numThreads = atoi(argv[4]);

    int width, height;
    vector<Pixel> image = loadBMP(inputFileName, width, height);

    if (image.empty()) {
        return -1;
    }

    int stripHeight = height / numThreads;
#pragma omp parallel for num_threads(numThreads)
    for (int i = 0; i < numThreads; ++i) {
        int startRow = i * stripHeight;
        int endRow = (i + 1) * stripHeight;
        applyBlur(image, width, height, blurRadius, startRow, endRow);
    }

    saveBMP(outputFileName, image, width, height);
    cout << "Изображение сохранено в " << outputFileName << endl;

    return 0;
}
