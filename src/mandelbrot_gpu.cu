#include "mandelbrot_gpu.h"

#include <stdio.h>
#include <stdlib.h>
#include <cuda_runtime.h>

#define gpuErrchk(ans) { gpuAssert((ans), __FILE__, __LINE__); }

inline void gpuAssert(cudaError_t code, const char *file, int line, bool abort=true) {
   if (code != cudaSuccess) {
      fprintf(stderr,"GPUassert: %s %s %d\n", cudaGetErrorString(code), file, line);
      if (abort) exit(code);
   }
}

__global__ 
void mandelbrot_kernel(unsigned char *out, int width, int height, double x_min,
                                double x_max, double y_min, double y_max, int max_iter) {
    int col = blockIdx.x * blockDim.x + threadIdx.x;
    int row = blockIdx.y * blockDim.y + threadIdx.y;

    if (row >= height || col >= width) {
        return;
    }

    double x0 = x_min + col * (x_max - x_min) / width;
    double y0 = y_min + row * (y_max - y_min) / height;

    double x = 0.0;
    double y = 0.0;
    int iter = 0;

    while ((x * x + y * y <= 4.0) && iter < max_iter) {
        double xtemp = x * x - y * y + x0;
        y = 2.0 * x * y + y0;
        x = xtemp;
        iter++;
    }

    // out[row * width + col] =
    //     (iter == max_iter) ? 0 : (unsigned char)(255.0 * iter / max_iter);out[row * width + col] =
    //     (iter == max_iter) ? 0 : (unsigned char)(255.0 * iter / max_iter);
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

        out[idx]     = r;
        out[idx + 1] = g;
        out[idx + 2] = b;
    }

}

__global__ void mandelbrot_kernel_float(unsigned char *out,int width,int height,float x_min,
                                        float x_max,float y_min,float y_max, int max_iter) {
    int col = blockIdx.x * blockDim.x + threadIdx.x;
    int row = blockIdx.y * blockDim.y + threadIdx.y;

    if (row >= height || col >= width) return;

    float x0 = x_min + col * (x_max - x_min) / width;
    float y0 = y_min + row * (y_max - y_min) / height;

    float x = 0.0f;
    float y = 0.0f;
    int iter = 0;

    while ((x * x + y * y <= 4.0f) && iter < max_iter) {
        float xtemp = x * x - y * y + x0;
        y = 2.0f * x * y + y0;
        x = xtemp;
        iter++;
    }

    int idx = 3 * (row * width + col);

    if (iter == max_iter) {
        out[idx] = out[idx+1] = out[idx+2] = 0;
    } else {
        float t = (float)iter / max_iter;

        out[idx]     = (unsigned char)(9 * (1 - t) * t * t * t * 255);
        out[idx + 1] = (unsigned char)(15 * (1 - t)*(1 - t)*t*t * 255);
        out[idx + 2] = (unsigned char)(8.5 * (1 - t)*(1 - t)*(1 - t)*t * 255);
    }
}


void mandelbrot_gpu(unsigned char *host_out, int width, int height, double x_min,
                    double x_max, double y_min, double y_max, int max_iter) {
    unsigned char *device_out;
    size_t image_size = width * height * 3 * sizeof(unsigned char);

    gpuErrchk(cudaMalloc((void **)&device_out, image_size));

    dim3 block_size(16, 16);
    //dim3 block_size(8, 8);
    //dim3 block_size(32, 8);
    //dim3 block_size(32, 16);

    dim3 grid_size((width + block_size.x - 1) / block_size.x,
                   (height + block_size.y - 1) / block_size.y);

    mandelbrot_kernel<<<grid_size, block_size>>>(
        device_out, width, height,
        x_min, x_max, y_min, y_max,
        max_iter
    );

    gpuErrchk(cudaDeviceSynchronize());

    gpuErrchk(cudaMemcpy(host_out, device_out, image_size, cudaMemcpyDeviceToHost));

    gpuErrchk(cudaFree(device_out));
}


void mandelbrot_gpu_float(unsigned char *host_out,
                          int width,
                          int height,
                          double x_min,
                          double x_max,
                          double y_min,
                          double y_max,
                          int max_iter) {

    unsigned char *device_out;
    size_t image_size = width * height * 3;

    cudaMalloc(&device_out, image_size);

    dim3 block_size(16, 16);
    dim3 grid_size((width + 15) / 16, (height + 15) / 16);

    mandelbrot_kernel_float<<<grid_size, block_size>>>(
        device_out,
        width, height,
        (float)x_min, (float)x_max,
        (float)y_min, (float)y_max,
        max_iter
    );

    cudaDeviceSynchronize();

    cudaMemcpy(host_out, device_out, image_size, cudaMemcpyDeviceToHost);

    cudaFree(device_out);
}