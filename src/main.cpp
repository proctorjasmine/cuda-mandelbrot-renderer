#include "mandelbrot_cpu.hpp"
#include "mandelbrot_gpu.h"
#include <opencv2/opencv.hpp>

#include <cstdio>
#include <cstdlib>
#include <chrono>


static void write_png(const char *file_name,
                      unsigned char *data,
                      int width,
                      int height) {
    cv::Mat img(height, width, CV_8UC3, data);

    if (!cv::imwrite(file_name, img)) {
        fprintf(stderr, "Error: could not write output file %s\n", file_name);
        exit(EXIT_FAILURE);
    }
}


int main(int argc, char **argv) {
    int width = 1920;
    int height = 1080;
    int max_iter = 1000;

    if (argc == 4) {
        width = atoi(argv[1]);
        height = atoi(argv[2]);
        max_iter = atoi(argv[3]);
    } else if (argc != 1) {
        fprintf(stderr, "Usage: %s [width height max_iter]\n", argv[0]);
        return EXIT_FAILURE;
    }

    double x_min, x_max, y_min, y_max;
    // Standard Mandelbrot viewing window.
    // x_min = -2.0;
    // x_max = 1.0;
    // y_min = -1.5;
    // y_max = 1.5;

    //slight zoom
    // x_min = -1.0;
    // x_max = 0.5;
    // y_min = -0.75;
    // y_max = 0.75;

    //boundary region
    // x_min = -0.85;
    // x_max = -0.65;
    // y_min = 0.0;
    // y_max = 0.2;

    //deep zoom
    // x_min = -0.75;
    // x_max = -0.70;
    // y_min = 0.10;
    // y_max = 0.15;

    //more zoom
    // x_min = -0.7437;
    // x_max = -0.7435;
    // y_min = 0.1317;
    // y_max = 0.1319;

    //spiral spotting
    x_min = -0.75;
    x_max = -0.73;
    y_min = 0.1;
    y_max = 0.12;

    //cool
    // x_min = 0.25;
    // x_max = 0.45;
    // y_min = -0.1;
    // y_max = 0.1;

    size_t image_size = width * height * 3 * sizeof(unsigned char);
    unsigned char *cpu_output = (unsigned char *)malloc(image_size);
    unsigned char *gpu_output = (unsigned char *)malloc(image_size);
    unsigned char *gpu_output_float = (unsigned char *)malloc(image_size);

    if (!cpu_output || !gpu_output) {
        fprintf(stderr, "Error: host allocation failed\n");
        free(cpu_output);
        free(gpu_output);
        return EXIT_FAILURE;
    }

    printf("Generating Mandelbrot image: %d x %d, max_iter = %d\n", width, height, max_iter);

    auto cpu_start = std::chrono::high_resolution_clock::now();
    mandelbrot_cpu(cpu_output, width, height, x_min, x_max, y_min, y_max, max_iter);
    auto cpu_end = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double, std::milli> cpu_ms = cpu_end - cpu_start;
    printf("CPU time: %.3f ms\n", cpu_ms.count());

    auto gpu_start = std::chrono::high_resolution_clock::now();
    mandelbrot_gpu(gpu_output, width, height, x_min, x_max, y_min, y_max, max_iter);
    auto gpu_end = std::chrono::high_resolution_clock::now();

    auto gpu_start_f = std::chrono::high_resolution_clock::now();
    mandelbrot_gpu_float(gpu_output_float, width, height, x_min, x_max, y_min, y_max, max_iter);
    auto gpu_end_f = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double, std::milli> gpu_ms = gpu_end - gpu_start;
    std::chrono::duration<double, std::milli> gpu_ms_f = gpu_end_f - gpu_start_f;

    printf("GPU double time: %.3f ms\n", gpu_ms.count());
    printf("Double speedup over CPU: %.2fx\n", cpu_ms.count() / gpu_ms.count());

    printf("GPU float time: %.3f ms\n", gpu_ms_f.count());
    printf("Float speedup over CPU: %.2fx\n", cpu_ms.count() / gpu_ms_f.count());
    printf("Float speedup over double GPU: %.2fx\n", gpu_ms.count() / gpu_ms_f.count());


    write_png("mandelbrot_cpu.png", cpu_output, width, height);
    write_png("mandelbrot_gpu.png", gpu_output, width, height);
    write_png("mandelbrot_gpu_float.png", gpu_output_float, width, height);
    printf("Wrote mandelbrot_cpu.png, mandelbrot_gpu_float.png, and mandelbrot_gpu.png\n");

    free(cpu_output);
    free(gpu_output);
    free(gpu_output_float);

    return 0;
}
