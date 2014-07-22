#ifndef _IMAGE_H
#define _IMAGE_H

#include <string>

#include "CImg.h"

#include "image_types.hpp"

namespace image_psb
{

void toGrayScale(image_fmt &arg);
double imageDiff(const image_fmt &arg1, const image_fmt &arg2);
// bool readImage(image_fmt &arg1, const std::string);
// image_fmt makeRho(const image_fmt &input);
// image_fmt makeInitialGuess(const image_fmt &input, bool);
// void roundValues(image_fmt &arg);

// std::string format(const char *arg, ...);
std::string printImage(image_fmt);
void scanAndAddImage(std::string sRootdir, std::string sSolverdir);

void processImage(std::string, double, double, const bool, const bool, const bool, const bool, const bool);

// void calculateAverage(std::string sFilePath);
    
} /* EndOfNamespace */

#endif
