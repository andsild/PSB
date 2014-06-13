#ifndef _SOLVER_GRID
#define _SOLVER_GRID 1

// TODO: make sure that boundaries are extended, currently at end-of-row
// you extend to the next pixel in raster order.
//TODO: check for diagonal dominance
//If you know that a matrix is diagonally dominant,
//
#include <iostream>
#include <cmath>
#include <algorithm>
#include <vector>
#include <stdio.h>
#include <cmath>
#include <string>

#include "../../main.cpp"
#include "gauss_image.cpp"
#include "../../../lib/CImg-1.5.8/CImg.h"

using namespace cimg_library;
using namespace std;

namespace pe_solver
{

void two_grid(double h, d1 &U, d1 &F, int iWidthLength, int iLength,
              int iSmoothFactor)
{
    // solve exactly if there is only one interior point
    // do a few pre-smoothing Gauss-Seidel steps
    for (int iPos = 0; iPos < iSmoothFactor; iPos++)
        iterate_gauss(F, U, iWidthLength, iLength);

    // find the residual
    int iCoarseLength = iWidthLength / 2;
    d1 r(iCoarseLength);
    for (int iPos = iWidthLength; iPos <= iCoarseLength - iWidthLength; iPos++)
        r[iPos] = getRangeVal(U, F, iPos, iWidthLength)
                  - ((4 * U[iPos]) / (h * h));

    // restrict residual to coarser grid
    // int L2 = iCoarseLength / 2;
    // d1 R(L2 + 2, L2 + 2);
    // for (int iPos = 1; iPos <= L2; iPos++) {
    //     int iPos = 2 * iPos - 1;
    //     for (int jPos = 1; jPos <= L2; jPos++) {
    //         int jPos = 2 * jPos - 1;
    //         R[iPos][jPos] = 0.25 * ( r[iPos][jPos] + r[iPos + 1][jPos] + r[iPos][jPos + 1] +
    //                 r[iPos + 1][jPos + 1]);
    //     }
    // }

    // initialize correction V on coarse grid to zero
    // d1 V(L2 + 2, L2 + 2);

    // call twoGrid recursively
    // double H = 2 * h;
    // two_grid(H, V, R, iWidthLength, iLength, iSmoothFactor);
    
    // prolongate V to fine grid using simple injection
    // d1 v(iCoarseLength + 2, iCoarseLength + 2);
    // for (int iPos = 1; iPos <= L2; iPos++) {
    //     int iPos = 2 * iPos - 1;
    //     for (int jPos = 1; jPos <= L2; jPos++) {
    //         int jPos = 2 * jPos - 1;
    //         v[iPos][jPos] = v[iPos + 1][jPos] = v[iPos][jPos + 1] = v[iPos + 1][jPos + 1] = V[iPos][jPos];
    //     }
    // }

    // correct U
    //TODO: fix range
    // for (int iPos = 1; iPos <= iCoarseLength; iPos++)
    //     U[iPos] += v[iPos][jPos];
    //
    // // do a few post-smoothing Gauss-Seidel steps
    // for (int iPos = 0; iPos < iSmoothFactor; iPos++)
    //     iterate_gauss(F, U, iWidthLength, iCoarseLength);
}

}

#endif
