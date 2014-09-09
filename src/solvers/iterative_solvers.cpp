/** File that has the different iterative solvers
*/

/* A CImg 2x2 neigbourhood with B as adjacent of pixel A is:
   x x x
   x A B
   x B B

   A 3x3 neigbourhood fetches all the adjacent within a range of 1
*/
//Speedup between doing 2x2 kernel with two ``manual'' extra fetches,
//or using a 3x3 kernel is trivial.
#include "iterative_solvers.hpp"

#include <iostream>
#include <string>
#include <vector>

#include "CImg.h"

#include "image_types.hpp"
#include "solver.hpp"
// #include "loginstance.hpp"

using namespace cimg_library;

namespace solver 
{

data_fmt dOldVal, dNewVal, dCurDiff;

void iterate_jacobi(const image_fmt &field, image_fmt &U,
                 double &dDiff, const int iWidth, const int iHeight)
{
    dDiff = 0;
    image_fmt origGuess(U);
    CImg_3x3(I,data_fmt );
    cimg_for_in3x3(origGuess, BORDER_SIZE, BORDER_SIZE,
                   iWidth - BORDER_SIZE - 1, iHeight - BORDER_SIZE - 1,
                   x,y,0,0,I,data_fmt) 
    {
        dNewVal = .25 * (Icn + Icp + Ipc + Inc - field(x,y));
        dCurDiff = fabs(Icc - dNewVal);
        U(x,y) = dNewVal;
        if( dCurDiff > dDiff)
            dDiff = dCurDiff;
    }
}


void iterate_gauss(const image_fmt &field, image_fmt &guess,
                 double &dDiff, const int iWidth, const int iHeight)
{
    dDiff = 0;
    CImg_3x3(I,data_fmt );

    cimg_for_in3x3(guess, BORDER_SIZE, BORDER_SIZE,
                   iWidth - BORDER_SIZE - 1, iHeight - BORDER_SIZE - 1,
                   x,y,0,0,I,data_fmt) // uses Neumann borders
    {
        if( (x + y) % 2 == 0)
            continue;
         dOldVal = guess(x,y);
         dNewVal = .25 * ( Icn + Icp + Ipc + Inc - field(x,y));
        guess(x,y) = dNewVal;
         dCurDiff = (fabs(dOldVal - dNewVal));
        if( dCurDiff > dDiff)
            dDiff = dCurDiff;
    }

    cimg_for_in3x3(guess, BORDER_SIZE, BORDER_SIZE,
                   iWidth - BORDER_SIZE - 1, iHeight - BORDER_SIZE - 1,
                   x,y,0,0,I,data_fmt) // uses Neumann borders
    {
        if( (x + y) % 2 != 0)
            continue;
         dOldVal = guess(x,y);
         dNewVal = .25 * ( Icn + Icp + Ipc + Inc - field(x,y));
        guess(x,y) = dNewVal;
         dCurDiff = (fabs(dOldVal - dNewVal));
        if( dCurDiff > dDiff)
            dDiff = dCurDiff;
    }
}

void iterate_sor(const image_fmt &field, image_fmt &U,
                 double &dDiff, const int iWidth, const int iHeight)
{
    dDiff = 0;
    CImg_3x3(I,data_fmt);

    data_fmt omega,dOmegaConstant, dNotOmega;

     omega = 2 / (1 + (3.14 / iWidth ));
     dOmegaConstant = omega / 4;
     dNotOmega = (1 - omega);


    cimg_for_in3x3(U, BORDER_SIZE, BORDER_SIZE,
                   U.width() - BORDER_SIZE - 1, U.height() - BORDER_SIZE - 1,
                   x,y,0,0,I,data_fmt) // uses Neumann borders
    {
        if( (x + y) % 2 == 0)
            continue;
         dOldVal = U(x,y);
         dNewVal = (dNotOmega * Icc)
                          + (dOmegaConstant)
                          * (Icn + Icp + Ipc + Inc - field(x,y));
        U(x,y) = dNewVal;
         dCurDiff = (fabs(dOldVal - dNewVal));
        if( dCurDiff > dDiff)
            dDiff = dCurDiff;
    }

    cimg_for_in3x3(U, BORDER_SIZE, BORDER_SIZE,
                   iWidth - BORDER_SIZE - 1, iHeight - BORDER_SIZE - 1,
                   x,y,0,0,I,double) // uses Neumann borders
    {
        if( (x + y) % 2 != 0)
            continue;
        double dOldVal = U(x,y);
        double dNewVal = (dNotOmega * Icc)
                          + (dOmegaConstant)
                          * (Icn + Icp + Ipc + Inc - field(x,y));
        U(x,y) = dNewVal;
        double dCurDiff = (fabs(dOldVal - dNewVal));
        if( dCurDiff > dDiff)
            dDiff = dCurDiff;
    }
}

} // EndOfNamespace

/* EOF */
