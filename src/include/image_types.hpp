#ifndef IMAGE_TYPE_H
#define  IMAGE_TYPE_H

#include <vector>

#include "CImg.h"

typedef cimg_library::CImgList<double> imageList_fmt;
typedef cimg_library::CImg<double> image_fmt;

typedef void (*iterative_function)(const cimg_library::CImg<double> &arg1,
        cimg_library::CImg<double> &arg2, double, int, double &arg3, double) ;

typedef void (*iterative_func)(const image_fmt &arg1,
        image_fmt &arg2, double &arg3) ;


#endif
