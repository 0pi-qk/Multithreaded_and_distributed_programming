#include "../include/inverse_MPI.h"
#include "../include/matrix_operation.h"

#define Debug false

// ~~~ ПАРАЛЛЕЛЬНОЕ ВЫЧИСЛЕНИЕ (MPI) ~~~

// Нахождение обратной матрицы с использованием метода Гаусса-Жордана
bool MPI::inverseMatrixParallelMPI(double** matrix, int size, int MPI_rank, int MPI_size) {
    double **tempMatrix = MATRIX::memoryAlloc(size , MATRIX::IDENTITY);
    MATRIX::generateMatrix(tempMatrix, size, MATRIX::IDENTITY);

    int rows_per_proc = size / MPI_size;
    int remainder = size % MPI_size;

    int start_row = MPI_rank * rows_per_proc + std::min(MPI_rank, remainder);
    int end_row = start_row + rows_per_proc + (MPI_rank < remainder ? 1 : 0);

    for (int j = 0; j < size; j++) {
        if (j >= start_row && j < end_row) {
            if (matrix[j][j] == 0)
                for (int k = j; k < size; k++)
                    if (matrix[k][j] != 0) {
                        MATRIX::memoryFree(tempMatrix, size);

                        return false;
                    }

            double div = matrix[j][j];
            for (int i = 0; i < size; i++) {
                matrix[j][i] /= div;
                tempMatrix[j][i] /= div;
            }
        }

        MPI_Bcast(matrix[j], size, MPI_DOUBLE, j / rows_per_proc, MPI_COMM_WORLD);
        MPI_Bcast(tempMatrix[j], size, MPI_DOUBLE, j / rows_per_proc, MPI_COMM_WORLD);

        for (int i = start_row; i < end_row; i++) {
            if (i == j) continue;

            double c = matrix[i][j];
            for (int k = 0; k < size; k++) {
                matrix[i][k] -= c * matrix[j][k];
                tempMatrix[i][k] -= c * tempMatrix[j][k];
            }
        }
    }

    // Вывод результата на каждом rank (отладка)
    for (int i = 0; i < MPI_size && Debug; i++) {
        MPI_Barrier(MPI_COMM_WORLD);

        if (MPI_rank == i) {
            std::cout<< "Rank = " << i << std::endl;
            MATRIX::printMatrix(tempMatrix, size);
            std::cout << std::endl;
        }
    }

    // Создание буфера для сбора всех строк матрицы
    auto* sendBuffer = new double[size * rows_per_proc];
    auto* recvBuffer = new double[size * size];

    // Заполнение буфера данными текущего процесса
    for (int i = 0; i < rows_per_proc; ++i)
        for (int j = 0; j < size; ++j)
            sendBuffer[i * size + j] = tempMatrix[start_row + i][j];

    MATRIX::memoryFree(tempMatrix, size);

    // Сборка всех строк матрицы на каждом процессе
    MPI_Allgather(sendBuffer, size * rows_per_proc, MPI_DOUBLE, recvBuffer, size * rows_per_proc, MPI_DOUBLE, MPI_COMM_WORLD);

    // Заполнение результатов в матрицу matrix
    for (int i = 0; i < size; ++i)
        for (int j = 0; j < size; ++j)
            matrix[i][j] = recvBuffer[i * size + j];

    // Освобождение памяти временных буферов
    delete[] sendBuffer;
    delete[] recvBuffer;

    return true;
}