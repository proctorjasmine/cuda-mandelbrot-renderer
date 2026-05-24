cmake_minimum_required(VERSION 3.18 FATAL_ERROR)

project(mandelbrot_project LANGUAGES CXX CUDA)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(CUDA REQUIRED)
find_package(OpenCV REQUIRED)


set(SRC_PATH "${CMAKE_CURRENT_SOURCE_DIR}/src")

add_executable(mandelbrot
    "${SRC_PATH}/main.cpp"
    "${SRC_PATH}/mandelbrot_cpu.cpp"
    "${SRC_PATH}/mandelbrot_gpu.cu"
)

target_include_directories(mandelbrot PRIVATE
    "${SRC_PATH}"
    ${OpenCV_INCLUDE_DIRS}
)

target_link_libraries(mandelbrot
    ${OpenCV_LIBS}
)

set_target_properties(mandelbrot
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/release"
)