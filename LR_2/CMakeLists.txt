cmake_minimum_required(VERSION 3.25)
project(LR_2)

set(CMAKE_CXX_STANDARD 17)

find_package(OpenMP REQUIRED)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")

include_directories(include)

add_executable(LR_2
        src/inverse_OpenMP.cpp
        src/inverse_single.cpp
        src/matrix_operation.cpp
        src/menu.cpp
        include/inverse_OpenMP.h
        include/inverse_single.h
        include/matrix_operation.h)

target_link_libraries(LR_2 ${OpenMP_CXX_LIBRARIES})

