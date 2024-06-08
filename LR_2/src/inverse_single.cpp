#include "../include/inverse_single.h"

using namespace std;

// ~~~ ПОСЛЕДОВАТЕЛЬНОЕ ВЫЧИСЛЕНИЕ ~~~

// Функция для выполнения элементарных преобразований над строками матрицы
void SINGLE::rowOperationSequential(double* matrix, int size, int row1, int row2, double factor) {
    for (int i = 0; i < size; i++)
        matrix[row1 * size + i] -= matrix[row2 * size + i] * factor;
}

// Функция для последовательного вычисления обратной матрицы
bool SINGLE::inverseMatrixSequential(double* matrix, int size) {
    // Создаем расширенную матрицу, к исходной матрице добавляется единичная матрица справа
    double* extendedMatrix = new double[size * size * 2];

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
            bool degenerateMatrix = false;

            for (int k = i + 1; k < size; k++)
                if (extendedMatrix[k * size * 2 + i] != 0) {
                    swap_ranges(extendedMatrix + i * size * 2, extendedMatrix + (i + 1) * size * 2, extendedMatrix + k * size * 2);
                    degenerateMatrix = true;
                    break;
                }

            if (!degenerateMatrix) {
                delete[] extendedMatrix;

                return false; // Матрица вырожденная
            }
        }

        for (int j = i + 1; j < size; j++) {
            double factor = extendedMatrix[j * size * 2 + i] / extendedMatrix[i * size * 2 + i];
            SINGLE::rowOperationSequential(extendedMatrix, size * 2, j, i, factor);
        }
    }

    // Приводим к диагональному виду
    for (int i = size - 1; i > 0; i--) {
        for (int j = i - 1; j >= 0; j--) {
            double factor = extendedMatrix[j * size * 2 + i] / extendedMatrix[i * size * 2 + i];
            SINGLE::rowOperationSequential(extendedMatrix, size * 2, j, i, factor);
        }
    }

    // Нормируем строки, чтобы на диагонали были единицы
    for (int i = 0; i < size; i++) {
        double divisor = extendedMatrix[i * size * 2 + i];

        for (int j = 0; j < size * 2; j++)
            extendedMatrix[i * size * 2 + j] /= divisor;
    }

    // Копируем обратную матрицу в результирующий массив
    for (int i = 0; i < size; i++)
        for (int j = 0; j < size; j++)
            matrix[i * size + j] = extendedMatrix[i * size * 2 + size + j];

    delete[] extendedMatrix;
    return true;
}
