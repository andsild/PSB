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
#include "logger.hpp"

namespace pe_solver //[p]oison-[e]quation
{

typedef void (*iterative_function)(const cimg_library::CImg<double> &arg1,
        cimg_library::CImg<double> &arg2, double, int, double &arg3, double) ;

void iterate_jacobi(const cimg_library::CImg<double> &arg1, cimg_library::CImg<double> &arg2,
                   double, int, double &arg3, double);
void iterate_gauss(const cimg_library::CImg<double> &arg1, cimg_library::CImg<double> &arg2,
                   double, int, double &arg3, double);
void iterate_sor(const cimg_library::CImg<double> &arg1, cimg_library::CImg<double> &arg2,
                   double, int, double &arg3, double);

std::vector<std::string> iterative_solve(iterative_function function,
                    const cimg_library::CImg<double>, cimg_library::CImg<double> &arg1, cimg_library::CImg<double>,
                    double, int,
                    logging::logger< logging::file_log_policy > &arg2);

} /* EndOfNamespace */

#endif
