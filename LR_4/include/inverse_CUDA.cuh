#ifndef LR_4_INVERSE_CUDA_CUH
#define LR_4_INVERSE_CUDA_CUH

#include "cuda_runtime.h"
#include "device_launch_parameters.h"

namespace CUDA {
    __global__ void noZeroInverseMatrix(double*, int, int);
    __global__ void normalizeInverseMatrix(double*, double*, int, int);
    __global__ void calculateInverseMatrix(double*, double*, int, int);
    bool inverseMatrixParallelCUDA(double**, int, int);
}

#endif
