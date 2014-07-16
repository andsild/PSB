#ifndef SOLVER_H
#define SOLVER_H

#include <iostream>
#include <cmath>
#include <algorithm>
#include <vector>
#include <stdio.h>
#include <cmath>
#include <string>
#include <sstream>

#include "CImg.h"
#include "image_types.hpp"
#include "loginstance.hpp"

namespace pe_solver //[p]oison-[e]quation
{

void iterate_jacobi(const cimg_library::CImg<double> &arg1,
                   cimg_library::CImg<double> &arg2,
                   double, int, double &arg3, double arg4);

void iterate_gauss(const cimg_library::CImg<double> &arg1,
                   cimg_library::CImg<double> &arg2,
                   double, int, double &arg3, double arg4);

void iterate_sor(const cimg_library::CImg<double> &arg1,
                 cimg_library::CImg<double> &arg2,
                   double, int, double &arg3, double arg4);

std::vector<std::string> iterative_solve(iterative_function function,
                    const cimg_library::CImg<double>,
                    cimg_library::CImg<double> &arg1, cimg_library::CImg<double>,
                    double, int,
                    logging::Logger< logging::FileLogPolicy > &arg2);

} /* EndOfNamespace */

#endif
