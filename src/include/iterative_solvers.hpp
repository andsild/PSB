#ifndef ITER_SOLVER_H
#define ITER_SOLVER_H

#include "image_types.hpp"

namespace solver //[p]oison-[e]quation
{


void iterate_gauss (const image_fmt &field, image_fmt &U, double &dDiff, const int, const int);
void iterate_jacobi(const image_fmt &field, image_fmt &U, double &dDiff, const int, const int);
void iterate_sor   (const image_fmt &field, image_fmt &U, double &dDiff, const int, const int);

} /* EndOfNamespace */

#endif
