#!/bin/bash
#SBATCH --export=/usr/local/cuda/bin
#SBATCH --gres=gpu:1

cmake --build build

# Usage: ./build/release/mandelbrot [width height max_iter]
./build/release/mandelbrot 1920 1080 1000
