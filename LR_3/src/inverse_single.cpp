#include "../include/inverse_single.h"
#include "../include/matrix_operation.h"

// ~~~ ПОСЛЕДОВАТЕЛЬНОЕ ВЫЧИСЛЕНИЕ ~~~

// Нахождение обратной матрицы с использованием метода Гаусса-Жордана
bool SINGLE::inverseMatrixSequential(double** matrix, int size) {
    // Выделение памяти под матрицу результата и инициализация ее как единичную
    double **resultMatrix = MATRIX::memoryAlloc(size , MATRIX::IDENTITY);
    MATRIX::generateMatrix(resultMatrix, size, MATRIX::IDENTITY);

    // Проход по диагонали матрицы
    for (int j = 0; j < size; j++) {
        // Проверка, что элемент на диагонали не равен нулю
        if (matrix[j][j] == 0)
            for (int k = j; k < size; k++)
                if (matrix[k][j] != 0) {
                    MATRIX::memoryFree(resultMatrix, size);

                    return false; // Матрица вырожденная
                }

        // Нормализация строки по диагонали (деление всех элементов строки на диагональный элемент)
        double div = matrix[j][j];
        for (int i = 0; i < size; i++) {
            matrix[j][i] /= div;
            resultMatrix[j][i] /= div;
        }

        // Обнуление всех элементов под и над текущей диагональю
        for (int i = 0; i < size; i++) {
            if (i == j)
                continue;

            double c = matrix[i][j];

            for (int k = 0; k < size; k++) {
                matrix[i][k] -= c * matrix[j][k];
                resultMatrix[i][k] -= c * resultMatrix[j][k];
            }
        }
    }

    // Копирование результата в исходную матрицу
    MATRIX::copyMatrix(resultMatrix, matrix, size, MATRIX::IDENTITY);
    // Освобождение памяти, выделенной для матрицы результата
    MATRIX::memoryFree(resultMatrix, size);

    return true; // Матрица невырожденная
}
