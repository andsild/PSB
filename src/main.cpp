#define DEFAULT_TOLERANCE 1.0

#include <iostream>
#include <string>
#include <thread>

#include "CImg.h"

#include "loginstance.hpp"
#include "file.hpp"
#include "image2.hpp"
#include "plot.hpp"

using namespace cimg_library;
using namespace image_psb;
using namespace file_IO;
// using namespace plot;
using namespace logging;


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
    mkdirp(logDir.c_str());
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

    const char *dirname = cimg_option("-d", (char*)0, "Input image directory");
    const char *filename = cimg_option("-f", (char*)0, "Input image file");
    const bool compare = cimg_option("-c", false, "Compare original images to solved images");
    const bool gauss = cimg_option("--gauss", false, "use gauss-seidel"),
               jacobi = cimg_option("--jacobi", false, "use jacobi solver"),
               sor = cimg_option("--sor", false, "use successive over-relaxation solver"),
               fft = cimg_option("--fft", false, "use FFT-solver"),
               wavelet = cimg_option("--wavelet", false, "use wavelet solver");
    const bool nosolve = cimg_option("-n", false, "do not comput anything");
    const bool plot = cimg_option("-p", false, "visualize the results in a graph");
    const int iVerbosityLevel = cimg_option("-v", 1, "verbosity level: from .. to .. "),
              iFileVerbosityLevel = cimg_option("-x", 1, "written verbosity level");
    const double dTolerance = cimg_option("-t", DEFAULT_TOLERANCE, sToleranceHelpStr.c_str());
    const double dResolve = cimg_option("-r", 1.0, "dResolve the image using a different field");

    std::string sFilename = (filename) ? std::string(filename) : std::string(),
                sDirname = (dirname) ? std::string(dirname) : std::string();

    setVerboseLevel(iVerbosityLevel, true);
    setVerboseLevel(iFileVerbosityLevel, false);

    if(sFilename.empty() && sDirname.empty())
    {
        std::vector<std::string> args(argv, argv+argc);
        for (size_t i = 1; i < args.size(); ++i)
        {
            if (args[i].at(0) != '-')
            {
                const char *fileType = cimg::file_type(0, args[i].c_str());
                if(!fileType)
                    sDirname = args[i];
                else
                    sFilename = args[i];
                break;
            }
        }
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

    if(compare)
    {
        std::string sDir = (sFilename.empty()) ? sDirname : sFilename;
        sDir = file_IO::getFoldername(sDir);
        image_psb::scanAndAddImage(sDir, DATA_DIR);
    }

    if(plot)
    {
    }

    // if(a)
    // {
    //     for (function_container::iterator it = vFuncContainer.begin();
    //         it != vFuncContainer.end();
    //         ++it)
    //     {
    //         calculateAverage((*it).sPath);
    //     }
    // }
    //
    // std::thread histLoop;
    // // if(v)
    // // {
    // //     imageSolver.clearFolders();
    // //     imageSolver.addFolder(folder);
    // //     imageList_fmt histogram = imageSolver.histogram(folder, vFuncContainer);
    //     // CImgDisplay hist_disp(histogram, "histogram", 0,  false,true);
    //     //histLoop = thread(display_histogram, histogram);
    //     // std::cout << histogram.width() << std::endl;
    //     // histLoop = thread(renderImage, hist_disp);
    // // }
    //
    //
    //
    std::thread plotLoop;
    // if(p) 
    // { 
    //     plot::plot();
    //     image_fmt imgPlot("graph.png");
    //     CImgDisplay plot_disp(imgPlot, "graph.png : graph for all images in folder",0, false, true);	
    //     plotLoop = std::thread(renderImage, plot_disp);
    // }
    //
    //
    // if(plotLoop.joinable())
    //     plotLoop.join();
    
    // if(histLoop.joinable())
    //     histLoop.join();
    //
    // LOG(severity_type::info)("Program exited successfully");
    return EXIT_SUCCESS;
}
