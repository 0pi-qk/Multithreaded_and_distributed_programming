#ifndef LR_2_4_INVERSE_OPENMP_H
#define LR_2_4_INVERSE_OPENMP_H

#include <omp.h>
#include <algorithm>

namespace OpenMP {
    void rowOperationOpenmp(double*, int, int, int, double, int);
    bool inverseMatrixParallelOpenmp(double*, int, int);
}

#endif
