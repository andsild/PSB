#ifndef _SOLVER
#define _SOLVER 1

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

#include "../main.cpp"
#include "../file.cpp"
#include "../image2.cpp"

#include "CImg.h"
using namespace cimg_library;
using namespace std;

namespace pe_solver //[p]oison-[e]quation
{


void printer(CImg<double> image)
{
    for(int iPos = 0; iPos < image.height(); iPos++)
    {
        for(int jPos = 0; jPos < image.width(); jPos++)
        {
            printf("%5.1f ",image(jPos,iPos));
        }
        cout << endl;
    }
}


typedef void (*iterative_function)(const CImg<double> &arg1, CImg<double> &arg2,
                                   double, int, double &arg3, double) ;

double calculateError(double, double);

double getRangeVal(const CImg<double> &U, const CImg<double> &F,
        const int iIndex, const int iWidthLength, const double H = 1.0)
{
    // int xPos = iIndex % (int)iWidthLength;
    // /* Skip the first and last column */
    // if(xPos < 1 || xPos > iWidthLength - 2) { return -999;}// return F[iIndex]; }
    // /* XXX: Row-wise skips are made in external for-loops */
    //
    // int iIndexPixelAbove = iIndex + iWidthLength;
    // int iIndexPixelBelow = iIndex - iWidthLength;
    //
    // return (U[iIndex+1] + U[iIndex-1]
    //         + U[iIndexPixelAbove]
    //         + U[iIndexPixelBelow]
    //         + F[iIndex] * H * H );
    return 5;
}

/** jacobi iteration
 *
 */
void iterate_jacobi(const CImg<double> &F, CImg<double> &U, double iWidthLength,
                    int iLength, double &dDiff, double H = 1)
{
    dDiff = 0;
    int BORDER_SIZE = 1;
    CImg<double> copyU = U;
    CImg_3x3(I,double);

    cimg_for_in3x3(copyU, BORDER_SIZE, BORDER_SIZE,
                   U.width() - BORDER_SIZE - 1, U.height() - BORDER_SIZE - 1,
                   x,y,0,0,I,double) // uses Neumann borders
    // cimg_for3x3(copyU,x,y,0,0,I,double) // uses Neumann borders
    {
        double dOldVal = U(x,y);
        double dNewVal = .25 * (Icn + Icp + Ipc + Inc + F(x,y) * H * H);
        U(x,y) = dNewVal;
        double dCurDiff = (abs(dOldVal - dNewVal));
        if( dCurDiff > dDiff)
            dDiff = dCurDiff;
    }
}



/** Gauss-seidel iteration
 *
 */
void iterate_gauss(const CImg<double> &F, CImg<double> &U, double iWidthLength,
                           int iLength, double &dDiff, double H = 1)
{
    for(vector<int>::size_type iPos = iWidthLength;
            iPos < (int)F.size() - iWidthLength;
            iPos++) 
    {
        U[iPos] = .25 * (getRangeVal(U, F, iPos, iWidthLength, H));
    }
}

void iterate_sor(const CImg<double> &F, CImg<double> &U,
                 double iWidthLength, int iLength, double &dDiff, double H = 1)
{
    double omega = 2 / (1 + (3.14 / iWidthLength));
    double dOmegaConstant = omega / 4;
    double dNotOmega = (1 - omega);

    for(vector<int>::size_type iPos = iWidthLength;
            iPos < (int)F.size() - iWidthLength;
            iPos++) 
    {
        {
            U[iPos] = (dNotOmega * U[iPos])
                     + dOmegaConstant
                     * getRangeVal(U, F, iPos, iWidthLength);
        }
    }
}



void two_grid(double h, CImg<double> &U, CImg<double> &F, int iWidthLength, int iSmoothFactor)
{
    double H = 2 * h;
    CImg<double> r(iWidthLength*2);
    CImg<double> R(iWidthLength);
    CImg<double> V(iWidthLength);
    int L2 = iWidthLength / 2;

    // solve exactly if there is only one interior point
    if (iWidthLength == 1) 
    {
        U[0] = 0.25;// * (U[0][1] + U[2][1] + U[1][0] + U[1][2] + h * h * F[1][1]);
        return;
    }

    // pre-smoothing
    for (int iPos = 0; iPos < iSmoothFactor; iPos++)
    {
        iterate_gauss(F, U, iWidthLength, iWidthLength * 2, h);
    }

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

    two_grid(H, V, R, L2, iSmoothFactor);
    CImg<double> v(iWidthLength*2);

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

    // post-smoothing Gauss-Seidel
    for (int iPos = 0; iPos < iSmoothFactor; iPos++)
    {
        iterate_gauss(F, U, iWidthLength, iWidthLength * 2, h);
    }
}

double calculateError(const double dOriginal, const double dNew)
{
    if(dNew== 0) { return dOriginal * 100; }
    // return abs( (dNew - dOriginal) );
    return (double)abs( (dNew - dOriginal) / dNew) * 100;
}

// double meanDifference(const CImg<double> origData, const CImg<double> newData, int iWidthLength)
// {
//     double dRelativeError = 0;
//     // for(std::vector<int>::size_type iPos = 0; iPos != origData.size(); iPos++)
//     for(std::vector<int>::size_type iPos = iWidthLength; iPos != origData.size() - iWidthLength; iPos++)
//     {
//         int xPos = iPos % (iWidthLength);
//         if(xPos < 1 || xPos > iWidthLength - 2) { continue; }
//         double dRes = calculateError(origData[iPos],
//                                  newData[iPos]);
//         // if(dRes == CHANGE_FROM_ZERO) { iLength--; continue; }
//         dRelativeError += dRes;
//         // dRelativeError += calculateError(origData[iPos],
//         //                          newData[iPos]);
//     }
//     return dRelativeError /= (double)origData.size();
//
// }

vector<string> iterative_solve(iterative_function function,
                    const CImg<double> solution, CImg<double> &guess, CImg<double> rho,
                    double dMaxErr, int iWidth) 
{
    CImg<double> old_guess = guess, newGuess = guess;
    double dRelativeError = 0;
    vector<string> vOutput;
    int iLength = solution.width() * solution.height();


    // cout << "Initial image" << endl;
    // // printAsImage(solution, iWidth) ;
    // cout << "Initial guess" << endl;
    // printAsImage(guess, iWidth) ;
    // cout << "Initial rho" << endl;
    // printAsImage(rho, iWidth);
    // cout << "Entering loop..." << endl;
    //
    int iIter = 0;
    do
    {
        // newGuess = old_guess;
        function(rho, newGuess, iWidth, iLength, dRelativeError, 1);
        cout << "New guess:" << endl;
        printer(newGuess);

        iIter++;
        if(iIter % 1 == 0) { cout << "=== [ solving: " << dRelativeError 
                                     << " ] ===" << endl;}
        double dMSE = newGuess.MSE(solution);
        old_guess = newGuess;

        cout << "Iteration diff(max): " << dRelativeError << endl;
        cout << "Image diff(mean): " << dMSE << endl;
        vOutput.push_back(std::to_string(dMSE));
    } while(dRelativeError > dMaxErr);

    cout << vOutput.size() << " iterations" << endl;
    guess = newGuess;

    return vOutput;
}

} // EndOfNamespace

#endif

/* EOF */
