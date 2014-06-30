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
typedef void (*iterative_function)(const d1 &arg1, d1 &arg, double, int, double) ;

double calculateError(double, double);

double getRangeVal(const d1 &U, const d1 &F,
        const int iIndex, const int iWidthLength, const double H = 1.0)
{
    int xPos = iIndex % (int)iWidthLength;
    /* Skip the first and last column */
    if(xPos < 1 || xPos > iWidthLength - 2) { return -999;}// return F[iIndex]; }
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
void iterate_jacobi(const d1 &F, d1 &U, double iWidthLength,
                    int iLength, double H = 1)
{
    d1 copyU = U;
    for(vector<int>::size_type iPos = iWidthLength;
            iPos < F.size() - iWidthLength;
            iPos++) 
    {
        double dNewVal = getRangeVal(copyU, F, iPos, iWidthLength);
        if(dNewVal==-999) continue;
        U[iPos] = (double)(.25 * (dNewVal));
    }
}



/** Gauss-seidel iteration
 *
 */
void iterate_gauss(const d1 &F, d1 &U, double iWidthLength,
                           int iLength, double H = 1)
{
    for(vector<int>::size_type iPos = iWidthLength;
            iPos < (int)F.size() - iWidthLength;
            iPos++) 
    {
        U[iPos] = .25 * (getRangeVal(U, F, iPos, iWidthLength, H));
    }
}

void iterate_sor(const d1 &F, d1 &U, double iWidthLength, int iLength, double H = 1)
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

double calculateError(const double dOriginal, const double dNew)
{
    if(dNew== 0) { return dOriginal * 100; }
    // return abs( (dNew - dOriginal) );
    return (double)abs( (dNew - dOriginal) / dNew) * 100;
}

double findRelativeError(const d1 origData, const d1 newData, int iWidthLength)
{
    double *dTmp = new double[origData.size()];

    for(std::vector<int>::size_type iPos = 0; iPos != origData.size(); iPos++)
    {
        double dRet = calculateError(origData[iPos],
                                 newData[iPos]);
        dTmp[iPos] = dRet;
    }
    double dMax = *std::max_element(dTmp, dTmp+origData.size());
    delete dTmp;
    return dMax;
}

double l2norm(const d1 data)
{
    double dTot = 0;
    for(d1::const_iterator it = data.begin(); it != data.end(); it++)
    {
        dTot += pow(*it, 2);
    }
    return sqrt(dTot);
}

double meanDifference(const d1 origData, const d1 newData, int iWidthLength)
{
    double dRelativeError = 0;
    // for(std::vector<int>::size_type iPos = 0; iPos != origData.size(); iPos++)
    for(std::vector<int>::size_type iPos = iWidthLength; iPos != origData.size() - iWidthLength; iPos++)
    {
        int xPos = iPos % (iWidthLength);
        if(xPos < 1 || xPos > iWidthLength - 2) { continue; }
        double dRes = calculateError(origData[iPos],
                                 newData[iPos]);
        // if(dRes == CHANGE_FROM_ZERO) { iLength--; continue; }
        dRelativeError += dRes;
        // dRelativeError += calculateError(origData[iPos],
        //                          newData[iPos]);
    }
    return dRelativeError /= (double)origData.size();

}

d1 computeFieldRho(const d1 &orig, const d1 &guess, int iWidthLength)
{
    d1 rho = orig;
    for(vector<int>::size_type iPos = iWidthLength;
            iPos < (int)orig.size() - iWidthLength;
            iPos++) 
    {
        int xPos = iPos % (iWidthLength);
        if(xPos < 1 || xPos > iWidthLength - 2) { continue; }
        int iIndexPixelAbove = iPos - iWidthLength;
        int iIndexPixelBelow = iPos + iWidthLength;

        //Symmetric finite difference with simple kernel 
        rho[iPos] = -1 *
                    (orig[iIndexPixelAbove] + orig[iIndexPixelBelow]
                    + orig[iPos + 1] + orig[iPos - 1] 
                    - (4 * orig[iPos] ));
    }
    
    return rho;
}

void printAsImage(d1 vec, int iW)
{
    for(int iPos = 0; iPos < vec.size(); iPos++)
    {
        if(iPos % iW == 0) cout << endl;
        // if(vec[iPos] == 0) continue;
        printf("%5.1f ",vec[iPos]);
    }
    cout << endl << flush;
}


vector<string> iterative_solve(iterative_function function,
                    const d1 solution, d1 &guess,
                    double dMaxErr, int iWidth) 
{
    d1 old_guess = guess, newGuess;
    double dRelativeError = 0;
    vector<string> vOutput;
    int iLength = solution.size();
    d1 rho = computeFieldRho(solution, guess, iWidth);


    cout << "Initial image" << endl;
    // printAsImage(solution, iWidth) ;
    cout << "Initial guess" << endl;
    printAsImage(guess, iWidth) ;
    cout << "Initial rho" << endl;
    printAsImage(rho, iWidth);
    cout << "Entering loop..." << endl;

    int iIter = 0;
    do
    {
        iIter++;
        if(iIter % 1000 == 0) { cout << "=== [ solving: " << dRelativeError 
                                     << " ] ===" << endl;}
        double dTmp = dRelativeError;
        newGuess = old_guess;
        function(rho, newGuess, iWidth, iLength, 1); // not fast enough
        // cout << "New guess:" << endl;
        // printAsImage(newGuess, iWidth);

        dRelativeError = findRelativeError(old_guess, newGuess, iWidth);
        double dDiff = meanDifference(solution, newGuess, iWidth);
        old_guess = newGuess;

        // cout << "Iteration diff(max): " << dRelativeError << endl;
        // cout << "Image diff(mean): " << dDiff << endl;
        vOutput.push_back(std::to_string(dDiff));
    } while(dRelativeError > dMaxErr);

    cout << vOutput.size() << " iterations" << endl;
    guess = newGuess;

    return vOutput;
}

} // EndOfNamespace

#endif

/* EOF */
