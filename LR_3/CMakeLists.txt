cmake_minimum_required(VERSION 3.25)
project(LR_3)

set(CMAKE_CXX_STANDARD 17)

add_executable(LR_3
        src/inverse_MPI.cpp
        src/inverse_single.cpp
        src/matrix_operation.cpp
        src/menu.cpp
        include/inverse_MPI.h
        include/inverse_single.h
        include/matrix_operation.h)

find_package(MPI REQUIRED)
target_link_libraries(LR_3 PRIVATE MPI::MPI_CXX)