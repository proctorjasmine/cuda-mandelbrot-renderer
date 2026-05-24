#include "mandelbrot_cpu.hpp"

void mandelbrot_cpu(unsigned char *out, int width, int height, double x_min,
                    double x_max, double y_min, double y_max, int max_iter) {
    for (int row = 0; row < height; row++) {
        for (int col = 0; col < width; col++) {
            //compute complex coordinate
            double x0 = x_min + col * (x_max - x_min) / width;
            double y0 = y_min + row * (y_max - y_min) / height;

            //init z
            double x = 0.0; //real
            double y = 0.0; //imaginary
            int iter = 0;

            //z^2 = (x^2 - y^2) +(2xy)i

            //∣z∣=sqrt(x2+y2)
            //avoid using sqrt
            while ((x * x + y * y <= 4.0) && iter < max_iter) {
                double xtemp = x * x - y * y + x0;
                y = 2.0 * x * y + y0;
                x = xtemp;
                iter++;
            }

            //if iter == max then its probably in the set
            int idx = 3 * (row * width + col);

            if (iter == max_iter) {
                out[idx]     = 0; // R
                out[idx + 1] = 0; // G
                out[idx + 2] = 0; // B
            } else {
                unsigned char r = (unsigned char)(9 * (1.0 - (double)iter / max_iter) * 
                                                ((double)iter / max_iter) * 
                                                ((double)iter / max_iter) * 
                                                ((double)iter / max_iter) * 255);

                unsigned char g = (unsigned char)(15 * (1.0 - (double)iter / max_iter) * 
                                                (1.0 - (double)iter / max_iter) * 
                                                ((double)iter / max_iter) * 
                                                ((double)iter / max_iter) * 255);

                unsigned char b = (unsigned char)(8.5 * (1.0 - (double)iter / max_iter) * 
                                                (1.0 - (double)iter / max_iter) * 
                                                (1.0 - (double)iter / max_iter) * 
                                                ((double)iter / max_iter) * 255);



                //switched colors to see different visual output
                out[idx]     = b;
                out[idx + 1] = g;
                out[idx + 2] = r;
            }
        }
    }
}