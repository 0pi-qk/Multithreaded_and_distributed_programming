#ifndef LR_3_MATRIX_OPERATION_H
#define LR_3_MATRIX_OPERATION_H

#include <iostream>
#include <fstream>
#include <iomanip>
#include <ctime>
#include <cmath>

namespace MATRIX {
    double randomInRange(double, double);
    void generateMatrix(double*, int);
    void printMatrix(const double*, int);
    void copyMatrix(const double*, double*, int);
    bool compareMatrices(const double*, const double*, int size);

    void saveMatrixToFile(const double*, int, const char*);
    void loadMatrixFromFile(double*&, int&, const char*);
}

#endif
