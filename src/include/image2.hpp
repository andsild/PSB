#ifndef _IMAGE_H
#define _IMAGE_H

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
// bool readImage(image_fmt &arg1, const std::string);
// image_fmt makeRho(const image_fmt &input);
// image_fmt makeInitialGuess(const image_fmt &input, bool);
// void roundValues(image_fmt &arg);

// std::string format(const char *arg, ...);
std::string printImage(const image_fmt);
std::string printImageAligned(const image_fmt, int iCols = 8);
std::string printCore(const image_fmt, const int);
void scanAndAddImage(std::string sRootdir, std::string sSolverdir);
rawdata_fmt averageResult(const std::vector<rawdata_fmt> &arg, int);

void processImage(std::string, double, double, 
        const bool, const bool, const bool, /* Iterative */
        const bool, const bool, /* FFT */
        const bool, const bool, const bool); /* wavelet */
    
} /* EndOfNamespace */

#endif
