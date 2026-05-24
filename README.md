# CUDA Mandelbrot Renderer

GPU-accelerated Mandelbrot fractal renderer using CUDA.

This project generates Mandelbrot set images and compares CPU execution against CUDA GPU acceleration. The goal was to study how well Mandelbrot generation maps to parallel GPU execution, test different CUDA kernel configurations, and analyze performance using NVIDIA Nsight Compute.

## Features

- Mandelbrot set image generation
- CPU baseline implementation
- CUDA GPU implementation
- One-thread-per-pixel parallel rendering
- Adjustable image window and zoom region
- Float and double precision kernel comparison
- CUDA block size testing
- NVIDIA Nsight Compute profiling
- Custom fractal image output

## Background

The Mandelbrot set is generated using the recurrence:

```text
z(n+1) = z(n)^2 + c
```

The algorithm starts with `z = 0`. For each pixel, `c` is treated as a complex number mapped from the image coordinate. The value is repeatedly updated until either:

- `|z| > 2`, meaning the point escapes
- the maximum iteration count is reached

The number of iterations before escape is used to determine the pixel color.

Mandelbrot generation is a good fit for GPU acceleration because each pixel can be calculated independently.

## How It Works

1. Map each pixel to a complex coordinate.
2. Start with `z = 0`.
3. Iterate `z(n+1) = z(n)^2 + c`.
4. Stop when the value escapes or reaches the iteration limit.
5. Store the iteration count for that pixel.
6. Convert the iteration count into a color.

## CUDA Approach

The baseline CUDA kernel uses a simple one-thread-per-pixel design.

Each CUDA thread calculates:

```text
col = blockIdx.x * blockDim.x + threadIdx.x
row = blockIdx.y * blockDim.y + threadIdx.y
```

The thread then maps that pixel to the complex plane and runs the Mandelbrot iteration loop.

This approach is straightforward and effective because there is very little communication between pixels. Each pixel can be computed independently, allowing the GPU to process many pixels in parallel.

## Performance Results

### Baseline Window

Generation window:

```text
x_min = -2.0
x_max = 1.0
y_min = -1.5
y_max = 1.5
```

| Version | Runtime |
|---|---|
| CPU | ~2000 ms |
| GPU double | ~170 ms |
| Average speedup | ~11.62x |

### Boundary Region Window

Generation window:

```text
x_min = -0.85
x_max = -0.65
y_min = 0.0
y_max = 0.2
```

| Version | Runtime |
|---|---|
| CPU | ~9500 ms |
| GPU double | ~515 ms |
| Average speedup | ~18.5x |

## Optimization Results

Several CUDA block sizes were tested, including:

- `(8, 8)`
- `(16, 16)`
- `(32, 8)`
- `(32, 16)`

Block size tuning had only a small effect on runtime. The largest performance improvement came from switching the Mandelbrot calculations from `double` to `float`.

For the baseline window:

| Version | Runtime |
|---|---|
| CPU | ~2026 ms |
| GPU double | ~172 ms |
| GPU float | ~6.8 ms |

Using `float` resulted in roughly a 25x speedup over the double-precision GPU version and around a 300x speedup over the CPU baseline.

## Nsight Compute Analysis

NVIDIA Nsight Compute showed that the double-precision version was limited by FP64 arithmetic throughput. The kernel was compute-bound rather than memory-bound, so optimizations like shared memory tiling were not very helpful.

Key findings:

- Double precision heavily used the FP64 pipeline.
- Double precision compute throughput was around 85%.
- Float precision used FP32 units instead.
- Float precision compute throughput was around 97%.
- The main bottleneck was arithmetic performance, not memory access.


## Build

```bash
cmake -B build
cmake --build build
```

## Run

```bash
./build/release/mandelbrot
```

Example with custom parameters:

```bash
./build/release/mandelbrot 1920 1080 1000
```

On the GPU for the lab machine, I use:

```bash
sbatch run_mandelbrot.sh
```

## Conclusions

This project showed that GPU acceleration provides a major performance improvement for Mandelbrot fractal generation. A simple one-thread-per-pixel CUDA kernel already produced significant speedup over the CPU baseline.

The most important optimization was switching from double precision to float precision. Nsight Compute confirmed that the double version was limited by FP64 arithmetic throughput, while the float version used FP32 units more efficiently.

## Future Improvements

- Add more zoom presets
- Add smoother color gradients
- Add command-line options for image bounds
- Add generated output images to the README
- Create an animated Mandelbrot zoom

## Notes

This repository is a cleaned and documented version of a GPU programming project focused on CUDA parallelism, Mandelbrot fractal generation, optimization, and performance profiling.

## Author

Jasmine Proctor
