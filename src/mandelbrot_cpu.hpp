#pragma once

void mandelbrot_cpu(unsigned char *out, int width, int height, double x_min,
                    double x_max, double y_min, double y_max, int max_iter);