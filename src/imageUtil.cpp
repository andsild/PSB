#include "imageUtil.hpp"

#include <limits.h>
#include <math.h>

#include <list>
#include <iostream>
#include <sstream>
#include <string>

#include "CImg.h"

#include "loginstance.hpp"

using namespace cimg_library;

namespace image_util
{
/** Simple function to render a CImg (useful for threading)
*/
void renderImage(CImgDisplay disp)
{
    disp.show();
    while(!disp.is_closed() && ! disp.is_keyQ() )
    {
        disp.wait();
    }

    disp.close();
}

/** Pad an image with zeroes along the boundary
  @param iNewWidth is the new width of the image (in pixels)
  @param iNewHeight is the new height of the image (in pixels)
  @param input if the image to pad
  @return is the new, zero-padded image.
*/
image_fmt padCore(int iNewWidth, int iNewHeight, const image_fmt &input)
{
    int iStartX = iNewWidth  / 2 - input.width()  / 2,
        iStartY = iNewHeight / 2 - input.height() / 2;
    image_fmt ret(iNewWidth, iNewHeight, 1, 1, 0);
    ret.draw_image(iStartX, iStartY, input);
    return ret;
}

/** Comput the solver domain for iterative methods. This means
  zero-initializing an image and copying its border (Dirichlet)

  @param input is the original image to extract from
  @param ret is the returned solver domain.
*/
void makeInitialGuess(const image_fmt &input, image_fmt &ret)
{
    if(input.size() < 1)
    {
        std::string sMsg = "input was not initialized before creating border"
                           " [[ width: " + std::to_string(input.width())
                           + " height: " + std::to_string(input.height()) + " ]]";
        throw ImageException(sMsg.c_str());
    }

    const int DEFAULT_GUESS_VAL = 0;
    /* Put DEFAULT_GUESS_VAL for all pixels */
    ret.assign(input, "xyz", DEFAULT_GUESS_VAL);

    /* Copy the border */
    cimg_for_borderXY(input,x,y,BORDER_SIZE)
    {
        ret(x,y) = input(x,y);
    }
}


/** Round the pixel values of an image to default RGB values, so that it can
   be presented on a screen

   @param image is the image to round
*/
void roundValues(image_fmt &image)
{
    image.round(0).cut(0,255);
}

/** Transform 1d vector data to a 1d CImg.
  The returned image will have width 1 and @param data size y dimension
*/
image_fmt vectorToImage(rawdata_fmt &data)
{
    image_fmt imgData(1, data.size(), 1, 1, false);
    cimg_forY(imgData,y)
        imgData(0, y) = data[y];

    return imgData;
}

/** Pad an image with zeroes at the end
    @param input is the image to pad
    @param iPadLength is the amount of pixels to extend the image with (zeroes)
    @return is the new, paddded image
*/
image_fmt padImage(const image_fmt &input, const int iPadLength)
{
    if(input.height() == iPadLength)  return input;
    image_fmt padded(1, iPadLength, 1, 1, 0);
    padded += input;
    for(int iPos = input.height() - 1; iPos < iPadLength; iPos++)
        padded(0, iPos) = 0;

    return padded;
}

/** DEPRECATED: Currently not in use.
  Average the result from a nested set of vectors
  If vector i is longer than vector j, then the average will
  calculate the average between i and j for as long as j has elements.
  After j is empty, the average will continue, calculating average of the
  remaining vectors.
  I.e. the average function is (sum from iterator vector elements)
                                            /
                             (number of vectors that still has elements)
*/
rawdata_fmt averageResult(const std::vector<rawdata_fmt> &vInput)
{
    int iShortest = INT_MAX, iLongest = INT_MIN;
    std::vector<rawdata_fmt>::iterator it;
    std::list<int> lLengths;
    int iDivisionCounter = DIVISION_SIZE;
    for(auto const it : vInput)
    {
        int iSize = it.size();
        iShortest = (iShortest < iSize) ? iShortest : iSize;
        iLongest  = (iLongest  > iSize) ? iLongest :  iSize;
        lLengths.push_back(iSize);
    }

    rawdata_fmt vRes(iLongest, 0);

    for(const auto it : vInput)
    {
        for(int iPos = 0; iPos < it.size(); iPos++)
        {
            vRes.at(iPos) += it.at(iPos);
        }
    }

    lLengths.sort(); // lowest first
    int iPos = 0;
    for(auto & it : vRes)
    {
        it = (double)( it / (double)iDivisionCounter);
        iPos++;
        if(iPos >= lLengths.front())
        {
            lLengths.pop_front();
            iDivisionCounter--;
        }
    }

    return vRes;
}

/** Format a string using printf format
  @param fmt is the format string
  @param ... are the elements to parsed in @param fmt
  @return is the formatted string
*/
std::string format(const char* fmt, ...)
{
    int size = 512;
    char* buffer = 0;
    buffer = new char[size];
    va_list vl;
    va_start(vl, fmt);
    int nsize = vsnprintf(buffer, size, fmt, vl);
    if(size<=nsize)
    {
        delete[] buffer;
        buffer = 0;
        buffer = new char[nsize+1]; //+1 for /0
        nsize = vsnprintf(buffer, size, fmt, vl);
    }
    std::string ret(buffer);
    va_end(vl);
    delete[] buffer;
    return ret;
}

/** Convert a CImg to a printed string
  Note that this method is very slow and has a large impact on
  running time.
*/
std::string printImage(const image_fmt image)
{
    std::stringstream ss;
    int iIndex = 0;
    char sign = ' ';
    cimg_for(image,ptr,data_fmt)
    {
        if( ((iIndex) % image.width()) == 0 && iIndex > 0) ss << "\n";
        if(*ptr < 0)
            sign = '-';
        else
            sign = ' ';
        ss << format("%c%-9.5f", sign, cimg::abs(*(ptr)));
        iIndex++;
    }

    return ss.str();
}

std::string printImageAligned(const image_fmt image, int iCols)
{
    std::stringstream ss;
    int iIndex = 0, iOffset = 0;
    const int iColumns = iCols;
    char sign = ' ';
    for(int iPos = 0; iPos < ceil(image.width() / (double)iColumns); iPos++)
    {
        cimg_forY(image, y)
        {
            for(int x = iOffset; (x < (iOffset + iColumns) && x < image.width()); x++)
            {
                if(image(x,y) < 0)
                    sign = '-';
                else
                    sign = ' ';
                ss << format("%c%-9.5f", sign, cimg::abs(image(x,y)));
            }
            ss << "\n";
        }
        iOffset += iColumns;
        ss << "\n####################\n";
    }

    return ss.str();
}

/** Convert an image to grayscale
*/
//TODO: ensure that image handles all formats (no errors for specific color channels)
void toGrayScale(image_fmt &image)
{
    if(image.spectrum() == 1)
    {
        return;
    }
    image_fmt grayscale(image.width(), image.height(),1, 1, 0);
    grayscale = image.get_norm().normalize(0,255);
    image = grayscale;
}

/** Compute the field from an image
*/
void makeField(
        const image_fmt &input, const data_fmt modifier, image_fmt &ret)
{
    int iKernDim = 3;
    /* Discrete poison stencil */
    image_fmt kernel(iKernDim, iKernDim, 1, 1,
                    0,1,0,
                    1,-4,1,
                    0,1,0);
    ret = input.get_convolve(kernel, 0);
    if(modifier != 1.0)
    {
        ret *= modifier;
    }
}


/** Load an image
  @param fileDest is the name of the image
  @param image is the image to load
  @return false if image could not be loaded successfully and print error message
*/
bool readImage(image_fmt &image, std::string sFileName)
{
    cimg::exception_mode(0);
    try
    {
        image.load(sFileName.c_str());
    }
    catch(CImgIOException &cioe)
    {
        std::cerr << cioe.what() << std::endl;
        return false;
    }
    return true;
}

/** Calculate the mean square error between two images
  @param source
  @param comparator
  @return the difference from source to comparator
  */
double imageDiff(const image_fmt &source, const image_fmt &comparator)
{
    return (double)(source.MSE(comparator));
}

/** Split an image, its field and guess separated regions
  @param iDivSize the amount of split to perform. Has to be square
  @param origImage the original image to split from
  @param rho the original rho to split from
  @param origimagelist a return list with iDivSize different splits
  @param rhoList a return list with iDivSize different splits
  @param guessList a return list with iDivSize different splits
*/
void divide(image_fmt* const origImage, image_fmt* const rho,
            std::vector<image_fmt *> &origImageList,
            std::vector<image_fmt *> &rhoList,
            std::vector<image_fmt *> &guessList)
{
    /* If we are not supposed to divide the image */
    if(DIVISION_SIZE == 1 || DIVISION_SIZE == 0)
    {
        origImageList.push_back(origImage);
        rhoList.push_back(rho);
        image_fmt* guess = new image_fmt;
        makeInitialGuess(*origImage, *guess);
        guessList.push_back(guess);
        return;
    }

    /* Simple division, horizontally in the middle */
    if(DIVISION_SIZE == 2)
    {
        int w = origImage->width(); int h = origImage->height();
        image_fmt origImageC1 = origImage->get_crop(0, 0, 0, 0, w, h / 2, 0, 0),
                  origImageC2 = origImage->get_crop(0, h / 2, 0, 0, w, h, 0, 0);
        origImageList.push_back(&origImageC1);
        origImageList.push_back(&origImageC2);

        image_fmt rhoC1 = rho->get_crop(0, 0, 0, 0, w, h / 2, 0, 0),
                  rhoC2 = rho->get_crop(0, h / 2, 0, 0, w, h, 0, 0);
        rhoList.push_back(&rhoC1);
        rhoList.push_back(&rhoC2);

        image_fmt* guess = new image_fmt;
        makeInitialGuess(*origImage, *guess);
        image_fmt guessC1 = guess->get_crop(0, 0, 0, 0, w, h / 2, 0, 0),
                  guessC2 = guess->get_crop(0, h / 2, 0, 0, w, h, 0, 0);
        guessList.push_back(&guessC1);
        guessList.push_back(&guessC2);
        return;
    }

    const int iWidth = origImage->width(), iHeight = origImage->height();
/* G++ will complain here that there might be a division by zero if DIVISION_SIZE < 2. 
   However, all cases that produce 0 should be caught in the conditional branches
   above. Thus the pragma. Note that it is popped afterward to re-enable warnings.
*/
#pragma GCC diagnostic ignored "-Wdiv-by-zero"
    const int WIDHT_REGION = (iWidth / (DIVISION_SIZE / 2));
    const int HEIGHT_REGION = (iHeight / (DIVISION_SIZE / 2));
#pragma GCC diagnostic pop
    // std::cerr << DIVISION_SIZE << std::endl;
    // exit(EXIT_FAILURE);
    image_fmt* ptr;
    imageList_fmt tmpOrigList, tmpRhoList, tmpGuessList;

    /* Since the code pushes back pointers, we need two iterations: one to
       allocate the images, and one to push back their addresses. Otherwise,
       all the pointers would point to same address.

       The for loop begins top left, then bottom left, so on..
    */
    for(int xSlice = 0; xSlice < DIVISION_SIZE / 2; xSlice++)
    {
        for(int ySlice = 0; ySlice < DIVISION_SIZE / 2; ySlice++)
        {
            int iLeftmostX = xSlice * WIDHT_REGION,
                iUpperY = ySlice * HEIGHT_REGION;
            int iRightmostX = iLeftmostX + WIDHT_REGION -1,
                iLowerY = iUpperY + HEIGHT_REGION - 1;

            if(WIDHT_REGION % 2 == 1 && xSlice == 0)
                iRightmostX++;
            if(HEIGHT_REGION % 2 == 1 && ySlice == 0)
                iLowerY++;

            if(iRightmostX > iWidth)
                iRightmostX = iWidth - 1;
            if(iLowerY > iHeight)
                iLowerY = iHeight - 1;

            image_fmt origImg = origImage->get_crop(iLeftmostX, iUpperY, 0,
                                                iRightmostX,
                                                iLowerY, 0);
            tmpOrigList.push_back(origImg);
            image_fmt rhoPush = rho->get_crop(iLeftmostX, iUpperY, 0,
                                            iRightmostX,
                                            iLowerY, 0) ;
            tmpRhoList.push_back(rhoPush);

            image_fmt retRegion(origImg, "xyz", 0);
            cimg_for_borderXY(origImg,x,y,BORDER_SIZE)
            {
                retRegion(x,y) = origImg(x,y);
            }
            tmpGuessList.push_back(retRegion);
        }
    }
    /* We made image regions; now get their memory addresses */
    for(int iPos = 0; iPos < tmpOrigList.size(); iPos++)
    {
        image_fmt* origImg  =  new image_fmt(),
                 * rhoImg   =  new image_fmt(),
                 * guessImg =  new image_fmt();
        *origImg    =  tmpOrigList[iPos];
        *rhoImg     =  tmpRhoList[iPos];
        *guessImg =  tmpGuessList[iPos];
        origImageList.push_back(origImg);
        rhoList.push_back(rhoImg);
        guessList.push_back(guessImg);
    }
}

/** Compute a prior for an image.
  The border condition used it ZZ
*/

/** Put together an image that has been divided into regions.
    The method will assume the image was split along the y axis first,
    then x axis.

    @param list is the list of image to join together
    @return is the joined image
*/
image_fmt joinImage(imageList_fmt list)
{
    image_fmt img((unsigned int)0, (unsigned int)0,(unsigned int)1);
    if(DIVISION_SIZE == 1)
    {
        return list.front();
    }
    for(int xSlice = 0; xSlice < DIVISION_SIZE / 2; xSlice++)
    {
        image_fmt tmp((unsigned int)0, (unsigned int)0,(unsigned int)1);
        for(int ySlice = 0; ySlice < DIVISION_SIZE / 2; ySlice++)
        {
            tmp.append(list.front(), 'y', 0);
            list.pop_front();
        }
        img.append(tmp, 'x', 0);
    }

    return img;
}
} /* EndOfNameSpace */
