#define DEFAULT_TOLERANCE 1.0

#include <iostream>
#include <string>
#include <thread>

#include "CImg.h"

#include "loginstance.hpp"
#include "file.hpp"
#include "image2.hpp"
#include "imageedit.hpp"
#include "plot.hpp"

using namespace cimg_library;
using namespace image_psb;
using namespace file_IO;
using namespace logging;


/** Set the verbosity level of a logger; set how much information should be printed.
  @param iLevel the verbosity level
  @param isConsole true means you change verbosity of console log,
                   false means you set the (file) log
*/
void setVerboseLevel(int iLevel, const bool isConsole)
{
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
}

int main(int argc, char **argv) 
{
    std::string logDir = LOG_DIR;
    MLOG(severity_type::info, "Started program");

    std::string sUsageMsg = std::string(argv[0]) + " <name of image file>"
                            "\n\nreport bugs to sildnes@mpi-cbg.de";
    cimg_usage(sUsageMsg.c_str());

    if(argc < 2)
    {
        std::cerr << sUsageMsg << std::endl;
        return EXIT_FAILURE;
    }

    std::string sToleranceHelpStr = "error tolerance (default: " + std::to_string(DEFAULT_TOLERANCE);

    /* Read and set the flags.
       Note that the format is OPT, DEFAULT, HELPTEXT
       */
    const char *dirname = cimg_option("-d", (char*)0, "Input image directory");
    const char *filename = cimg_option("-f", (char*)0, "Input image file");
    const bool compare = cimg_option("-c", false, "Compare original images to solved images");
    const bool average =  cimg_option("-a", false, "average the results for each solver (writes to end of output file");
    const bool gauss = cimg_option("--gauss", false, "use gauss-seidel"),
               jacobi = cimg_option("--jacobi", false, "use jacobi solver"),
               sor = cimg_option("--sor", false, "use successive over-relaxation solver"),
               fft = cimg_option("--fft", false, "use FFT-solver"),
               wavelet = cimg_option("--wavelet", false, "use wavelet solver");
    const bool nosolve = cimg_option("-n", false, "do not compute anything");
    /** If you want a cimg plot */
    const bool plot = cimg_option("-p", false, "visualize the results in a graph");
    const int iVerbosityLevel = cimg_option("-v", 1, "verbosity level: from .. to .. "),
              iFileVerbosityLevel = cimg_option("-x", 1, "written verbosity level");
    const double dTolerance = cimg_option("-t", DEFAULT_TOLERANCE, sToleranceHelpStr.c_str());
    const double dResolve = cimg_option("-r", 1.0, "dResolve the image using a different field");

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
                /* cimg::file_type is NULL for directories */
                const char *fileType = cimg::file_type(0, args[i].c_str());
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
            std::vector<std::string> vFiles = file_IO::getFilesInFolder(sDirname);
            for(auto const it : vFiles)
            {
                image_psb::processImage(it, dTolerance, dResolve,
                                        gauss, jacobi, sor, wavelet, fft);
            }
        }
        if(sFilename.empty() == false)
        {
            image_psb::processImage(sFilename, dTolerance, dResolve,
                                    gauss, jacobi, sor, wavelet, fft);
        }
    }

    std::thread compareLoop;
    std::thread plotLoop;
    if(compare)
    {
        std::string sDir = (sFilename.empty()) ? sDirname : sFilename;
        sDir = file_IO::getFoldername(sDir);
        // image_psb::scanAndAddImage(sDir, DATA_DIR);
        compareLoop = std::thread(image_display::scanAndAddImage, sDir, DATA_DIR);
    }

    const bool doAverage = (nosolve) ? false : average;
    image_fmt img = readData(doAverage,plot);

    if(plot)
    {
        cimg_library::CImgDisplay disp = plot::plot(img);
        plotLoop = std::thread(image_psb::renderImage, disp);
    }

    if(plotLoop.joinable())
        plotLoop.join();
    if(compareLoop.joinable())
        compareLoop.join();

    // MLOG(severity_type::info, "Program exited successfully\n");
    // MLOG(severity_type::info, std::flush);
    // MFLUSH;
    return EXIT_SUCCESS;
}
