#ifndef _SOLVER
#define _SOLVER 1

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
#include "../../../lib/CImg-1.5.8/CImg.h"

using namespace cimg_library;
using namespace std;

namespace pe_solver
{
typedef vector<double> d1;
typedef CImg<double> matrix_type;

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
                           int iLength)
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

void iterate_sor(d1 F, d1 &U, double iWidthLength,
                           int iLength)
{
    double omega = 2 / (1 + (3.14 / iWidthLength));
    double dOmegaConstant = omega / 4;
    double dNotOmega = (1 - omega);
    // update even sites first
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

matrix_type test(d1 solution, d1 guess,
                double dMaxErr, int iLength, int iWidth) 
{
    double dMax = 0;
    d1 old_guess = guess;
    double dError = 0;
    int iIterCount = 0;
    vector<string> vOutput(1, "");
    do
    {
        dError = 0;
        d1 newGuess = old_guess;
        iterate_gauss(solution, newGuess, iLength, iWidth);
        // CImg<double> newGuess = iterate_jacobi(solution, old_guess, iWidth);
        // CImg<double> newGuess = iterate_sor(solution, old_guess, iWidth);

        dError = findError(old_guess, newGuess, iLength);
        old_guess = newGuess;
        iIterCount++;
        
        vOutput.push_back(to_string(iIterCount) + "\t" 
                          + to_string(dError) + "\n");

    // } while(dMax > dMaxErr);
    } while(dError > dMaxErr);

    writeToFile(vOutput, dError);

    return CImg<double>(1, 1, 1, 1, 1);
    // return solution;
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


} // EndOfNamespace

#endif

/* EOF */
