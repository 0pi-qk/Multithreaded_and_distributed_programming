#include "../include/matrix_operation.h"

using namespace std;

// ~~~ ФУНКЦИИ ДЛЯ РАБОТЫ С МАТРИЦЕЙ ~~~

// Функция генерации случайных значений в заданном диапазоне
double MATRIX::randomInRange(double min, double max) {
    return min + static_cast<double>(rand()) / RAND_MAX * (max - min);
}

// Функция для заполнения матрицы в соответствии с заданным типом
void MATRIX::generateMatrix(double* matrix, int size) {
    srand(static_cast<unsigned int>(time(nullptr)));

    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            matrix[i * size + j] = MATRIX::randomInRange(50, 100);
        }
    }
}

// Функция для вывода матрицы на экран
void MATRIX::printMatrix(const double* matrix, int size) {
    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j)
            cout << setw(12) << matrix[i * size + j] << " "; // Вывод элементов матрицы с отступами

        cout << endl;
    }
}

// Функция копирования матрицы
void MATRIX::copyMatrix(const double* source, double* destination, int size) {
#pragma omp parallel for shared(source, destination, size) default(none)
    for (int i = 0; i < size * size; ++i)
        destination[i] = source[i];
}

// Функция сравнения двух матриц на равенство
bool MATRIX::compareMatrices(const double* matrix1, const double* matrix2, int size) {
    bool key = true;

    for (int i = 0; i < size * size; ++i)
        if (fabs(matrix1[i] - matrix2[i]) > 1e-9 && key) // Сравнение с учетом погрешности
            key = false;

    if (key)
        return true;
    return false;
}

// Функция сохранения матрицы в файл
void MATRIX::saveMatrixToFile(const double* matrix, int size, const char* filename) {
    ofstream file(filename);

    if (file.is_open()) {
        file << size << " " << size << endl; // Запись размера матрицы в файл

        for (int i = 0; i < size; ++i) {
            for (int j = 0; j < size; ++j)
                file << matrix[i * size + j] << " "; // Запись элементов матрицы в файл

            file << endl;
        }

        file.close();

        cout << "The matrix is saved to a file '" << filename << "'" << endl;
    }
    else
        cerr << "Failed to open the file for writing." << endl;
}

// Функция загрузки матрицы из файла
void MATRIX::loadMatrixFromFile(double*& matrix, int& size, const char* filename) {
    ifstream file(filename);

    if (file.is_open()) {
        file >> size >> size; // Чтение размера матрицы из файла

        matrix = new double[size * size]; // Выделение памяти под матрицу

        for (int i = 0; i < size; ++i)
            for (int j = 0; j < size; ++j)
                file >> matrix[i * size + j]; // Чтение элементов матрицы из файла

        file.close();

        cout << "Matrix loaded from a file '" << filename << "'" << endl;
    }
    else
        cerr << "Failed to open the file for reading." << endl;
}
