#include "../include/inverse_OpenMP.h"

using namespace std;

// ~~~ ПАРАЛЛЕЛЬНОЕ ВЫЧИСЛЕНИЕ (OpenMP) ~~~

// Функция для выполнения элементарных преобразований над строками матрицы
void OpenMP::rowOperationOpenmp(double* matrix, int size, int row1, int row2, double factor, int threads) {
#pragma omp parallel for shared(matrix, size, row1, row2, factor) default(none) num_threads(threads)
    for (int i = 0; i < size; i++)
        matrix[row1 * size + i] -= matrix[row2 * size + i] * factor;
}

// Функция для нахождения обратной матрицы OpenMP
bool OpenMP::inverseMatrixParallelOpenmp(double* matrix, int size, int threads) {
    // Создаем расширенную матрицу, к исходной матрице добавляется единичная матрица справа
    auto* extendedMatrix = new double[size * size * 2];

#pragma omp parallel for shared(extendedMatrix, matrix, size) default(none) num_threads(threads)
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++)
            extendedMatrix[i * size * 2 + j] = matrix[i * size + j];

        for (int j = size; j < size * 2; j++) {
            if (i == j - size)
                extendedMatrix[i * size * 2 + j] = 1.0;
            else
                extendedMatrix[i * size * 2 + j] = 0.0;
        }
    }

    // Приводим к верхнетреугольному виду
    for (int i = 0; i < size; i++) {
        if (extendedMatrix[i * size * 2 + i] == 0) {
            // Если на главной диагонали встретился ноль, ищем ненулевой элемент ниже и меняем строки
            bool degenerateMatrix = true;

            for (int k = i + 1; k < size; k++) {
                if (extendedMatrix[k * size * 2 + i] != 0) {
                    swap_ranges(extendedMatrix + i * size * 2, extendedMatrix + (i + 1) * size * 2, extendedMatrix + k * size * 2);
                    degenerateMatrix = false;
                    break;
                }
            }

            if (!degenerateMatrix) {
                delete[] extendedMatrix;

                return false; // Матрица вырожденная
            }

        }

#pragma omp parallel for default(none) shared(extendedMatrix, size, threads, i) num_threads(threads)
        for (int j = i + 1; j < size; j++) {
            double factor = extendedMatrix[j * size * 2 + i] / extendedMatrix[i * size * 2 + i];
            OpenMP::rowOperationOpenmp(extendedMatrix, size * 2, j, i, factor, threads);
        }
    }

    // Приводим к диагональному виду
    for (int i = size - 1; i > 0; i--) {
        for (int j = i - 1; j >= 0; j--) {
            double factor = extendedMatrix[j * size * 2 + i] / extendedMatrix[i * size * 2 + i];
            OpenMP::rowOperationOpenmp(extendedMatrix, size * 2, j, i, factor, threads);
        }
    }

    // Нормируем строки, чтобы на диагонали были единицы
#pragma omp parallel for shared(extendedMatrix, size) default(none) num_threads(threads)
    for (int i = 0; i < size; i++) {
        double divisor = extendedMatrix[i * size * 2 + i];

        for (int j = 0; j < size * 2; j++)
            extendedMatrix[i * size * 2 + j] /= divisor;
    }

    // Копируем обратную матрицу в результирующий массив
#pragma omp parallel for collapse(2) default(none) shared(extendedMatrix, matrix, size) num_threads(threads)
    for (int i = 0; i < size; i++)
        for (int j = 0; j < size; j++)
            matrix[i * size + j] = extendedMatrix[i * size * 2 + size + j];

    delete[] extendedMatrix;
    return true;
}
