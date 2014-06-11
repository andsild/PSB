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
CImg<double> iterate(CImg<double> A, const CImg<double> F,
        CImg<double> U) 
{
    // 1 iteration of gauss-seidel
    for (int iPos = 0; iPos < A.width(); iPos++) 
    {
        double sum = 0;
        for (int jPos = 0; jPos < A.height(); jPos++) 
        {
            if (iPos != jPos) 
            {
                // Number in A times it's coefficient
                sum += U(0,jPos*A.width()) * A(iPos, jPos);// * U[jPos];
                if(std::isinf(sum)) 
                {int a = 1;}
            }
        }
        if(A(iPos, iPos) == 0) 
        { 
            U(0, iPos) = 0;
            continue;
        }
        double temp = ( (F(0, iPos*A.width()) - sum) / A(iPos, iPos));
        if(std::isinf(temp)) 
        {
            int a = 1;
        }

        U(0, iPos*A.width()) = temp;
    }
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
matrix_type test(CImg<double> inmatrix, const CImg<double> solution,
                 CImg<double> guess, double dMaxErr) 
{
    double dMax = 0;
    CImg<double> old_guess = guess;
    do
    {
        CImg<double> newGuess = iterate(inmatrix, solution, old_guess);

        print_image(old_guess);
        print_image(newGuess);
        cout << "#### EOG ###" << endl;

        // getting the errors
        CImg<double> errorLine(newGuess.height());
        //TODO: newGuess bigger than errorline
        for(int iPos = 0; iPos < newGuess.height(); iPos++)
        {
            double dRet = calculateError(old_guess(0, iPos),
                                            newGuess(0, iPos));
            errorLine(iPos) = dRet;
        }
        dMax = errorLine.max();
        cout << "Max error: " << dMax << endl;
        old_guess = CImg<double>(newGuess);

    } while(dMax > dMaxErr);

    return inmatrix;
}

double calculateError(const double dOriginal, const double dNew)
{
    if(dNew == 0) { return 0; }
    return abs( (dNew - dOriginal) / dNew) * 100;
}
}
