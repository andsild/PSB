#ifndef _IMAGE_H
#define _IMAGE_H

#define DIVISION_SIZE 1
#define BORDER_SIZE 1

#include <string>

#include "CImg.h"

#include "image_types.hpp"

namespace image_util
{

image_fmt padImage(const image_fmt &arg1, const int);
image_fmt vectorToImage(rawdata_fmt &arg1);
void toGrayScale(image_fmt &arg1);
void renderImage(cimg_library::CImgDisplay disp);
double imageDiff(const image_fmt &arg1, const image_fmt &arg2);
void makeField( const image_fmt &arg1, const data_fmt, image_fmt &arg2);

void divide(image_fmt* const ptr1, image_fmt* const ptr2,
            std::vector<image_fmt *> &arg1,
            std::vector<image_fmt *> &arg2,
            std::vector<image_fmt *> &arg3);

bool readImage(image_fmt &arg1, std::string);
void roundValues(image_fmt &arg1);
image_fmt joinImage(imageList_fmt);

std::string printImage(const image_fmt);
std::string printImageAligned(const image_fmt, int iCols = 8);
std::string printCore(const image_fmt, const int);
void scanAndAddImage(std::string sRootdir, std::string sSolverdir);
image_fmt padCore(int, int, const image_fmt &arg1);
rawdata_fmt averageResult(const std::vector<rawdata_fmt> &arg);

} /* EndOfNamespace */

#endif
