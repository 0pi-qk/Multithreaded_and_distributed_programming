#include "../include/matrix_operation.h"

using namespace std;

// ~~~ ФУНКЦИИ ДЛЯ РАБОТЫ С МАТРИЦЕЙ ~~~

// Функция для выделения памяти под матрицу
double **MATRIX::memoryAlloc(int size, MATRIX::MatrixType type) {
    auto **matrix = new double*[size];

    for (int i = 0; i < size; i++)
        if (type == IDENTITY)
            matrix[i] = new double[size];
        else
            matrix[i] = new double[size * 2];

    return matrix;
}

// Функция для очистки памяти
void MATRIX::memoryFree(double** matrix, int size) {
    for (int i = 0; i < size; i++)
        delete[] matrix[i];

    delete[] matrix;
}

// Функция генерации случайных значений в заданном диапазоне
double MATRIX::randomInRange(double min, double max) {
    return min + static_cast<double>(rand()) / RAND_MAX * (max - min);
}

// Функция для заполнения матрицы в соответствии с заданным типом
void MATRIX::generateMatrix(double** matrix, int size, MATRIX::MatrixType type) {
    if (type == IDENTITY)
        for (int i = 0; i < size; i++)
            for (int j = 0; j < size; j++)
                matrix[i][j] = (i == j) ? 1 : 0;
    else {
        srand(static_cast<unsigned int>(time(nullptr)));

        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size * 2; j++) {
                matrix[i][j] = (j < size) ? MATRIX::randomInRange(50, 100) : ((j - size == i) ? 1 : 0);
            }
        }
    }
}

// Функция для вывода матрицы на экран
void MATRIX::printMatrix(double** matrix, int size) {
    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j)
            cout << setw(12) << matrix[i][j] << " "; // Вывод элементов матрицы с отступами

        cout << endl;
    }
}

// Функция копирования матрицы
void MATRIX::copyMatrix(double** source, double** destination, int size, MATRIX::MatrixType type) {
    for (int i = 0; i < size; ++i)
        if (type == IDENTITY)
            for (int j = 0; j < size; ++j)
                destination[i][j] = source[i][j];
        else
            for (int j = 0; j < size * 2; ++j)
                destination[i][j] = source[i][j];
}

// Функция сравнения двух матриц на равенство
bool MATRIX::compareMatrices(double** matrix1, double** matrix2, int size) {
    bool key = true;

    for (int i = 0; i < size; ++i)
        for (int j = 0; j < size; ++j)
            if (fabs(matrix1[i][j] - matrix2[i][j]) > 1e-9 && key) // Сравнение с учетом погрешности
                key = false;

    if (key)
        return true;
    return false;
}

// Функция сохранения матрицы в файл
void MATRIX::saveMatrixToFile(double** matrix, int size, const char* filename) {
    ofstream file(filename);

    if (file.is_open()) {
        file << size << " " << size * 2 << endl; // Запись размера матрицы в файл

        for (int i = 0; i < size; ++i) {
            for (int j = 0; j < size * 2; ++j)
                file << matrix[i][j] << " "; // Запись элементов матрицы в файл

            file << endl;
        }

        file.close();

        cout << "The matrix is saved to a file '" << filename << "'" << endl;
    }
    else
        cerr << "Failed to open the file for writing." << endl;
}

// Функция загрузки матрицы из файла
void MATRIX::loadMatrixFromFile(double**& matrix, int& size, const char* filename) {
    ifstream file(filename);

    if (file.is_open()) {
        file >> size >> size; // Чтение размера матрицы из файла

        size /= 2;

        matrix = MATRIX::memoryAlloc(size, MATRIX::RANDOM_EXTENDED); // Выделение памяти под матрицу

        for (int i = 0; i < size; ++i)
            for (int j = 0; j < size * 2; ++j)
                file >> matrix[i][j]; // Чтение элементов матрицы из файла

        file.close();

        cout << "Matrix loaded from a file '" << filename << "'" << endl;
    }
    else
        cerr << "Failed to open the file for reading." << endl;
}

