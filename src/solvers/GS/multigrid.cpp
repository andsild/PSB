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
void two_grid(double h, d1 &U, d1 &F, int iWidthLength, int iSmoothFactor)
{

    // solve exactly if there is only one interior point
    if (iWidthLength == 1) 
    {
        U[0] = 0.25;// * (U[0][1] + U[2][1] + U[1][0] + U[1][2] + h * h * F[1][1]);
        return;
    }

    // do a few pre-smoothing Gauss-Seidel steps
    for (int iPos = 0; iPos < iSmoothFactor; iPos++)
    {
        iterate_gauss(F, U, iWidthLength, iWidthLength * 2, h);
    }

    // find the residual
    d1 r(iWidthLength*2);
    for (int yPos = iWidthLength;
         yPos <= iWidthLength;
         yPos += iWidthLength)
    {
        for (int xPos = 0; xPos <= iWidthLength; xPos++)
        {
            int iCurrent = xPos + yPos;
            r[iCurrent] = F[iCurrent] - (4 * U[iCurrent] / (h * h))
                            + ( U[iCurrent + 1] + U[iCurrent - 1]
                              + U[iCurrent - iWidthLength] 
                              + U[iCurrent + iWidthLength]);
        }
    }

    // restrict residual to coarser grid
    int L2 = iWidthLength / 2;
    d1 R(iWidthLength);
    for (int yPos = iWidthLength; yPos <= L2; yPos += iWidthLength)
    {
        int iPos = 2 * yPos - 1;
        for (int xPos = 0; xPos <= L2; xPos++)
        {
            int jPos = 2 * jPos - 1;
            int iCurrent = (iPos+jPos);
            R[(yPos+xPos)] = 0.25 * 
                            ( r[iCurrent]
                              + r[iCurrent+iWidthLength]
                              + r[iCurrent + 1]
                              + r[iCurrent+1+iWidthLength]
                            );
        }
    }

    // initialize correction V on coarse grid to zero
    d1 V(iWidthLength);

    // call twoGrid recursively
    double H = 2 * h;
    two_grid(H, V, R, L2, iSmoothFactor);

    // prolongate V to fine grid using simple injection
    d1 v(iWidthLength*2);
    for (int yPos = iWidthLength; yPos <= L2; yPos += iWidthLength)
    {
        int iPos = 2 * yPos - 1;
        for (int xPos = 0; xPos <= L2; xPos++)
        {
            int jPos = 2 * jPos - 1;
            int iCurrent = (iPos+jPos);
            v[(xPos + yPos)] = v[iCurrent+iWidthLength] 
                             = v[iCurrent+1]
                             = v[iCurrent+1+iWidthLength]
                             = V[(xPos+yPos)];
        }
    }

    // correct U
    for(vector<int>::size_type iPos = iWidthLength;
            iPos < iWidthLength * 2;
            iPos++) 
    {
            U[iPos] += v[iPos];
    }

    // do a few post-smoothing Gauss-Seidel steps
    for (int iPos = 0; iPos < iSmoothFactor; iPos++)
    {
        iterate_gauss(F, U, iWidthLength, iWidthLength * 2, h);
    }

}

} // EndOfNamespace
#endif
