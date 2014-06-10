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
    for (double iPos = 0; iPos < image.width(); iPos++) 
    {
        double sum = solution[iPos];
        for (double jPos = 0; jPos < image.height(); jPos++) 
        {
            int a = 1;
            if (iPos != jPos) 
            {
                sum -= image(iPos, jPos) * guess[jPos];
            }
        }
        if(image(iPos, iPos) == 0) { guess[iPos] = 0; continue; }
        guess[iPos] = sum / image(iPos, iPos);
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
        d1 errorLine(3);
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
