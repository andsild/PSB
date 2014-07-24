#ifndef PLOT_H
#define PLOT_H

#include "CImg.h"

#include "image_types.hpp"

namespace plot
{
cimg_library::CImgDisplay plot(const image_fmt &img);

} /* EndOfNamespace */

#endif
