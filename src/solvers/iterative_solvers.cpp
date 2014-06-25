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

using namespace std;

namespace pe_solver //[p]oison-[e]quation
{

typedef vector<double> d1;
typedef void (*iterative_function)(d1, d1 &arg, double, int, double) ;

double calculateError(double, double);

double getRangeVal(const d1 &U, const d1 &F,
        const int iIndex, const int iWidthLength, const double H = 1.0)
{
    int xPos = iIndex % (int)iWidthLength;
    /* Skip the first and last column */
    if(xPos < 1 || xPos > iWidthLength - 2) { return U[iIndex]; }
    /* XXX: Row-wise skips are made in external for-loops */

    int iIndexPixelAbove = iIndex + iWidthLength;
    int iIndexPixelBelow = iIndex - iWidthLength;

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
            iPos < F.size() - iWidthLength;
            iPos++) 
    {
        double dNewVal = getRangeVal(copyU, F, iPos, iWidthLength);
        if(dNewVal==U[iPos]) continue;
        U[iPos] = (double)(.25 * (dNewVal));

    }
}



/** Gauss-seidel iteration
 *
 */
void iterate_gauss(const d1 F, d1 &U, double iWidthLength,
                           int iLength, double H = 1)
{
    for(vector<int>::size_type iPos = iWidthLength;
            iPos < (int)F.size() - iWidthLength;
            iPos++) 
    {
        U[iPos] = .25 * (getRangeVal(U, F, iPos, iWidthLength, H));
    }
}

void iterate_sor(const d1 F, d1 &U, double iWidthLength, int iLength, double H = 1)
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

double findError(const d1 origData, const d1 newData, int iLength)
{
    double *dTmp = new double[origData.size()];

    for(std::vector<int>::size_type iPos = 0; iPos != origData.size(); iPos++)
    {
        if(newData[iPos] == 0) continue;
        dTmp[iPos] = calculateError(origData[iPos],
                                 newData[iPos]);
        // cout << dTmp[iPos] << " ";
        // cout << newData[iPos] << " "; 
    }

    return *std::max_element(dTmp, dTmp+origData.size());
}

double calculateError(const double dOriginal, const double dNew)
{
    if(dNew == 0) { return 0; }
    // return abs( (dNew - dOriginal) );
    return (double)abs( (dNew - dOriginal) / dNew) * 100;
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

double meanDifference(const d1 origData, const d1 newData, int iLength)
{
    double dError = 0;
    for(std::vector<int>::size_type iPos = 0; iPos != origData.size(); iPos++)
    {
        double dRes = calculateError(origData[iPos],
                                 newData[iPos]);
        // if(dRes == 0) { iLength -= 1; continue; }
        dError += calculateError(origData[iPos],
                                 newData[iPos]);
    }
    return dError /= (double)iLength;

}

d1 computeField(const d1 &orig, const d1 &guess, int iWidthLength)
{
    d1 rho(orig.size());
    for(vector<int>::size_type iPos = iWidthLength;
            iPos < (int)orig.size() - iWidthLength;
            iPos++) 
    {
        int iIndexPixelAbove = iPos + iWidthLength;
        int iIndexPixelBelow = iPos - iWidthLength;

        rho[iPos] = guess[iIndexPixelAbove] + guess[iIndexPixelBelow]
                    + guess[iPos + 1] + guess[iPos - 1] 
                    + (4 * orig[iPos] );
    }
    
    return rho;
}

void printAsImage(d1 vec, int iW)
{
    for(int iPos = 0; iPos < vec.size(); iPos++)
    {
        if(iPos % iW == 0) cout << endl;
        cout << vec[iPos] << " ";
    }
    cout << endl << flush;
}


vector<string> iterative_solve(iterative_function function,
                    const d1 solution, d1 &guess,
                    double dMaxErr, int iWidth) 
{
    d1 old_guess = guess;
    d1 newGuess;
    double dError = 0;
    int iIterCount = 0;
    vector<string> vOutput;
    int iLength = solution.size();
    int iPos = 0;
    d1 rho = computeField(solution, guess, iWidth);
    cout << flush;
    do
    {
        newGuess = old_guess;
        printAsImage(old_guess, iWidth);
        function(rho, newGuess, iWidth, iLength, 1);

        //TODO: or is it the other way around?
        dError = findError(old_guess, newGuess, iLength);
        // dError = findError(newGuess, old_guess, iLength);
        //TODO: or is it the other way around?
        double dDiff = meanDifference(solution, newGuess, iLength);
        old_guess = newGuess;
        iIterCount++;

        // cout << "Iteration diff(max): " << dError << endl;
        // cout << "Image diff(mean): " << dDiff << endl;
        vOutput.push_back(std::to_string(dDiff));
        iPos++;
        // if(iPos > 2) break;
    } while(dError > dMaxErr);

    // cout << vOutput.size() << " iterations" << endl;
    guess = newGuess;

    return vOutput;
}

} // EndOfNamespace

#endif

/* EOF */
