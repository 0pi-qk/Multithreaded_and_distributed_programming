#ifndef LR_3_MATRIX_OPERATION_H
#define LR_3_MATRIX_OPERATION_H

#include <iostream>
#include <fstream>
#include <iomanip>
#include <ctime>
#include <cmath>

namespace MATRIX {
    enum MatrixType {
        IDENTITY,
        RANDOM_EXTENDED
    };

    double **memoryAlloc(int, MatrixType);
    void memoryFree(double**, int);

    double randomInRange(double, double);
    void generateMatrix(double**, int, MatrixType);
    void printMatrix(double**, int);
    void copyMatrix(double**, double**, int, MatrixType);
    bool compareMatrices(double**, double**, int);

    void saveMatrixToFile(double**, int, const char*);
    void loadMatrixFromFile(double**&, int&, const char*);
}

#endif
