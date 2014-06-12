#include <iostream>
#include <cmath>
#include <algorithm>
#include <vector>
#include <stdio.h>
#include <cmath>

#include "../../../lib/CImg-1.5.8/CImg.h"

using namespace cimg_library;
using namespace std;

namespace pe_solver
{
typedef vector<double> d1;
typedef CImg<double> matrix_type;

double calculateError(double, double);

/** Gauss-seidel iteration
 *
 */
CImg<double> iterate(const CImg<double> F, CImg<double> U, double iWidthLength) 
{
    // for(int iPos = 0; iPos < F.height(); iPos++)
    for(int iPos = iWidthLength; iPos < F.height() - iWidthLength; iPos++)
    {
        // TODO: make sure that boundaries are extended, currently at end-of-row
        // you extend to the next pixel in raster order.
        U(0, iPos) = .25 
                     * (U(0, iPos+1) + U(0, iPos-1)
                       + U(0, iPos - iWidthLength)
                       + U(0, iPos + iWidthLength)

                       + F(0, iPos) * F.height()
                    );
    }
        // 1 iteration of gauss-seidel
        // for (int iPos = 0; iPos < A.width(); iPos++) 
        // {
        //     double sum = 0;
        //     for (int jPos = 0; jPos < A.height(); jPos++) 
        //     {
        //         if (iPos != jPos) 
        //         {
        //             // Number in A times it's coefficient
        //             if(std::isinf(sum)) 
        //             {int a = 1;}
        //         }
        //     }
        //     if(A(iPos, iPos) == 0) 
        //     { 
        //         U(0, iPos) = 0;
        //         continue;
        //     }
        //     double temp = ( (F(0, iPos*A.width()) - sum) / A(iPos, iPos));
        //     if(std::isinf(temp)) 
        //     {
        //         int a = 1;
        //     }
        //
        //     U(0, iPos*A.width()) = temp;
        // }
    return U;
}

void print_image(const CImg<double> img)
{
    for(int iPos = 0; iPos < img.width(); iPos++)
    {
        for(int jPos = 0; jPos < img.height(); jPos++)
        {
            // if(img(iPos, jPos) == 0) { continue; }
            printf("\t%.2lf", img(iPos, jPos));
        }
        cout << endl << iPos+1 << "#: ";
    }
}

//TODO: check for diagonal dominance
//If you know that a matrix is diagonally dominant,
matrix_type test(const CImg<double> solution,
                 CImg<double> guess, double dMaxErr, int iWidth) 
{
    double dMax = 0;
    CImg<double> old_guess = guess;
    double dError = 0;
    int iIterCount = 0;
    do
    {
        dError = 0;
        CImg<double> newGuess = iterate(solution, old_guess, iWidth);

        // print_image(old_guess);
        // print_image(newGuess);
        // cout << "#### EOG ###" << endl;

        // getting the errors
        CImg<double> errorLine(newGuess.height());
        //TODO: newGuess bigger than errorline
        for(int iPos = 0; iPos < newGuess.height(); iPos++)
        {
            double dRet = calculateError(old_guess(0, iPos),
                                            newGuess(0, iPos));
            dError += dRet;
            // errorLine(iPos) = dRet;
        }
        dError /= newGuess.height();
        dMax = errorLine.max();
        // cout << "Max error: " << dMax << " after " << iIterCount << " iterations " << endl;
        // cout << "Max error: " << dError << " after " << iIterCount << " iterations " << endl;
        old_guess = CImg<double>(newGuess);
        iIterCount++;
        cout << iIterCount << "\t" << dError << endl;

    // } while(dMax > dMaxErr);
    } while(dError > dMaxErr);

    return solution;
}

double calculateError(const double dOriginal, const double dNew)
{
    if(dNew == 0) { return 0; }
    return abs( (dNew - dOriginal) / dNew) * 100;
}
}

/* EOF */
