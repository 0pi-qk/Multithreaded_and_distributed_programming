#include "../include/matrix_operation.h"
#include "../include/inverse_single.h"
#include "../include/inverse_CUDA.cuh"

using namespace std;

// ~~~ МЕНЮ УПРАВЛЕНИЯ ~~~

int main() {
    double **matrix = nullptr;
    double **matrixSingle = nullptr;
    double **matrixCUDA = nullptr;

    int size;
    int threadsInBlock;
    char matrixGenerate, matrixPrint;

    chrono::time_point<chrono::steady_clock> start;

    cout << "=== PARAMETERS ===" << endl;

    while (true) {
        cout << "Generate a random matrix (g) or load from a file (f)?: ";
        cin >> matrixGenerate;

        if (matrixGenerate == 'g') {
            cout << "Enter the dimension of the matrix: ";
            cin >> size;

            matrix = MATRIX::memoryAlloc(size);
            MATRIX::generateMatrix(matrix, size, MATRIX::RANDOM);

            break;
        } else if (matrixGenerate == 'f') {
            string filename;

            cout << "Enter the file name (with extension -> .txt): ";
            cin >> filename;

            MATRIX::loadMatrixFromFile(matrix, size, filename.c_str());

            break;
        } else
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
    } else if (matrixPrint == 's')
        MATRIX::saveMatrixToFile(matrix, size, "matrix.txt");

    matrixSingle = MATRIX::memoryAlloc(size);
    MATRIX::copyMatrix(matrix, matrixSingle, size);
    matrixCUDA = MATRIX::memoryAlloc(size);
    MATRIX::copyMatrix(matrix, matrixCUDA, size);

    cout << endl << "=== SEQUENTIAL CALCULATION ===" << endl;

    start = chrono::high_resolution_clock::now();
    if (SINGLE::inverseMatrixSequential(matrixSingle, size)) {
        cout << "Execution time: " << chrono::duration<double>(chrono::high_resolution_clock::now()- start).count() << endl;

        if (matrixPrint == 'p') {
            cout << "Inverse matrix:" << endl;
            MATRIX::printMatrix(matrixSingle, size);
        } else if (matrixPrint == 's')
            MATRIX::saveMatrixToFile(matrixSingle, size, "inverse_matrix_single.txt");
    } else {
        cout << "The matrix is degenerate, the inverse does not exist." << endl;

        return 1;
    }

    cout << endl << "=== PARALLEL COMPUTING (CUDA) ===" << endl;

    cout << "Enter the number of threads in the block: ";
    cin >> threadsInBlock;

    start = chrono::high_resolution_clock::now();

    if (CUDA::inverseMatrixParallelCUDA(matrixCUDA, size, threadsInBlock)) {
        cout << "Execution time: " << chrono::duration<double>(chrono::high_resolution_clock::now()- start).count() << endl;
        cout << "Comparison of sequential and parallel algorithms: "
             << MATRIX::compareMatrices(matrixSingle, matrixCUDA, size) << endl;

        if (matrixPrint == 'p') {
            cout << "Inverse matrix:" << endl;
            MATRIX::printMatrix(matrixCUDA, size);
        } else if (matrixPrint == 's')
            MATRIX::saveMatrixToFile(matrixCUDA, size, "inverse_matrix_MPI.txt");
    } else
        cout << "The matrix is degenerate, the inverse does not exist." << endl;

    MATRIX::memoryFree(matrixSingle, size);
    MATRIX::memoryFree(matrix, size);
    MATRIX::memoryFree(matrixCUDA, size);

    return 0;
}