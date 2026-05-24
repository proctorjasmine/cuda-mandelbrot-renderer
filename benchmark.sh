#!/bin/bash
#SBATCH --export=/usr/local/cuda/bin
#SBATCH --gres=gpu:1

export TMPDIR=$HOME/tmp/ncu-lock

mkdir -p $HOME/tmp/ncu-lock
export TMPDIR=$HOME/tmp/ncu-lock

#double version
# ncu --set full -o mandelbrot_double -f \
# ./build/release/mandelbrot 1920 1080 1000

#float version
ncu --set full -o mandelbrot_float -f \
./build/release/mandelbrot 1920 1080 1000