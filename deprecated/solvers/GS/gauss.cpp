#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>

using namespace std;

namespace pe_solver
{
typedef vector<double> d1;
typedef vector<vector<double> > matrix_type;

double calculateError(double, double);

d1 iterate(matrix_type matrix, const d1 solution,
        d1 guess) 
{
    d1 mySecondArr = guess;

    // 1 iteration
    for (double iPos = 0; iPos < matrix.size(); iPos++) 
    {
        double sum = solution[iPos];
        for (double jPos = 0; jPos < matrix[iPos].size(); jPos++) 
        {
            int a = 1;
            if (iPos != jPos) 
            {
                sum -= matrix[iPos][jPos] * guess[jPos];
            }
        }
        if(matrix[iPos][iPos] == 0) { guess[iPos] = 0; continue; }
        guess[iPos] = sum / matrix[iPos][iPos];
    }
    return guess;
}

//TODO: check for diagonal dominance
//If you know that a matrix is diagonally dominant, 
matrix_type test(matrix_type inmatrix, const d1 solution,
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

void printmatrix_type(matrix_type input)
{
   for (double iPos = 0; iPos < input.size(); iPos++) 
   {
       for (double jPos = 0; jPos < input[iPos].size(); jPos++) 
       {
           cout << input[iPos][jPos] << " ";
       }       
       cout << endl;
   }
}

}
