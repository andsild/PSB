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
#include "../../lib/CImg-1.5.8/CImg.h"

using namespace cimg_library;
using namespace std;

namespace pe_solver //[p]oison-[e]quation
{
typedef vector<double> d1;

double calculateError(double, double);
void writeToFile(vector<string>, double);

double getRangeVal(const d1 &U, const d1 &F,
        const int iIndex, const int iWidthLength, const double H = 1.0)
{
    int iIndexPixelAbove = iIndex - iWidthLength;
    int iIndexPixelBelow = iIndex + iWidthLength;

    return (U[iIndex+1] + U[iIndex-1]
            + U[iIndexPixelAbove]
            + U[iIndexPixelBelow]
            + F[iIndex] * H * H );
}

/** jacobi iteration
 *
 */
void iterate_jacobi(d1 F, d1 &U, double iWidthLength,
                    int iLength, double H = 1)
{
    d1 copyU = U;
    for(vector<int>::size_type iPos = iWidthLength;
            iPos < iLength - iWidthLength;
            iPos++) 
    {
        U[iPos] = .25 * (getRangeVal(copyU, F, iPos, iWidthLength));
    }
}



/** Gauss-seidel iteration
 *
 */
void iterate_gauss(d1 F, d1 &U, double iWidthLength,
                           int iLength, double H = 1)
{
    for(vector<int>::size_type iPos = iWidthLength;
            iPos < iLength - iWidthLength;
            iPos++) 
    {
        U[iPos] = .25 * (getRangeVal(U, F, iPos, iWidthLength, H));
    }
}

void iterate_sor(d1 F, d1 &U, double iWidthLength, int iLength, double H = 1)
{
    double omega = 2 / (1 + (3.14 / iWidthLength));
    double dOmegaConstant = omega / 4;
    double dNotOmega = (1 - omega);

    for(vector<int>::size_type iPos = iWidthLength;
            iPos < iLength - iWidthLength;
            iPos++) 
    {
        int xPos = iPos % (int)iWidthLength;
        int yPos = iPos / (int)iWidthLength;
        if ((xPos+yPos) % 2 == 0)
        {
            U[iPos] = (dNotOmega * U[iPos])
                     + dOmegaConstant
                     * getRangeVal(U, F, iPos, iWidthLength);
        }
    }

    for(vector<int>::size_type iPos = iWidthLength;
            iPos < iLength - iWidthLength;
            iPos++) 
    {
        int xPos = iPos % (int)iWidthLength;
        int yPos = iPos / (int)iWidthLength;
        if ((xPos+yPos) % 2 != 0)
        {
            U[iPos] = (dNotOmega * U[iPos])
                     + dOmegaConstant
                     * getRangeVal(U, F, iPos, iWidthLength);
        }
    }
}

// TODO: should this be max? (or get max() from diffs in vector?)
double findError(const d1 origData, const d1 newData, int iLength)
{
    double dError = 0;
    for(std::vector<int>::size_type iPos = 0; iPos != origData.size(); iPos++)
    {
        dError += calculateError(origData[iPos],
                                 newData[iPos]);
    }
    return dError /= iLength;

}

double calculateError(const double dOriginal, const double dNew)
{
    if(dNew == 0) { return 0; }
    return abs( (dNew - dOriginal) / dNew) * 100;
}

void two_grid(double h, d1 &U, d1 &F, int iWidthLength, int iSmoothFactor)
{
    double H = 2 * h;
    d1 r(iWidthLength*2);
    d1 R(iWidthLength);
    d1 V(iWidthLength);
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

    // post-smoothing Gauss-Seidel
    for (int iPos = 0; iPos < iSmoothFactor; iPos++)
    {
        iterate_gauss(F, U, iWidthLength, iWidthLength * 2, h);
    }
}


void writeToFile(vector<string> vRes, double dID)
{
    ofstream data_file(DATA_DIR + to_string(dID) + DATA_EXTENSION);

    for (vector<string>::iterator it = vRes.begin();
            it != vRes.end();
            ++it)
    {
        data_file << *it << endl;
    }

    data_file.close();
}

void iterative_solve(void (*function)(d1, d1 &arg, double, int, double),
                    d1 solution, d1 guess,
                    double dMaxErr, int iLength, int iWidth) 
{
    double dMax = 0;
    d1 old_guess = guess;
    d1 newGuess;
    double dError = 0;
    int iIterCount = 0;
    vector<string> vOutput;
    do
    {
        dError = 0;
        newGuess = old_guess;
        function(solution, newGuess, iLength, iWidth, iLength);
        // iterate_jacobi(solution, old_guess, iWidth);
        // iterate_sor(solution, old_guess, iWidth);
        

        dError = findError(old_guess, newGuess, iLength);
        old_guess = newGuess;
        iIterCount++;
        // cout << iIterCount << "\t" << dError << endl;
        
        // vOutput.push_back(to_string(iIterCount) + "\t" 
        //                   + to_string(dError) + "\n");
        vOutput.push_back(to_string(dError));
    } while(dError > dMaxErr);

    writeToFile(vOutput, dError);
}

} // EndOfNamespace

#endif

/* EOF */
