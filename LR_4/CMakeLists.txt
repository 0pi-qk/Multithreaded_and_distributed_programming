cmake_minimum_required(VERSION 3.25)
project(LR_4 LANGUAGES CUDA CXX)

set(CMAKE_CUDA_STANDARD 17)
set(CMAKE_CXX_STANDARD 17)

add_executable(LR_4
        include/inverse_CUDA.cuh
        include/inverse_single.h
        include/matrix_operation.h
        src/inverse_CUDA.cu
        src/inverse_single.cpp
        src/matrix_operation.cpp
        src/menu.cpp
        )

set_target_properties(LR_4 PROPERTIES
        CUDA_SEPARABLE_COMPILATION ON
        CXX_STANDARD 17
        )
