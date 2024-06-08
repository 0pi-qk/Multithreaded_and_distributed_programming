#ifndef LR_3_INVERSE_MPI_H
#define LR_3_INVERSE_MPI_H

#include <mpi.h>

namespace MPI {
    bool inverseMatrixParallelMPI(double**, int, int, int);
}

#endif
