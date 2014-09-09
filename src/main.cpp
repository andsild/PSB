#define DEFAULT_TOLERANCE 1.0

#include <iostream>
#include <string>
#include <thread>

#include "CImg.h"

#include "loginstance.hpp"
#include "file.hpp"
#include "image2.hpp"
#include "imageQueue.hpp"
#include "image_types.hpp"
#include "imageedit.hpp"
#include "plot.hpp"

using namespace cimg_library;
using namespace image_util;
using namespace file_IO;
using namespace logging;


/** Set the verbosity level of a logger; set how much information should be printed.
  @param iLevel the verbosity level
  @param isConsole true means you change verbosity of console log,
                   false means you set the (file) log
*/
void setVerboseLevel(int iLevel, const bool isConsole)
{
#ifdef LOGGING
    if(iLevel >= severity_type::no_output && iLevel <= severity_type::debug)
    {
        if(iLevel == severity_type::debug)
            std::cout << "WARNING: debug mode will slow down the"
                            " program by * a lot *" << std::endl;
        if(isConsole)
            CSETLEVEL(iLevel);
        else SETLEVEL(iLevel);
    }
    else
        std::cerr << "Error: stdout verbose level out of range" << std::endl;
#endif
}

int main(int argc, char **argv) 
{
    MLOG(severity_type::info, "Started program");

    std::string sUsageMsg = std::string(argv[0]) + " <name of image file or directory> <solvers>"
                            "\n\nreport bugs to sildnes@mpi-cbg.de";
    cimg_usage(sUsageMsg.c_str());

    if(argc < 2)
    {
        std::cerr << sUsageMsg << std::endl;
        return EXIT_FAILURE;
    }

    std::string sToleranceHelpStr = "error tolerance (default: " + std::to_string(DEFAULT_TOLERANCE) + ")";

    /* Read and set the flags.
       Note that the format is OPT, DEFAULT, HELPTEXT
       */
    const bool gauss = cimg_option("--gauss", false, "use gauss-seidel"),
               jacobi = cimg_option("--jacobi", false, "use jacobi solver"),
               sor = cimg_option("--sor", false, "use successive over-relaxation solver"),
               fft_dst = cimg_option("--dst", false, "use discrete sine solver"),
               fft_dct = cimg_option("--dct", false, "use discrete cosine solver"),
               wavelet_5x5 = cimg_option("--wavelet5", false, "use wavelet solver with a 5x5 kernel"),
               wavelet_7x7 = cimg_option("--wavelet7", false, "use wavelet solver with a 7x7 kernel"),
               multi_wavelet = cimg_option("--multi-wavelet", false, "use multi-wavelet solver");
    const char *dirname = cimg_option("-d", (char*)0, "Input image directory");
    const char *filename = cimg_option("-f", (char*)0, "Input image file");
    const bool compare = cimg_option("-c", false, "Compare original images to solved images");
    const bool average =  cimg_option("-a", false, "average the results for each solver (writes to end of output file");
    const bool nosolve = cimg_option("--nosolve", false, "do not compute anything");
    const double dNoise = cimg_option("-n", (0.0), "add <percent> noise to the image before solving");
    /** If you want a cimg plot */
    const bool plot = cimg_option("-p", false, "visualize the results in a graph");
    const int iVerbosityLevel = cimg_option("-v", 1, "verbosity level: from .. to .. "),
              iFileVerbosityLevel = cimg_option("-x", 1, "written verbosity level");
    const double dTolerance = cimg_option("-t", DEFAULT_TOLERANCE, sToleranceHelpStr.c_str());
    const double resolve = cimg_option("-r", 1.0, "resolve the image using a modified field, multiplied by a scalar (used for testing)");

    std::string sFilename = (filename) ? std::string(filename) : std::string(),
                sDirname = (dirname) ? std::string(dirname) : std::string();

    setVerboseLevel(iVerbosityLevel, true);
    setVerboseLevel(iFileVerbosityLevel, false);

    /* If the user has not set file or directory using flags.. */
    if(sFilename.empty() && sDirname.empty())
    {
        std::vector<std::string> args(argv, argv+argc);
        /* ... iterate over all argv */
        for (size_t i = 1; i < args.size(); ++i)
        {
            /* ... we found argv which is not a parameter */
            if (args[i].at(0) != '-')
            {
                cimg::exception_mode(0);
                const char *fileType;
                try{
                fileType = cimg::file_type(0, args[i].c_str());
                }catch(CImgIOException){}
                if(!fileType)
                    sDirname = args[i];
                else
                    sFilename = args[i];
                break;
            }
        }
        /* If nothing was found, display usage message */
        if(sFilename.empty() && sDirname.empty()) {
        std::cerr << sUsageMsg << std::endl;
        return EXIT_FAILURE; }
    }

    if(nosolve == false)
    {
        if(sDirname.empty() == false)
        {
            std::vector<std::string> vFiles;
            try{
                vFiles = file_IO::getFilesInFolder(sDirname);
            }catch(file_IO::DirNotFound dnf) { std::cerr << "Failed to open " << sDirname << std::endl << " exiting..." << std::endl; exit(EXIT_FAILURE);}
            for(auto const it : vFiles)
            {
             processImage(it, dNoise, dTolerance, resolve,
                                        gauss, jacobi, sor,
                                        fft_dst, fft_dct,
                                        wavelet_5x5, wavelet_7x7, multi_wavelet);
            }
        }
        if(sFilename.empty() == false)
        {
            processImage(sFilename, dNoise, dTolerance, resolve,
                                    gauss, jacobi, sor,
                                    fft_dst, fft_dct,
                                    wavelet_5x5, wavelet_7x7, multi_wavelet);
        }
    }

    std::thread compareLoop;
    std::thread plotLoop;
    if(compare)
    {
        std::string sDir = (sFilename.empty()) ? sDirname : sFilename;
        sDir = file_IO::getFoldername(sDir);
        // image_util::scanAndAddImage(sDir, DATA_DIR);
        compareLoop = std::thread(image_display::scanAndAddImage, sDir, DATA_DIR);
    }

    const bool doAverage = (nosolve) ? false : average;
    image_fmt img;
    if(doAverage || plot)
        img = readData(doAverage,plot);

    if(plot)
    {
        cimg_library::CImgDisplay disp = plot::plot(img);
        plotLoop = std::thread(image_util::renderImage, disp);
    }

    if(plotLoop.joinable())
        plotLoop.join();
    if(compareLoop.joinable())
        compareLoop.join();

    MLOG(severity_type::info, "Program exited successfully");
    return EXIT_SUCCESS;
}
