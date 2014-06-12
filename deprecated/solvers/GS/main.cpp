#include <iostream>

#include "gauss.cpp"

using namespace pe_solver;

int main()
{
   matrix_type test1 {{1, 2, 3}, {4, 5, 6}, {7,8,9}};
   matrix_type test2 {{12, 3, -5}, {1, 5, 3}, {3,7,13}};
   double max_error = .9;
   // vector<double> sol(3, 3);
   vector<double> sol {1, 28, 76};
   vector<double> initGuess {1, 0, 1};

   // matrix_type x1 = test(test1, sol, initGuess);
   matrix_type x2 = test(test2, sol, initGuess, max_error);

   // printmatrix_type(x2);
}

/* EOF */
