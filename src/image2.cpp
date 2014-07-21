#include "image2.hpp"

#include <iostream>
#include <iomanip>
#include <list>
#include <map>
#include <sstream>
#include <string>

#include "CImg.h"

#include "imageedit.hpp"
#include "loginstance.hpp"
#include "loadingbar.hpp"
#include "iterative_solvers.hpp"
#include "file.hpp"
#include "plot.hpp"

using namespace cimg_library;
using namespace file_IO;
using namespace plot;
using namespace logging;
using namespace loadbar;
using namespace solver;
using namespace image_display;

namespace image_psb
{

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


image_fmt makeInitialGuess(const image_fmt &image, bool bExtractBordes = true)
{
    if(image.size() < 1)
    {
        std::string sMsg = "Image was not initialized before creating border"
                           " [[ width: " + std::to_string(image.width()) 
                           + " height: " + std::to_string(image.height()) + " ]]";
        throw ImageException(sMsg.c_str());
    }

    image_fmt U;

    int BORDER_SIZE = 1;
    int DEFAULT_GUESS_VAL = 0;
    U.assign(image, "xyz", DEFAULT_GUESS_VAL);

    if(bExtractBordes == true)
    {
        cimg_for_borderXY(image,x,y,BORDER_SIZE)
        {
            U(x,y) = image(x,y);
        }
    }

    return U;
}



template <class T> class ImageProcess
{
    private:
    typedef std::map<iterative_function,std::vector<std::string> > mapfuncres;
        image_fmt image;
        const char *fileName;
        mapfuncres mapFuncRes;
        std::vector<std::string> vOutput;

        image_fmt sRGBtoGrayscale()
        {
            return this->image.get_norm();
        }


    public:
        image_fmt rho;
        image_fmt U;
        image_fmt &getGuess() { return this->U; }
        image_fmt &getImage () { return this->image; }
        image_fmt &getRho() { return this->rho; }


        double dMaxErr;
        ImageProcess(image_fmt image, const char *fileName, double dMaxErr)
                : fileName(fileName), dMaxErr(dMaxErr),
                  image(image)

        {
        }

        void clearImages()
        {
            this->U.clear();
            this->vOutput.clear();
        }
        void solve(iterative_function func,logging::Logger< logging::FileLogPolicy > &logInstance)
       {
            this->vOutput =  iterative_solve(func,
                                       this->image, this->U, this->rho,
                                       this->dMaxErr, this->image.width(),
                                       logInstance);

            if(this->vOutput.size() < 1)
            {
                throw ImageException("Solver had no results from function");
            }
       }

        void writeResultToFile(std::string fileDir)
        {
            std::string sFilename = std::string(this->fileName);
            try{
            writeToFile(this->vOutput, sFilename, fileDir);
            }
            catch(...)
            {
                throw ImageException("could not write result file to " + fileDir);
            }
        }

        void writeImageToFile(const char *fileDir)
        {
            std::string sFileName = std::string(this->fileName);
            trimLeadingFileName(sFileName);
            std::string sFilename = std::string(fileDir) + "/" + sFileName;
            cimg::exception_mode(0);
            try
            {
                mkdirp(fileDir);
                this->U.save_ascii(sFilename.c_str());
            }
            catch(CImgIOException &cioe)
            {
                CLOG(severity_type::error)(cioe.what());
                LOG(severity_type::error)(cioe.what());
            }

        }
};


ImageSolver::ImageSolver() {}
void ImageSolver::addFolder(std::string sFolder, const char *errMsg)
{
    try{
        getFilesInFolder(sFolder, this->filenames);
    }
    catch(const file_IO::DirNotFound& f)
    {
        CLOG(severity_type::error)(f.what());
        LOG(severity_type::error)(f.what());
        exit(EXIT_FAILURE);
    }
}

bool loadImage(const char *fileDest, image_fmt &image)
{
    cimg::exception_mode(0);
    try {
        image.load(fileDest);
        LOG(severity_type::debug)("Loaded image ", fileDest,
                                  "to code format at ", &image);
        CLOG(severity_type::debug)("Loaded image ", fileDest,
                                  "to code format at ", &image);
        toGrayScale(image);
        LOG(severity_type::debug)("Converted ", fileDest, " to grayscale");
        CLOG(severity_type::debug)("Converted ", fileDest, " to grayscale");
    }
    catch(CImgIOException cioe)
    {
        LOG(severity_type::error)(cioe.what());
        CLOG(severity_type::error)(cioe.what());
        return false;
    }

    return true;
}

void ImageSolver::clearFolders()
{
    this->filenames.clear();
}

void scanAndAddImage(std::string sRootdir, std::string sSolverdir)
{
    std::vector<std::string> vFilenames = file_IO::getFilesInFolder2(sRootdir),
                        vSolvedNames = file_IO::getFilesInFolder2(sSolverdir);
    ImageDisplay id;

    std::cout << sRootdir  << " " << sSolverdir << std::endl;

    for(auto const it : vFilenames)
    {
        id.addMainImage(it);
    }
    for(auto const it : vSolvedNames)
    {
        bool isResolved = false;
        std::string _, sLabel, sFilename;
        file_IO::SAVE_PATTERN.getNames(it, _, sLabel, sFilename, isResolved);
        id.addResolvedImage2(it, sFilename, isResolved);
    }
    id.show();
    id.loop();
}


imageList_fmt ImageSolver::histogram(std::string sDir, function_container vIf)
{
    // for (function_container::iterator subIt = vIf.begin();
    //     subIt != vIf.end();
    //     ++subIt)
    // {
    image_fmt bigImg;
    imageList_fmt images;
    cimg::exception_mode(0);

    for(std::vector<std::string>::iterator it = filenames.begin();
        it != filenames.end();
        it++)
    {
        image_fmt image;
        if(!loadImage((*it).c_str(), image))
        {
            continue;
        }
        bigImg.append(image, 'x');
    }
    images.push_back(bigImg.histogram(256));


    for (function_container::iterator it = vIf.begin();
        it != vIf.end();
        ++it)
    {
        this->filenames.clear();
        std::string sImageDir = DATA_DIR + (*it).sPath + std::string("image/");
        this->addFolder(sImageDir);
        image_fmt appender;

        for(std::vector<std::string>::iterator subIt = filenames.begin();
            subIt != filenames.end();
            subIt++)
        {
            image_fmt image;
            if(!loadImage((*subIt).c_str(), image))
            {
                continue;
            }
            appender.append(image, 'x');
        }
        images.push_back(appender.histogram(256));
    }

    return images;
}

void ImageSolver::solve(function_container vIf, bool bComputeLines,
            double dTolerance, const char *cImagePath, const char *cResultPath
           , double dScalar)
{
    if(this->filenames.size() < 1
    || vIf.size() < 1) {
        LOG(severity_type::error)("Solve called without files (or functions?)");
        CLOG(severity_type::error)("Solve called without files (or functions?)");
        return;
    }

    int iTotalIterations;
    double dProgress = 0;
    double dStepSize;

    iTotalIterations = this->filenames.size() * vIf.size();
    dStepSize = ((double)100 / iTotalIterations);
    int iTimeRemaining = filenames.size() * vIf.size();
    loadBar.initialize(iTotalIterations);
    for(std::vector<std::string>::iterator it = filenames.begin();
        it != filenames.end();
        it++)
    {
        cimg::exception_mode(0);
        image_fmt image;
        if(!loadImage((*it).c_str(), image))
        {
            loadBar.increaseProgress(vIf.size() - 1);
            std::cout << loadBar << std::endl;
            continue;
        }

        ImageProcess<double> ipImage(image, (*it).c_str(), dTolerance);
        ipImage.rho = makeRho(ipImage.getImage());
        if(dScalar != 1.0)
        {
            ipImage.getRho() *= dScalar;
        }

        bool BORDERS=true;

        for (function_container::iterator subIt = vIf.begin();
            subIt != vIf.end();
            ++subIt)
        {
            std::string sRedo = "";
            if(dScalar != 1.0)
                sRedo = "re";

            std::string sImageDir = DATA_DIR + (*subIt).sPath + std::string(cImagePath);
            std::string sLogPath = LOG_DIR + (*subIt).sPath;
            std::string sLogFile = sLogPath + (*it);
            trimTrailingFilename(sLogFile);
            trimLeadingFileName(sLogFile);
            sLogFile = sRedo + sLogFile + ".log";
            mkdirp(sLogPath.c_str());


            logging::Logger< logging::FileLogPolicy > logInstance(sLogPath + sLogFile);
            logInstance.setLevel(log_inst.getLevel());

            ipImage.U = makeInitialGuess(ipImage.getImage(), BORDERS);
            if(logInstance.getLevel() >= severity_type::extensive)
            {
                (logInstance.print<severity_type::extensive>)("Initial image: ", (*it).c_str(), "\n", printImage(ipImage.getImage()));
                (logInstance.print<severity_type::extensive>)("Initial guess\n", printImage(ipImage.getGuess()));
                (logInstance.print<severity_type::extensive>)("Initial rho\n", printImage(ipImage.getRho()));
            }
            if(CGETLEVEL >= severity_type::extensive)
            {
                CLOG(severity_type::extensive)("Initial image\n", printImage(ipImage.getImage()));
                CLOG(severity_type::extensive)("Initial guess\n", printImage(ipImage.getGuess()));
                CLOG(severity_type::extensive)("Initial rho\n", printImage(ipImage.getRho()));
            }
            (logInstance.print<severity_type::info>)("Beginning solver: ", (*subIt).sPath);
            CLOG(severity_type::info)("Beginning solver: ", (*subIt).sPath);
            CLOG(severity_type::info)("Beginning solver: ", (*subIt).sPath, " for image ", (*it));
            LOG(severity_type::info)("Beginning solver: ", (*subIt).sPath, " for image ", (*it));

            try{

                ipImage.solve((*subIt).func, logInstance);
                CLOG(severity_type::extensive)("Finished image: \n", printImage(ipImage.getGuess()));
                (logInstance.print<severity_type::extensive>)("Finished image: \n", printImage(ipImage.getGuess()));
                std::cout << loadBar << std::endl;

                ipImage.writeResultToFile(std::string(cResultPath) + (*subIt).sPath);

                if(dScalar == 1.0)
                {
                    roundValues(ipImage.getGuess());
                }
                CLOG(severity_type::extensive)("Finished image (rounded and cut): \n", printImage(ipImage.getGuess()));
                (logInstance.print<severity_type::extensive>)("Finished image(rounded and cut): \n", printImage(ipImage.getGuess()));
                ipImage.writeImageToFile(sImageDir.c_str());
                ipImage.clearImages();
            }
            catch(ImageException &ie)
            {
                LOG(severity_type::warning)(ie.what());
                CLOG(severity_type::warning)(ie.what());
            }
        }

    }
    std::cout << loadBar << std::endl;// if last image has errors, this might be necessary
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
    //     LOG(severity_type::error)(f.what());
    //     CLOG(severity_type::error)(f.what());
    // }
    //
    // std::vector<double> average; // can give undererror
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
