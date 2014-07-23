#include "image2.hpp"

#include <math.h>
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

#define SPLIT_VALUE -1

using namespace cimg_library;
using namespace file_IO;
using namespace plot;
using namespace logging;
// using namespace loadbar;
using namespace solver;
using namespace image_display;

namespace image_psb
{
image_fmt makeInitialGuess(const image_fmt &image);

bool loadImage(const char *fileDest, image_fmt &image)
{
    cimg::exception_mode(0);
    try {
        image.load(fileDest);
        MLOG(severity_type::debug, "Loaded image ", fileDest,
                                  "to code format at ", &image);
        toGrayScale(image);
        MLOG(severity_type::debug, "Converted ", fileDest, " to grayscale");
    }
    catch(CImgIOException cioe)
    {
        MLOG(severity_type::error, cioe.what());
        return false;
    }

    return true;
}

void renderImage(CImgDisplay disp)
{
    disp.show();
    while(!disp.is_closed() && ! disp.is_keyQ() )
    {
        disp.wait();
    }
}

void roundValues(image_fmt &image)
{
    image.round(0).cut(0,255);
}

void display_histogram(image_fmt image)
{
    image.display_graph("Histogram", 3);
}


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

std::string printImage(const image_fmt image)
{
    std::stringstream ss;
    int iIndex = 0;
    cimg_forXY(image, x,y)
    {
        ss << format("%5.1f  ", image(x,y));
        if( ((iIndex + 1) % image.width()) == 0 && iIndex > 0) ss << "\n";
        iIndex++;
    }

    return ss.str();
}

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

double imageDiff(const image_fmt &source, const image_fmt &comparator)
{
    return (double)(source.MSE(comparator));
}

void divide(const int DIVISION_SIZE, const image_fmt &origImage, const image_fmt &rho,
            imageList_fmt &origImageList, imageList_fmt &rhoList, imageList_fmt &guessList)
{
    const int DEFAULT_GUESS_VAL = 0;

    // U.assign(image, "xyz", DEFAULT_GUESS_VAL);
    if(DIVISION_SIZE == 1)
    {
        origImageList.push_back(origImage);
        rhoList.push_back(rho);
        guessList.push_back(makeInitialGuess(origImage));
        return;
    }

    const int WIDHT_REGION = (origImage.width() / (DIVISION_SIZE / 2)),
              HEIGHT_REGION = (origImage.height() / (DIVISION_SIZE / 2));

    for(int xSlice = 0; xSlice < DIVISION_SIZE / 2; xSlice++)
    {
        for(int ySlice = 0; ySlice < DIVISION_SIZE / 2; ySlice++)
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



void scanAndAddImage(std::string sRootdir, std::string sSolverdir)
{
    std::vector<std::string> vFilenames = getFilesInFolder(sRootdir),
                        vSolvedNames = getFilesInFolder(sSolverdir);
    ImageDisplay id;

    for(auto const it : vFilenames)
    {
        id.addMainImage(it);
    }
    for(auto const it : vSolvedNames)
    {
        bool isResolved = false; std::string _, sLabel, sFilename;
        SAVE_PATTERN.getNames(it, _, sLabel, sFilename, isResolved);
        id.addResolvedImage2(it, sFilename, isResolved);
    }
    id.show();
    id.loop();
}

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
    vIn.push_back(new solver::IterativeSolver(origList.back(),
                                        rhoList.back(), guessList.back(),
                                        func, dTolerance,
                                        sFilename, sLabel, true, true));
}

/** Given n containers with results, "unify" each container such that
  each container has the same length.

  This is done using a simple interpolation scheme:
  given container A:
            1 2 3 4
        container B:
            1 2 
    After unify, for A:
            lengthDifferenceRatio = max(len(A), len(B)) / min(len(B), len(A))
            ( (1 + 2) / lengthdifferenceratio ) ( (3 + 4) / lengthdifferenceratio)
        container A:
            1.5 3.5
        for B:
            1 2
*/
rawdata_fmt sampleRateConversion(const rawdata_fmt &vData, const int iNewsize)
{
    const int iInterpolationSize = ceil((double)vData.size() / iNewsize);
    rawdata_fmt s(iNewsize, 0);

    int iIndex = 0;
    double dAccumulator = 0;
    for(int iPos = 0; iPos < vData.size(); iPos++)
    {
        dAccumulator = 0;
        for(int iIndex = 0; iIndex < iInterpolationSize; iIndex++)
        {
            dAccumulator += vData[iPos];
            iPos++;
        }
        iPos--;
        s.at(iPos / iInterpolationSize) = dAccumulator / (double)iInterpolationSize;
    }

    return s;
}


rawdata_fmt averageResult(const std::vector<rawdata_fmt> &vInput, const int iDivSize)
{
    std::vector<rawdata_fmt> vInterpolatedInput;
    int iShortest;
    for(auto const it : vInput)
    {
        iShortest = (iShortest < it.size()) ? iShortest : it.size();
    }
    rawdata_fmt vRes(iShortest, 0);
    
    for(const auto & it : vInput)
    {
        rawdata_fmt interpolatedData = sampleRateConversion(it, iShortest);
        vInterpolatedInput.push_back(interpolatedData);
    }
    
    for(const auto it : vInterpolatedInput)
    {
        for(int iPos = 0; iPos < iShortest; iPos++)
        {
            vRes.at(iPos) += it.at(iPos);
        }
    }

    for(auto & it : vRes)
    {
        it = (double)( it / (double)iDivSize);
    }

    return vRes;


    // for(int iPos = 0; iPos < vInput.size(); iPos++)
    // {
    //     double dElem = vInput[iPos];
    //     int iIndex = iPos % iMod;
    //     if(dElem == SPLIT_VALUE || (iIndex == 1 && iIndex > iPrev))
    //     {
    //         iPrev = iPos + 1;
    //         if(iIndex > iMod)
    //             iMod = iPos + 1;
    //         continue;
    //     }
    //     vRes[iPos % iMod] += vInput[iPos];
    // }
    // const double dDiv = (double)iDivSize;
    //
    // for(rawdata_fmt::iterator it = vRes.begin();
    //         it != vRes.end(); it++)
    // {
    //     if( (*it) == 0)
    //     {
    //         vRes.erase(it, vRes.end());
    //         break;
    //     }
    //     (*it) /= dDiv;
    // }

    // vRes.pop_back(); // a minus 1 that sneaks in
    return vRes;
}


    
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
    MLOG(severity_type::extensive, "Initial image\n", printImage(use_img));
    MLOG(severity_type::extensive, "Initial guess\n", printImage(guess));
    MLOG(severity_type::extensive, "Initial rho\n", printImage(field));
    imageList_fmt origList, guessList, rhoList;

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

    imageList_fmt accumulator;
    rawdata_fmt vResults;

    std::vector<rawdata_fmt > vAccumulator;

    for(auto it : vSolvers)
    {
        image_fmt result = it->solve(vResults);
        // vResults.push_back(SPLIT_VALUE);
        if(it->isMultipart())
        {
            accumulator.push_back(result);
            vAccumulator.push_back(vResults);
            if(it->isFinal())
            {
                result = joinImage(accumulator, DIVISION_SIZE);
                vResults = averageResult(vAccumulator, DIVISION_SIZE);
                accumulator.clear();
                vAccumulator.clear();
                // vResults = averageResult(vResults, DIVISION_SIZE);
            }
            else
                continue;
        }
        roundValues(result);
        DO_IF_LOGLEVEL(severity_type::extensive)
        {
            std::string sMsg = "Final image(cut)\n" + printImage(result);
            it->log(1, sMsg);
        }

        std::string sSavename = file_IO::SAVE_PATTERN.getSavename(sFilename, it->getLabel(), false);
        file_IO::saveImage(result, sSavename);
        file_IO::writeData(vResults, it->getLabel(), it->getFilename());
        // vResults.clear();
        vResults.erase(vResults.begin(), vResults.end());

        // if(dResolve != 1.0)
        // {
        //     it->alterField(dResolve);
        //     result = it->solve(vResults);
        //     std::string sSavename = file_IO::SAVE_PATTERN.getSavename(sFilename, it->getLabel(), true);
        //     file_IO::saveImage(result, sSavename);
        //     file_IO::writeData(vResults, it->getLabel(), it->getFilename());
        //     vResults.clear();
        // }
    }
}

void calculateAverage(std::string sFilePath)
{
    // std::vector<std::string> files;
    //
    // std::string sReadFolder = DATA_DIR + sFilePath;
    //
    // try
    // {
    //     getFilesInFolder(sReadFolder.c_str(), files);
    // }
    // catch(file_IO::DirNotFound &f)
    // {
    //     std::cout << f.what() << std::endl;
        // MLOG(severity_type::error, f.what());
    // }
    //
    // rawdata_fmt average; // can give undererror
    // int iLineCount = numeric_limits<int>::max();
    // std::string avoid = "average";
    // double dValidFiles = 0;
    // list<int> lLengths;
    // for (std::vector<std::string>::iterator it = files.begin();
    //     it != files.end();
    //     ++it)
    // {
    //     size_t found = (*it).find(avoid);
    //     if(found!=std::string::npos)
    //     {
    //        LOG(severity_type::warning)("average: skipping file", *it);
    //        CLOG(severity_type::warning)("average: skipping file", *it);
    //         continue;
    //     }
    //     dValidFiles++;
    //
    //     ifstream infile;
    //     int iPos = 0;
    //     double dNum;
    //     infile.open(*it);
    //
    //     while(infile >> dNum) // read whole file or stop
    //     {
    //         if(iPos >= average.size())
    //             average.push_back(dNum);
    //         else
    //             average[iPos] += dNum;
    //         iPos++;
    //     }
    //
    //     lLengths.push_back(iPos);
    // }
    //
    // lLengths.sort();
    // for(std::vector<int>::size_type iPos = 0;
    //         iPos < (int)(average.size());
    //         iPos++)
    // {
    //     if(lLengths.size() > 0 &&
    //         iPos >= lLengths.front()) //XXX: or iPos >= ?
    //     {
    //         lLengths.pop_front();
    //         dValidFiles--;
    //     }
    //     average[iPos] /= dValidFiles;
    // }
    //
    // std::string sFilename = string("average") + DATA_EXTENSION;
    // writeToFile(average, sFilename, sFilePath);
}

} /* EndOfNameSpace */
