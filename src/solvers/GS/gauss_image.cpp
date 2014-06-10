#include <iostream>
#include <cmath>
#include <algorithm>
#include <vector>

#include "../../../lib/CImg-1.5.8/CImg.h"

using namespace cimg_library;
using namespace std;

namespace pe_solver
{
typedef vector<double> d1;
typedef CImg<double> matrix_type;

double calculateError(double, double);

d1 iterate(CImg<double> image, const d1 solution,
        d1 guess) 
{
    d1 mySecondArr = guess;

    // 1 iteration
    for (int iPos = 0; iPos < image.width(); iPos++) 
    {
        double sum = 0;
        for (int jPos = 0; jPos < image.height(); jPos++) 
        {
            if (iPos != jPos) 
            {
                sum += (double)image(iPos, jPos, 0, 0);// * guess[jPos];
            }
        }
        // if((double)image(iPos, iPos, 0, 0) == 0) 
        // { 
        //     guess[iPos] = 0;
        //     continue;
        // }
        guess[iPos] = (solution[iPos] - sum) 
                            / 
                    ( (double)image(iPos, iPos, 0, 0));
        //guess[iPos] = sum / (double)image(iPos, iPos, 0, 0);
    }
    return guess;
}

//TODO: check for diagonal dominance
//If you know that a matrix is diagonally dominant, 
matrix_type test(CImg<double> inmatrix, const d1 solution,
                 d1 guess, double dMaxErr) 
{
    double dMax = 200;
    d1 old_guess = guess;
    do
    {
        d1 newGuess = iterate(inmatrix, solution, old_guess);

        // getting the errors
        d1 errorLine(inmatrix.width());
        for(int iPos = 0; iPos < newGuess.size(); iPos++)
        {
            errorLine[iPos] = calculateError(old_guess[iPos],
                                            newGuess[iPos]);
        }
        d1::iterator pos = max_element(errorLine.begin(),
                          errorLine.end());
        dMax = (double)*pos;
        old_guess = d1(newGuess);

        for(d1::const_iterator i = newGuess.begin();
            i != newGuess.end();
            i++){
                cout << *i << ' ';
        }
        cout << endl;

        cout << "error: " << *pos << endl;

    } while(dMax > dMaxErr);

    return inmatrix;
}

double calculateError(const double dOriginal, const double dNew)
{
    return abs( (dNew - dOriginal) / dNew) * 100;
}
}
