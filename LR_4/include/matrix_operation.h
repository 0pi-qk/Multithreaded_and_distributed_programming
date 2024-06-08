#ifndef LR_4_MATRIX_OPERATION_H
#define LR_4_MATRIX_OPERATION_H

#include <iostream>
#include <fstream>
#include <iomanip>
#include <chrono>
#include <ctime>
#include <cmath>

namespace MATRIX {
    enum MatrixType {
        IDENTITY,
        RANDOM
    };

    double **memoryAlloc(int);
    void memoryFree(double**, int);

    double randomInRange(double, double);
    void generateMatrix(double**, int, MatrixType);
    void printMatrix(double**, int);
    void copyMatrix(double**, double**, int);
    bool compareMatrices(double**, double**, int);
    void converterMatrix(double**, double*, int, bool);

    void saveMatrixToFile(double**, int, const char*);
    void loadMatrixFromFile(double**&, int&, const char*);
}

#endif
