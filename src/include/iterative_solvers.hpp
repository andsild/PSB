#ifndef _SOLVER
#define _SOLVER 1

#include <iostream>
#include <cmath>
#include <algorithm>
#include <vector>
#include <stdio.h>
#include <cmath>
#include <string>
#include <sstream>

#include "CImg.h"

#include "../main.cpp"
#include "./file.hpp"
#include "../image2.cpp"

using namespace cimg_library;

namespace pe_solver //[p]oison-[e]quation
{

typedef void (*iterative_function)(const CImg<double> &arg1, CImg<double> &arg2,
                                   double, int, double &arg3, double) ;

void iterate_jacobi(const CImg<double> &arg1, CImg<double> &arg2,
                   double, int, double &arg3, double);
void iterate_gauss(const CImg<double> &arg1, CImg<double> &arg2,
                   double, int, double &arg3, double);
void iterate_sor(const CImg<double> &arg1, CImg<double> &arg2,
                   double, int, double &arg3, double);

std::vector<string> iterative_solve(iterative_function function,
                    const CImg<double>, CImg<double> &arg1, CImg<double>,
                    double, int,
                    logging::logger< logging::file_log_policy > &arg2);

} /* EndOfNamespace */

#endif
