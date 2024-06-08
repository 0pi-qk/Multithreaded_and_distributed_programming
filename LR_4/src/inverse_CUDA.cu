#include "../include/inverse_CUDA.cuh"
#include "../include/matrix_operation.h"

// ~~~ ПАРАЛЛЕЛЬНОЕ ВЫЧИСЛЕНИЕ (CUDA) ~~~

__global__ void CUDA::noZeroInverseMatrix(double* matrix, int size, int row) {
    int i = blockIdx.x * blockDim.x + threadIdx.x;
    int j = blockIdx.y * blockDim.y + threadIdx.y;

    if (i < size && j < size && i == row)
        if (matrix[i * size + i] == 0)
            for (int k = i + 1; k < size; k++)
                if (matrix[k * size + i] != 0.0) {
                    matrix[i * size + j] += matrix[k * size + j];
                    return;
                }
}

__global__ void CUDA::normalizeInverseMatrix(double* matrix, double* identityMatrix, int size, int row) {
    int i = blockIdx.x * blockDim.x + threadIdx.x;
    int j = blockIdx.y * blockDim.y + threadIdx.y;

    if (i < size && j < size) {
        if (i == row && i != j) {
            identityMatrix[i * size + j] /= matrix[i * size + i];
            matrix[i * size + j] /= matrix[i * size + i];
        }

        __syncthreads();

        if (i == row && i == j) {
            identityMatrix[i * size + j] /= matrix[i * size + i];
            matrix[i * size + j] /= matrix[i * size + i];
        }
    }
}

__global__ void CUDA::calculateInverseMatrix(double* matrix, double* identityMatrix, int size, int row) {
    int i = blockIdx.x * blockDim.x + threadIdx.x;
    int j = blockIdx.y * blockDim.y + threadIdx.y;

    if (i < size && j < size)
        if (i != row) {
            identityMatrix[i * size + j] -= identityMatrix[row * size + j] * matrix[i * size + row];

            if (j != row)
                matrix[i * size + j] -= matrix[row * size + j] * matrix[i * size + row];
        }
}

// Нахождение обратной матрицы с использованием метода Гаусса-Жордана
bool CUDA::inverseMatrixParallelCUDA(double** matrix, int size, int threadsInBlock) {
    // Подготовка конфигурации CUDA
    dim3 numBlocks(threadsInBlock, threadsInBlock);
    dim3 blockSize((size + threadsInBlock - 1) / threadsInBlock, (size + threadsInBlock - 1) / threadsInBlock);

    // Выделение памяти для результирующей матрицы и её инициализация как единичной матрицы
    double **resultMatrix = MATRIX::memoryAlloc(size);
    MATRIX::generateMatrix(resultMatrix, size, MATRIX::IDENTITY);

    // Подготовка памяти на хосте
    int size_bytes = size * size * sizeof(double);
    auto* h_A = new double[size * size];
    auto* h_I = new double[size * size];

    // Копирование исходной и единичной матриц в одномерные массивы для параллельных вычислений
    MATRIX::converterMatrix(matrix, h_A, size, true);
    MATRIX::converterMatrix(resultMatrix, h_I, size, true);

    // Выделение памяти на устройстве
    double* d_A;
    double* d_I;
    cudaMalloc((void**)&d_A, size_bytes);
    cudaMalloc((void**)&d_I, size_bytes);

    // Копирование данных из хоста на устройство
    cudaMemcpy(d_A, h_A, size_bytes, cudaMemcpyHostToDevice);
    cudaMemcpy(d_I, h_I, size_bytes, cudaMemcpyHostToDevice);

    // Запуск параллельного алгоритма
    for (int row = 0; row < size; row++) {
        // Обработка строки, чтобы главный элемент был ненулевым
        CUDA::noZeroInverseMatrix<<<blockSize, numBlocks>>>(d_A, size, row);
        cudaDeviceSynchronize();

        // Нормализация строки
        CUDA::normalizeInverseMatrix<<<blockSize, numBlocks>>>(d_A, d_I, size, row);
        cudaDeviceSynchronize();

        // Вычитание текущей строки из остальных строк
        CUDA::calculateInverseMatrix<<<blockSize, numBlocks>>>(d_A, d_I, size, row);
        cudaDeviceSynchronize();
    }

    // Копирование результатов обратно на хост
    cudaMemcpy(h_A, d_A, size_bytes, cudaMemcpyDeviceToHost);
    cudaMemcpy(h_I, d_I, size_bytes, cudaMemcpyDeviceToHost);

    // Конвертирование результатов в двумерный массив
    MATRIX::converterMatrix(matrix, h_I, size, false);

    // Освобождение памяти, выделенной для результирующей матрицы
    MATRIX::memoryFree(resultMatrix, size);

    // Освобождение памяти на устройстве
    cudaFree(d_A);
    cudaFree(d_I);

    // Освобождение памяти на хосте
    delete[] h_A;
    delete[] h_I;

    return true;
}