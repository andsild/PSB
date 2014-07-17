#define DEFAULT_TOLERANCE 1.0

#include <fstream>
#include <iostream>
#include <iomanip>
#include <map>
#include <sstream> 
#include <string>
#include <thread>

#include "CImg.h"

#include "iterative_solvers.hpp"
#include "solver.hpp"
#include "wavelet.hpp"
#include "fft.hpp"
#include "loginstance.hpp"
#include "file.hpp"
#include "image2.hpp"
#include "plot.hpp"

using namespace cimg_library;
using namespace image_psb;
using namespace file_IO;
using namespace plot;
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
    LOG(severity_type::info)("Started program");

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
    const bool gauss = cimg_option("--gauss", false, "use gauss-seidel"),
               jacobi = cimg_option("--jacobi", false, "use jacobi solver"),
               sor = cimg_option("--sor", false, "use successive over-relaxation solver"),
               fft = cimg_option("--fft", false, "use FFT-solver"),
               wavelet = cimg_option("--wavelet", false, "use wavelet solver");
    const bool nosolve = cimg_option("-n", false, "do not comput anything");
    const int iVerbosityLevel = cimg_option("-v", 1, "verbosity level: from .. to .. "),
              iFileVerbosityLevel = cimg_option("-x", 1, "written verbosity level");
    const double dTolerance = cimg_option("-t", DEFAULT_TOLERANCE, sToleranceHelpStr.c_str());
    const double resolve = cimg_option("-n", 1.0, "resolve the image using a different field");

    std::string sFilename = (filename) ? std::string(filename) : std::string(),
                sDirname = (dirname) ? std::string(dirname) : std::string();

    setVerboseLevel(iVerbosityLevel, false);
    setVerboseLevel(iFileVerbosityLevel, true);

    /* Begin processing image */
    // readImages
    // process, compute rho only once
    // then do a long check:
    // if(rho)
    // Solver rho(....)
    // if(gauss)
    // solver gauss

    if(sFilename.empty() && sDirname.empty())
    {
        std::vector<std::string> args(argv, argv+argc);
        for (size_t i = 1; i < args.size(); ++i)
        {
            if (args[i].at(0) != '-')
            {
                sFilename = args[i];
                break;
            }
        }
        if(sFilename.empty()) {
        std::cerr << sUsageMsg << std::endl;
        return EXIT_FAILURE; }
    }

    image_fmt use_img;
    if(!image_psb::readImage(use_img, sFilename))
    {
        std::cerr << "Error:: could not load image: " << sFilename << std::endl;
        return EXIT_FAILURE;
    }
    image_psb::toGrayScale(use_img);
    image_fmt field = image_psb::makeRho(use_img),
              guess = image_psb::makeInitialGuess(use_img, true);
    
    if(sor)
    {
        std::cout << sFilename << std::endl;
        std::string *test = new std::string("daw");
        solver::IterativeSolver *sSor = new solver::IterativeSolver(use_img, field, guess,
                                    solver::iterate_sor2, dTolerance, *test);
        image_fmt result = sSor->solve();
        roundValues(result);
        if(resolve != 1.0)
        {
            // sor.alterField(resolve);
            // result = sor.solve();
        }
        delete sSor;
    }


    // if(d) {
    //     if(vFuncContainer.size() < 1 && !n)
    //     {
    //         CLOG(severity_type::warning)("no iterators chosen");
    //         LOG(severity_type::warning)("no iterators chosen");
    //     }
    //     imageSolver.addFolder(folder);
    //     if(!n)
    //         imageSolver.solve(vFuncContainer, l>0, dTolerance);
    // }
    // else {
    //     CLOG(severity_type::warning)("no media folder given");
    //     LOG(severity_type::warning)("no media folder given");
    // }
    //
    // if(f)
    // {
    //     // image_fmt img("../nice_example/increasing.png");
    //     image_fmt img("../media_resolve/test.png");
    //     toGrayScale(img);
    //     fft::FFT2D(img);
    // }
    //
    // if(w)
    // {
    //     // image_fmt img("../media_resolve/test.png");
    //     // image_fmt img("../nice_example/3gradientAnother.png");
    //     image_fmt img("../nice_example/all_increasing.png");
    //     toGrayScale(img);
    //     wavelet::pyconv(img);
    // }
    //
    // if(r)
    // {
    //     ImageSolver imageSolver2;
    //     for (function_container::iterator it = vFuncContainer.begin();
    //         it != vFuncContainer.end();
    //         ++it)
    //     {
    //         std::string sPath = "output/" + (*it).sPath + "/image/";
    //         // imageSolver2.addFolder(sPath);
    //         imageSolver2.addFolder(folder);
    //         break;
    //     }
    //
    //     if(!n)
    //         imageSolver2.solve(vFuncContainer, l>0, dTolerance, "re", "re", dScalar);
    // }
    //
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
    //     // std::cout << "SIZE " << histogram.size() << std::endl;
    //     //histLoop = thread(display_histogram, histogram);
    //     // std::cout << histogram.width() << std::endl;
    //     // std::cout << histogram.height() << std::endl;
    //     // histLoop = thread(renderImage, hist_disp);
    // // }
    //
    //
    //
    // std::thread plotLoop;
    // if(p) 
    // { 
    //     plot::plot();
    //     image_fmt imgPlot("graph.png");
    //     CImgDisplay plot_disp(imgPlot, "graph.png : graph for all images in folder",0, false, true);	
    //     plotLoop = std::thread(renderImage, plot_disp);
    // }
    //
    //
    // if(c && d)
    // {
    //     imageSolver.clearFolders();
    //     imageSolver.addFolder(folder, "when trying to show rendered images (-c flag)");
    //     const char *text = "NOT";
    //     if(r) text = "re";
    //     imageSolver.renderImages(folder, vFuncContainer, "image/", text);
    // }
    //
    // if(plotLoop.joinable())
    //     plotLoop.join();
    //
    // if(histLoop.joinable())
    //     histLoop.join();
    //
    // LOG(severity_type::info)("Program exited successfully");
    return EXIT_SUCCESS;
}
