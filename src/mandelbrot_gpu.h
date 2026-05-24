#pragma once

void mandelbrot_gpu(unsigned char *host_out,
                    int width,
                    int height,
                    double x_min,
                    double x_max,
                    double y_min,
                    double y_max,
                    int max_iter);

void mandelbrot_gpu_float(unsigned char *host_out,
                          int width,
                          int height,
                          double x_min,
                          double x_max,
                          double y_min,
                          double y_max,
                          int max_iter);