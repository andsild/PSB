#ifndef _IMAGE_H
#define _IMAGE_H

#define DIVISION_SIZE 1

#include <string>

#include "CImg.h"

#include "image_types.hpp"

namespace image_psb
{

image_fmt padImage(const image_fmt &arg1, const int);
image_fmt vectorToImage(rawdata_fmt &arg1);
void toGrayScale(image_fmt &arg1);
void renderImage(cimg_library::CImgDisplay disp);
double imageDiff(const image_fmt &arg1, const image_fmt &arg2);

std::string printImage(const image_fmt);
std::string printImageAligned(const image_fmt, int iCols = 8);
std::string printCore(const image_fmt, const int);
void scanAndAddImage(std::string sRootdir, std::string sSolverdir);
image_fmt padCore(int, int, const image_fmt &arg1);
rawdata_fmt averageResult(const std::vector<rawdata_fmt> &arg);

void processImage(std::string, double, double, data_fmt resolve, 
        const bool, const bool, const bool, /* Iterative */
        const bool, const bool, /* FFT */
        const bool, const bool, const bool); /* wavelet */
    
} /* EndOfNamespace */

#endif
