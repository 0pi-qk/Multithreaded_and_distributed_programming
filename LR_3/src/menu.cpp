#include "../include/matrix_operation.h"
#include "../include/inverse_single.h"
#include "../include/inverse_MPI.h"

using namespace std;

// ~~~ МЕНЮ УПРАВЛЕНИЯ ~~~

int main(int argc, char **argv) {
    int MPI_rank, MPI_size;

    double **matrix = nullptr;
    double **matrixSingle = nullptr;
    double **matrixMPI = nullptr;

    int size;
    double start;
    char matrixGenerate, matrixPrint;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &MPI_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &MPI_size);

    if (MPI_rank == 0) {
        cout << "=== PARAMETERS ===" << endl;

        while (true) {
            cout << "Generate a random matrix (g) or load from a file (f)?: ";
            cin >> matrixGenerate;

            if (matrixGenerate == 'g') {
                while (true) {
                    cout << "Enter the dimension of the matrix (multiple of " << MPI_size << "): ";
                    cin >> size;

                    if (size % MPI_size != 0)
                        cout << "EXCEPTION: The dimension of the matrix must be a multiple of the number of processes (MPI_size = " << MPI_size << ")" << endl;
                    else
                        break;
                }

                matrix = MATRIX::memoryAlloc(size, MATRIX::RANDOM_EXTENDED);
                MATRIX::generateMatrix(matrix, size, MATRIX::RANDOM_EXTENDED);

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
    }
    MPI_Barrier(MPI_COMM_WORLD);

    MPI_Bcast(&size, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&matrixPrint, 1, MPI_CHAR, 0, MPI_COMM_WORLD);
    MPI_Bcast(&matrixGenerate, 1, MPI_CHAR, 0, MPI_COMM_WORLD);

    if (MPI_rank == 0) {
        // Отправка каждой строки матрицы по очереди
        for (int i = 0; i < size; ++i) {
            MPI_Bcast(matrix[i], size * 2, MPI_DOUBLE, 0, MPI_COMM_WORLD);
        }
    } else {
        // Выделение памяти для матрицы на других процессах
        matrix = MATRIX::memoryAlloc(size, MATRIX::RANDOM_EXTENDED);
        for (int i = 0; i < size; ++i) {
            MPI_Bcast(matrix[i], size * 2, MPI_DOUBLE, 0, MPI_COMM_WORLD);
        }
    }

    MPI_Barrier(MPI_COMM_WORLD);

    matrixSingle = MATRIX::memoryAlloc(size, MATRIX::RANDOM_EXTENDED);
    MATRIX::copyMatrix(matrix, matrixSingle, size, MATRIX::RANDOM_EXTENDED);
    matrixMPI = MATRIX::memoryAlloc(size, MATRIX::RANDOM_EXTENDED);
    MATRIX::copyMatrix(matrix, matrixMPI, size, MATRIX::RANDOM_EXTENDED);

    if (MPI_rank == 0) {
        cout << endl << "=== SEQUENTIAL CALCULATION ===" << endl;

        start = MPI_Wtime();
        if (SINGLE::inverseMatrixSequential(matrixSingle, size)) {
            cout << "Execution time: " << MPI_Wtime() - start << endl;

            if (matrixPrint == 'p') {
                cout << "Inverse matrix:" << endl;
                MATRIX::printMatrix(matrixSingle, size);
            } else if (matrixPrint == 's')
                MATRIX::saveMatrixToFile(matrixSingle, size, "inverse_matrix_single.txt");
        } else {
            cout << "The matrix is degenerate, the inverse does not exist." << endl;

            return 1;
        }

        cout << endl << "=== PARALLEL COMPUTING (MPI) ===" << endl;

        start = MPI_Wtime();
    }

    if (MPI::inverseMatrixParallelMPI(matrixMPI, size, MPI_rank, MPI_size)) {
        MPI_Barrier(MPI_COMM_WORLD);

        if (MPI_rank == 0) {
            cout << "Execution time: " << MPI_Wtime() - start << endl;
            cout << "Comparison of sequential and parallel algorithms: "
                 << MATRIX::compareMatrices(matrixSingle, matrixMPI, size) << endl;

            if (matrixPrint == 'p') {
                cout << "Inverse matrix:" << endl;
                MATRIX::printMatrix(matrixMPI, size);
            } else if (matrixPrint == 's')
                MATRIX::saveMatrixToFile(matrixMPI, size, "inverse_matrix_MPI.txt");
        }
    } else if (MPI_rank == 0)
            cout << "The matrix is degenerate, the inverse does not exist." << endl;

    MATRIX::memoryFree(matrixSingle, size);
    MATRIX::memoryFree(matrix, size);

    if (MPI_rank == 0)
        MATRIX::memoryFree(matrixMPI, size);

    MPI_Finalize();

    return 0;
}