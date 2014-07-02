#ifndef _MAIN_CPP
#define _MAIN_CPP   1

#define DATA_DIR "./output/"
#define DATA_EXTENSION ".dat"
#define PRECISION 20

#define LOG_DIR "./log/"
#define LOG(x) (log_inst.print< x >)
#define CLOG(x) (log_inst_std.print< x >)
#define DO_IF_LOGLEVEL(x) if(x >= log_inst.getLevel()) 
#define SETLEVEL(x) log_inst.setLevel(x)
#define CSETLEVEL(x) log_inst_std.setLevel(x)

#include <fstream>
#include <iostream>
#include <iomanip>
#include <limits>
#include <map>
#include <sstream> 
#include <string>
#include <thread>

#include <getopt.h>

#include "./logger.hpp"
using namespace logging;
static logging::logger< logging::file_log_policy > log_inst( LOG_DIR "/execution.log" );
static logging::logger< logging::file_log_policy > log_inst_std( "/dev/fd/0");

#include "CImg.h"

#include "./file.hpp"
#include "./image2.cpp"
#include "include/iterative_solvers.hpp"
#include "plot.cpp"
// #include "solvers/FFT.cpp"

#define no_argument 0
#define required_argument 1 
#define optional_argument 2


using namespace cimg_library;
using namespace pe_solver;
using namespace image_psb;
using namespace file_IO;
using namespace plot;


void usage()
{
    cout << "Usage: main <folder>" << endl;

    printf("\t -%c, %s\t%s\n", 'a', "--average", "compute average errors for solvers");
    printf("\t -%c, %s\t%s\n", 'c', "--compare", "visual comparison of solved images vs original image");
    printf("\t -%c, %s\t%s\n", 'd', "--directory", "directory with images");
    printf("\t -%c, %s\t%s\n", 'g', "--gauss", "perform gauss-seidel iteration");
    printf("\t -%c, %s\t%s\n", 'h', "--help", "view this text");
    printf("\t -%c, %s\t%s\n", 'j', "--jacobi", "perform jacobi iteration");
    printf("\t -%c, %s\t%s\n", 'n', "--nosolve", "do not invoke any solvers on the image");
    printf("\t -%c, %s\t%s\n", 'p', "--plot", "generate plots for graphs");
    printf("\t -%c, %s\t%s\n", 's', "--sor", "perform sor iteration");
    printf("\t -%c, %s\t%s\n", 't', "--tolerance", "set the error tolerance between iterations");
    printf("\n");

    exit(EXIT_FAILURE);
}


int main(int argc, char *argv[]) 
{
    string logDir = LOG_DIR;
    mkdirp(logDir.c_str());
    LOG(severity_type::info)("Started program");

    const struct option longopts[] =
    {
        {"average"          , no_argument       , 0, 'a'},
        {"compare"          , no_argument       , 0, 'c'},
        {"directory"        , required_argument , 0, 'd'},
        {"fft"              , no_argument       , 0, 'f'},
        {"gauss"            , no_argument       , 0, 'g'},
        {"help"             , no_argument       , 0, 'h'},
        {"computeline"      , no_argument       , 0, 'l'},
        {"jacobi"           , no_argument       , 0, 'j'},
        {"nosolve"          , no_argument       , 0, 'n'},
        {"plot"             , no_argument       , 0, 'p'},
        {"resolve"          , required_argument , 0, 'r'},
        {"tolerance"        , no_argument       , 0, 't'},
        {"sor"              , no_argument       , 0, 's'},
        {"verbose"          , optional_argument , 0, 'v'},
        {"fileverbose"      , optional_argument , 0, 'x'},
        // {"values-histogram" , no_argument       , 0, 'v'},
        {0,0,0,0},
    };

    int index;
    int iarg=0;
    extern char *optarg;
    
    log_inst_std.setHeader(false);
    int a = 0, c = 0, d = 0, f = 0, g = 0, j = 0, l = 0, n = 0, p = 0, r = 0, s = 0,
        v = 0, x = 0;
    double dScalar, dTolerance = 0.5;
    char *folder;
    function_container vFuncContainer;
    SolverMeta smG(iterate_gauss, string("gauss/"));
    SolverMeta smJ(iterate_jacobi, string("jacobi/"));
    SolverMeta smS(iterate_sor, string("sor/"));
    ImageSolver imageSolver;

    if(argc == 2 && ! (strcmp(argv[1], "-p") || strcmp(argv[1], "-n") 
                       || strcmp(argv[1], "-h")))
    {
        cout << "Assuming \"-d " << string(argv[1]) << " --gauss --plot" << endl;
        vFuncContainer.push_back(smG);
        imageSolver.addFolder(string(argv[1]));
        imageSolver.solve(vFuncContainer);
        plot::plot();
        exit(EXIT_SUCCESS);
    }


    while(iarg != -1)
    {
        iarg = getopt_long(argc, argv, "acd:fgjlhnpr:st:v:x:", longopts, &index);

        switch (iarg)
        {
            case 'a':
                a++;
                break;

            case 'c':
                c++;
                break;

            case 'd':
                d++;
                folder = optarg;
                break;

            case 'f':
                f++;
                break;

            case 'g':
                vFuncContainer.push_back(smG);
                break;

            case 'j':
                vFuncContainer.push_back(smJ);
                break;

            case 'h':
                usage();
                break;

            case 'l':
                l++;
                break;
            case 'n':
                n++;
                break;

            case 'r':
                r++;
                dScalar = atof(optarg);
                break;

            case 's':
                vFuncContainer.push_back(smS);
                break;

            case 't':
                dTolerance = atof(optarg);
                break;

            case 'p':
                p++;
                break;
            case 'v':
                v = atoi(optarg);
                if(v >= severity_type::no_output && v <= severity_type::error)
                    CSETLEVEL(v);
                else
                    cout << "Error: stdout verbose level out of range" << endl;
                break;
            case 'x':
                x = atoi(optarg);
                if(x >= severity_type::no_output && x <= severity_type::error)
                    SETLEVEL(x);
                else
                    cout << "Error: file verbose level out of range" << endl;
                break;
        }
    } 

    if(d) {
        if(vFuncContainer.size() < 1 && !n)
            CLOG(severity_type::warning)("no iterators chosen");
            LOG(severity_type::warning)("no iterators chosen");
        imageSolver.addFolder(folder);
        if(!n)
            imageSolver.solve(vFuncContainer, l>0, dTolerance);
    }
    else {
        CLOG(severity_type::warning)("no media folder given");
        LOG(severity_type::warning)("no media folder given");
    }

    if(r)
    {
        ImageSolver imageSolver2;
        imageSolver2.setVerbosity(v);
        for (function_container::iterator it = vFuncContainer.begin();
            it != vFuncContainer.end();
            ++it)
        {
            string sPath = DATA_DIR + (*it).sPath + "/image";
            imageSolver2.addFolder(sPath);
        }

        imageSolver2.solve(vFuncContainer, l>0, dTolerance, "re", "re", dScalar);
    }

    if(a)
    {
        for (function_container::iterator it = vFuncContainer.begin();
            it != vFuncContainer.end();
            ++it)
        {
            calculateAverage((*it).sPath);
        }
    }

    std::thread histLoop;
    // if(v)
    // {
    //     imageSolver.clearFolders();
    //     imageSolver.addFolder(folder);
    //     imageList_fmt histogram = imageSolver.histogram(folder, vFuncContainer);
        // CImgDisplay hist_disp(histogram, "histogram", 0,  false,true);
        // cout << "SIZE " << histogram.size() << endl;
        //histLoop = thread(display_histogram, histogram);
        // cout << histogram.width() << endl;
        // cout << histogram.height() << endl;
        // histLoop = thread(renderImage, hist_disp);
    // }



    std::thread plotLoop;
    if(p) 
    { 
        plot::plot();
        image_fmt imgPlot("graph.png");
        CImgDisplay plot_disp(imgPlot, "graph.png : graph for all images in folder",0, false, true);	
        plotLoop = thread(renderImage, plot_disp);
    }


    if(c && d)
    {
        imageSolver.clearFolders();
        imageSolver.addFolder(folder, "when trying to show rendered images (-c flag)");
        imageSolver.renderImages(folder, vFuncContainer);
    }

    if(plotLoop.joinable())
        plotLoop.join();

    if(histLoop.joinable())
        histLoop.join();

    LOG(severity_type::info)("Program exited successfully");
    return 0;
}

#endif /* _MAIN.cpp */
