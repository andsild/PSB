#include "image2.hpp"

#include <limits.h>
#include <math.h>

#include <list>
#include <iostream>
#include <sstream>
#include <string>

#include "CImg.h"

#include "imageedit.hpp"
#include "loginstance.hpp"
// #include "loadingbar.hpp"
#include "iterative_solvers.hpp"
#include "wavelet.hpp"
#include "solver.hpp"
#include "file.hpp"
#include "plot.hpp"
#include "fft.hpp"

using namespace cimg_library;
using namespace file_IO;
using namespace plot;
// using namespace logging;
using namespace solver;
using namespace image_display;

namespace image_psb
{
image_fmt makeInitialGuess(const image_fmt &image);

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

/** Average the result from a nested set of vectors
  If vector i is longer than vector j, then the average will 
  calculate the average between i and j for as long as j has elements.
  After j is empty, the average will continue, calculating average of the
  remaining vectors. 
  I.e. the average function is (sum from iterator vector elements) 
                                            / 
                             (number of vectors that still has elements)
*/
rawdata_fmt averageResult(const std::vector<rawdata_fmt> &vInput, int iDivSize)
{
    int iShortest = INT_MAX, iLongest = INT_MIN;
    std::vector<rawdata_fmt>::iterator it;
    std::list<int> lLengths;
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
        it = (double)( it / (double)iDivSize);
        iPos++;
        if(iPos >= lLengths.front())
        {
            lLengths.pop_front();
            iDivSize--;
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
    // cimg_forXY(image, x,y)
    // {
    cimg_for(image,ptr,data_fmt)
    {
        if( ((iIndex) % image.width()) == 0 && iIndex > 0) ss << "\n";
        if(*ptr < 0)
            sign = '-';
        else
            sign = ' ';
        ss << format("%c%-10.2f", sign, cimg::abs(*(ptr)));
        iIndex++;
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
image_fmt makeRho(const image_fmt &input)
{
    int iKernDim = 3;
    /* Discrete poison stencil */
    image_fmt kernel(iKernDim, iKernDim, 1, 1,
                    0,1,0,
                    1,-4,1,
                    0,1,0);
    return input.get_convolve(kernel, 0);
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
void divide(int iDivSize, const image_fmt &origImage, const image_fmt &rho,
            imageList_fmt &origImageList, imageList_fmt &rhoList, imageList_fmt &guessList)
{

    if(iDivSize == 1)
    {
        origImageList.push_back(origImage);
        rhoList.push_back(rho);
        guessList.push_back(makeInitialGuess(origImage));
        return;
    }
    
    if(iDivSize == 2)
    {
        int w = origImage.width(); int h = origImage.height();
        origImageList.push_back(origImage.get_crop(0, 0, 0, 0, w, h / 2, 0, 0));
        origImageList.push_back(origImage.get_crop(0, h / 2, 0, 0, w, h, 0, 0));
        rhoList.push_back(rho.get_crop(0, 0, 0, 0, w, h / 2, 0, 0));
        rhoList.push_back(rho.get_crop(0, h / 2, 0, 0, w, h, 0, 0));

        image_fmt guess = makeInitialGuess(origImage);
        guessList.push_back(guess.get_crop(0, 0, 0, 0, w, h / 2, 0, 0));
        guessList.push_back(guess.get_crop(0, h / 2, 0, 0, w, h, 0, 0));
        return;
    }


    const int WIDHT_REGION = (origImage.width() / (iDivSize / 2));
    const int HEIGHT_REGION = (origImage.height() / (iDivSize / 2));

    for(int xSlice = 0; xSlice < iDivSize / 2; xSlice++)
    {
        for(int ySlice = 0; ySlice < iDivSize / 2; ySlice++)
        {
            int iLeftmostX = xSlice * WIDHT_REGION,
                iUpperY = ySlice * HEIGHT_REGION;
            int iRightmostX = iLeftmostX + WIDHT_REGION -1,
                iLowerY = iUpperY + HEIGHT_REGION - 1;

            if(WIDHT_REGION % 2 == 0 && xSlice == 0)
                iRightmostX++;
            if(HEIGHT_REGION % 2 == 0 && ySlice == 0)
                iLowerY++;

            if(iRightmostX > origImage.width())
                iRightmostX = origImage.width() - 1;
            if(iLowerY > origImage.height())
                iLowerY = origImage.height() - 1;

            image_fmt origImg = origImage.get_crop(iLeftmostX, iUpperY, 0,
                                                iRightmostX,
                                                iLowerY, 0);
            origImageList.push_back(origImg);

            rhoList.push_back(rho.get_crop(iLeftmostX, iUpperY, 0,
                                                                iRightmostX,
                                                                iLowerY, 0));

            image_fmt retRegion(origImg, "xyz", 0);
            cimg_for_borderXY(origImg,x,y,BORDER_SIZE)
            {
                retRegion(x,y) = origImg(x,y);
            }
            guessList.push_back(retRegion);
        }
    }

}

/** Compute a prior for an image.
  The border condition used it ZZ
*/

image_fmt makeInitialGuess(const image_fmt &image)
{
    if(image.size() < 1)
    {
        std::string sMsg = "Image was not initialized before creating border"
                           " [[ width: " + std::to_string(image.width()) 
                           + " height: " + std::to_string(image.height()) + " ]]";
        throw ImageException(sMsg.c_str());
    }

    image_fmt U;
    const int DEFAULT_GUESS_VAL = 0;

    U.assign(image, "xyz", DEFAULT_GUESS_VAL);

    cimg_for_borderXY(image,x,y,BORDER_SIZE)
    {
        U(x,y) = image(x,y);
    }

    return U;
}

/** Put together an image that has been divided into regions.
    The method will assume the image was split along the y axis first,
    then x axis.

    @param list is the list of image to join together
    @param DIVISION_SIZE is the number of splits that was made
    @return is the joined image
*/
image_fmt joinImage(imageList_fmt list, int DIVISION_SIZE)
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

/** Add an iterative solver for an image.
  Since the iterative solvers are split into different parts before they
  are rejoined, they need this method to tag the different parts
  so that other methods can know how to recombine them.
*/
void addIterativeSolver(std::vector<solver::Solver*> &vIn,
        const int DIVISION_SIZE, const double dTolerance,
        const std::string sFilename, const std::string sLabel,
        const iterative_func func,
        const imageList_fmt &origList, imageList_fmt &rhoList, const imageList_fmt &guessList)
{
    for(int iPos = 0; iPos < origList.size() - 1; iPos++)
    {
        vIn.push_back(new solver::IterativeSolver(origList[iPos],
                                            rhoList[iPos], guessList[iPos],
                                            func, dTolerance,
                                            sFilename, sLabel, true));
    }
    /* Tag the last image with "isFinal" */
    vIn.push_back(new solver::IterativeSolver(origList.back(),
                                        rhoList.back(), guessList.back(),
                                        func, dTolerance,
                                        sFilename, sLabel, true, true));
}

/** The main entry point for solvers to an image.

  For each solver(boolean), add the corresponding method and field,
  then execute solve().
*/
void processImage(std::string sFilename, double dTolerance, double dResolve,
                  const bool gauss, const bool jacobi, const bool sor,
                  const bool wav, const bool fft)
{

    image_fmt use_img;
    std::vector<solver::Solver*> vSolvers;
    const int DIVISION_SIZE = 4;

    if(!readImage(use_img, sFilename))
    {
        std::cerr << "Error:: could not load image: " << sFilename << std::endl;
        return;
    }
    toGrayScale(use_img);
    image_fmt field = makeRho(use_img),
              guess = makeInitialGuess(use_img);
    // MLOG(severity_type::extensive, "Initial image\n", printImage(use_img));
    // MLOG(severity_type::extensive, "Initial guess\n", printImage(guess));
    // MLOG(severity_type::extensive, "Initial rho\n", printImage(field));
    imageList_fmt origList, guessList, rhoList;


    // imageList_fmt test = use_img.get_gradient("xy", -1);
    // const image_fmt divergence = test[0].get_gradient("x",-1)[0] + test[1].get_gradient("y",-1)[0];
    // MLOG(severity_type::error, "\n", printImage(use_img ));
    // MLOG(severity_type::error, "\n", printImage(field));
    // MLOG(severity_type::error, "\n", printImage(test[0]));
    // MLOG(severity_type::error, "\n", printImage(test[1]));
    // MLOG(severity_type::error, "\n", printImage(divergence));
    // return;


    divide(DIVISION_SIZE, use_img, field, origList, rhoList, guessList);

    if(gauss)
    {
        std::string sLabel = "gauss";
        addIterativeSolver(vSolvers, DIVISION_SIZE, dTolerance, sFilename, sLabel,
                            solver::iterate_gauss, origList, rhoList, guessList);
    }
    if(jacobi)
    {
        std::string sLabel = "jacobi";
        addIterativeSolver(vSolvers, DIVISION_SIZE, dTolerance, sFilename, sLabel,
                            solver::iterate_jacobi, origList, rhoList, guessList);
    }
    if(sor)
    {
        std::string sLabel = "sor";
        addIterativeSolver(vSolvers, DIVISION_SIZE, dTolerance, sFilename, sLabel,
                            solver::iterate_sor, origList, rhoList, guessList);
    }
    if(fft)
    {
        std::string sLabel = "fft";
        vSolvers.push_back(new solver::DirectSolver(use_img, field,
                                                    solver::FFT2D,
                                                    sFilename, sLabel, false));
    }
    if(wav)
    {
        std::string sLabel = "wavelet";
        vSolvers.push_back(new solver::DirectSolver(use_img, field,
                                                    wavelet::pyconv,
                                                    sFilename, sLabel, false));
    }

    imageList_fmt accumulator; /*< container for subdivisions of solved image */
    rawdata_fmt vResults;
    int iPartIndex = 0;

    for(auto it : vSolvers) // for each solver for each image (and its divisions)
    {
        image_fmt result = it->solve(vResults); /*< result now holds the resulting image,
                                                  < vResults holds the imagediffs */
        /* Multipart images: solve each region before moving past this if block */
        if(it->isMultipart()) 
        {
            accumulator.push_back(result);
            /* We can now merge the regions together */
            if(it->isFinal())
            {
                result = joinImage(accumulator, DIVISION_SIZE);
                accumulator.clear();
                iPartIndex = 0;
            }
            else
            {
                std::string sFilename = it->getFilename() + std::to_string(iPartIndex);
                file_IO::writeData(vResults, it->getLabel(), sFilename);
                iPartIndex++;
                vResults.clear(); // important, otherwise it stacks results
                continue;
            }

        }

        /* Before saving the image, round the values so that the image can
           be viewed later */
        roundValues(result);
        std::string sSavename = file_IO::SAVE_PATTERN.getSavename(sFilename, it->getLabel(), false);
        roundValues(result);
        file_IO::saveImage(result, sSavename, false);
        file_IO::writeData(vResults, it->getLabel(), it->getFilename());
        /* Erase before re-iterating */
        vResults.erase(vResults.begin(), vResults.end());
        // DO_IF_LOGLEVEL(severity_type::extensive)
        // {
        //     std::string sMsg = "Final image(cut)\n" + printImage(result);
        //     it->log(1, sMsg);
        // }


    }
        if(dResolve != 1.0)
        {
            for(auto it : vSolvers) // for each solver for each image (and its divisions)
            {
                it->alterField(dResolve);
                image_fmt result = it->solve(vResults); /*< result now holds the resulting image,
                                                        < vResults holds the imagediffs */
                /* Multipart images: solve each region before moving past this if block */
                if(it->isMultipart()) 
                {
                    accumulator.push_back(result);
                    /* We can now merge the regions together */
                    if(it->isFinal())
                    {
                        result = joinImage(accumulator, DIVISION_SIZE);
                        accumulator.clear();
                        iPartIndex = 0;
                    }
                    else
                    {
                        std::string sFilename = it->getFilename() + std::to_string(iPartIndex);
                        file_IO::writeData(vResults, it->getLabel(), sFilename);
                        iPartIndex++;
                        vResults.clear(); // important, otherwise it stacks results
                        continue;
                    }
                }

                roundValues(result);
                std::string sSavename = file_IO::SAVE_PATTERN.getSavename(sFilename, it->getLabel(), true);
                // roundValues(result);
                file_IO::saveImage(result, sSavename, true);
                file_IO::writeData(vResults, it->getLabel(), it->getFilename());
                /* Erase before re-iterating */
                vResults.erase(vResults.begin(), vResults.end());
            }
        }
}

} /* EndOfNameSpace */
