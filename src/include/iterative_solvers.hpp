#ifndef ITER_SOLVER_H
#define ITER_SOLVER_H

#include <vector>
#include <string>

#include "CImg.h"
#include "image_types.hpp"
#include "loginstance.hpp"

namespace solver //[p]oison-[e]quation
{

void iterate_jacobi(const image_fmt &arg1,
                   image_fmt &arg2,
                   double, int, double &arg3, double arg4);

void iterate_gauss(const image_fmt &arg1,
                   image_fmt &arg2,
                   double, int, double &arg3, double arg4);

void iterate_sor(const image_fmt &arg1,
                 image_fmt &arg2,
                   double, int, double &arg3, double arg4);

void iterate_sor2(const image_fmt &field, image_fmt &U, double &dDiff);

std::vector<std::string> iterative_solve(iterative_function function,
                    const image_fmt,
                    image_fmt &arg1, image_fmt,
                    double, int,
                    logging::Logger< logging::FileLogPolicy > &arg2);

} /* EndOfNamespace */

#endif
