#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <fstream>
#include <vector>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <algorithm>

int main() {
    std::ifstream file("dump_13122019_145433.bin", std::ios::binary);
    if (!file) {
        std::cerr << "Error when opening file.\n";
        return 1;
    }

    // Пропуск заголовка
    file.seekg(32, file.beg);

    // Задание размеров кадра
    const int width = 640, height = 512;
    const int frameSize = width * height;
    std::vector<uint16_t> buffer(frameSize);

    // Инициализация videowriter
    cv::VideoWriter videoWriter("output.mp4", cv::VideoWriter::fourcc('M', 'P', '4', 'V'), 10, cv::Size(width, height), true);

    if (!videoWriter.isOpened()) {
        std::cerr << "Failed to create video file.\n";
        return 1;
    }

    // Чтение кадров
    for (int frame = 0; frame < 1000 && file.read(reinterpret_cast<char*>(buffer.data()), frameSize * sizeof(uint16_t)); frame++) {

        // Нахождение минимума и максимума
        uint16_t minVal = *std::min_element(buffer.begin(), buffer.end());
        uint16_t maxVal = *std::max_element(buffer.begin(), buffer.end());

        // Создание изображения
        cv::Mat image(height, width, CV_8UC3);
        for (int i = 0; i < height; ++i) {
            for (int j = 0; j < width; ++j) {
                // Преобразование значения пикселя
                float normPixel = static_cast<float>(buffer[i * width + j] - minVal) / (maxVal - minVal);
                normPixel = pow(normPixel, 1 / 2.2); // Гамма-коррекция
                uchar pixelValue = static_cast<uchar>(normPixel * 255);

                // Установка значения пикселя в изображении
                image.at<cv::Vec3b>(i, j) = cv::Vec3b(pixelValue, pixelValue, pixelValue);
            }
        }

        // Запись преобразованного кадра
        videoWriter.write(image);

    }

    file.close();
    return 0;
    videoWriter.release();
}