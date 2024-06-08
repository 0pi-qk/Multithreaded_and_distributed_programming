#include "../include/inverse_single.h"
#include "../include/inverse_OpenMP.h"
#include "../include/matrix_operation.h"

using namespace std;

// ~~~ МЕНЮ УПРАВЛЕНИЯ ~~~

int main() {
    double* matrix = nullptr;  // Начальная матрицы
    double* matrixSingle = nullptr; // Результат последовательного вычисления
    double* matrixOpenmp = nullptr;  // Результат OpenMP

    int size;
    double start;
    char matrixGenerate, matrixPrint;

    cout << "=== PARAMETERS ===" << endl;

    while (true) {
        cout << "Generate a random matrix (g) or load from a file (f)?: ";
        cin >> matrixGenerate;

        if (matrixGenerate == 'g') {
            cout << "Enter the dimensionality of the matrix: ";
            cin >> size;

            matrix = new double[size * size];
            MATRIX::generateMatrix(matrix, size);

            break;
        }
        else if (matrixGenerate == 'f') {
            string filename;

            cout << "Enter the file name (with extension -> .txt): ";
            cin >> filename;

            MATRIX::loadMatrixFromFile(matrix, size, filename.c_str());
            break;
        }
        else
            cerr << "Wrong selection. Please select again." << endl;
    }

    while (true) {
        cout << "Output (p) / do not output (n) matrices to the screen or save to a file (s)?: ";
        cin >> matrixPrint;

        if (matrixPrint == 'p' || matrixPrint == 'n' || matrixPrint == 's')
            break;
        else
            cerr << "Wrong selection. Please select again." << endl;
    }

    if (matrixPrint == 'p') {
        cout << "Initial matrix:" << endl;
        MATRIX::printMatrix(matrix, size);
    }
    else if (matrixPrint == 's')
        MATRIX::saveMatrixToFile(matrix, size, "matrix.txt");

    matrixSingle = new double[size * size];
    MATRIX::copyMatrix(matrix, matrixSingle, size);
    matrixOpenmp = new double[size * size];
    MATRIX::copyMatrix(matrix, matrixOpenmp, size);

    cout << endl << "=== SEQUENTIAL CALCULATION ===" << endl;

    start = omp_get_wtime();
    if (SINGLE::inverseMatrixSequential(matrixSingle, size)) {
        cout << "Execution time: " << omp_get_wtime() - start << endl;

        if (matrixPrint == 'p') {
            cout << "Inverse matrix:" << endl;
            MATRIX::printMatrix(matrixSingle, size);
        }
        else if (matrixPrint == 's')
            MATRIX::saveMatrixToFile(matrixSingle, size, "inverse_matrix_single.txt");
    }
    else
        cout << "The matrix is degenerate, the inverse does not exist." << endl;

    cout << endl << "=== PARALLEL COMPUTING (OpenMP) ===" << endl;

    int threads;

    cout << "Enter the number of threads: ";
    cin >> threads;

    start = omp_get_wtime();
    if (OpenMP::inverseMatrixParallelOpenmp(matrixOpenmp, size, threads)) {
        cout << "Execution time: " << omp_get_wtime() - start << endl;
        cout << "Comparison of sequential and parallel algorithms: " << MATRIX::compareMatrices(matrixSingle, matrixOpenmp, size) << endl;

        if (matrixPrint == 'p') {
            cout << "Inverse matrix:" << endl;
            MATRIX::printMatrix(matrixOpenmp, size);
        }
        else if (matrixPrint == 's')
            MATRIX::saveMatrixToFile(matrixOpenmp, size, "inverse_matrix_openmp.txt");
    }
    else
        cout << "The matrix is degenerate, the inverse does not exist." << endl;

    delete[] matrix;
    delete[] matrixSingle;
    delete[] matrixOpenmp;

    return 0;
}