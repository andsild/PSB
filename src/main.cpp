#ifndef _MAIN_CPP
#define _MAIN_CPP   1

#define DATA_DIR "./output/"
#define DATA_EXTENSION ".dat"
#define PRECISION 20

#include <fstream>
#include <iostream>
#include <iomanip>
#include <limits>
#include <map>
#include <sstream> 
#include <string>
#include <thread>

#include <dirent.h>
#include <getopt.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "CImg.h"
#include "./image2.cpp"
#include "./file.cpp"
#include "solvers/iterative_solvers.cpp"
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

    printf("\t -%c, %s\t%s\n", 'f', "--folder", "folder with images");
    printf("\t -%c, %s\t%s\n", 'g', "--gauss", "perform gauss-seidel iteration");
    printf("\t -%c, %s\t%s\n", 'h', "--help", "view this text");
    printf("\t -%c, %s\t%s\n", 'j', "--jacobi", "perform jacobi iteration");
    printf("\t -%c, %s\t%s\n", 'p', "--plot", "generate plots for graphs");
    printf("\t -%c, %s\t%s\n", 's', "--sor", "perform sor iteration");
    printf("\n");

    exit(EXIT_FAILURE);
}


int main(int argc, char *argv[]) 
{
    const struct option longopts[] =
    {
        {"average", no_argument,  0, 'a'},
        {"compare", no_argument,  0, 'c'},
        {"folder",     required_argument,  0, 'f'},
        {"gauss",   no_argument,        0, 'g'},
        {"help",   no_argument,        0, 'h'},
        {"computeline",   no_argument,        0, 'l'},
        {"jacobi",   no_argument,        0, 'j'},
        {"nosolve",   no_argument,        0, 'n'},
        {"plot",   no_argument,        0, 'p'},
        {"resolve",   required_argument,        0, 'r'},
        {"sor",   no_argument,        0, 's'},
        {"values-histogram",   no_argument,        0, 'v'},
        {0,0,0,0},
    };

    int index;
    int iarg=0;
    extern char *optarg;
    
    int a = 0, c = 0, f = 0, g = 0, j = 0, l = 0, n = 0, p = 0, r = 0, s = 0,
        v = 0;
    double dScalar;
    char *folder;
    function_container vFuncContainer;
    SolverMeta smG(iterate_gauss, "gauss/", "images/");
    SolverMeta smJ(iterate_jacobi, "jacobi/", "images/");
    SolverMeta smS(iterate_sor, "sor/", "images/");
    ImageSolver imageSolver;

    if(argc == 2)// && ! ( strcmp(argv[1], "-p") || strcmp(argv[1], "-n")))
    {
        cout << "Assuming \"-f " << string(argv[1]) << " --gauss --plot" << endl;
        vFuncContainer.push_back(smG);
        imageSolver.addFolder(string(argv[1]));
        imageSolver.solve(vFuncContainer, false);
        plot::plot();
        exit(EXIT_SUCCESS);
    }


    while(iarg != -1)
    {
        iarg = getopt_long(argc, argv, "acf:gjlhnpr:sv", longopts, &index);

        switch (iarg)
        {
            case 'a':
                a++;
                break;

            case 'c':
                c++;
                break;

            case 'f':
                f++;
                folder = optarg;
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

            case 'p':
                p++;
                break;
            case 'v':
                v++;
                break;

            default: 
                // usage();
                break;
        }
    } 

    if(f) {
        if(vFuncContainer.size() < 1 && !n)
            cout << "Warning: no iterators chosen" << endl;
        imageSolver.addFolder(folder);
        if(!n)
            imageSolver.solve(vFuncContainer, l>0);
    }
    else {
        cout << "Warning: no media folder given" << endl;
    }

    if(r)
    {
        ImageSolver imageSolver2;
        for (function_container::iterator it = vFuncContainer.begin();
            it != vFuncContainer.end();
            ++it)
        {
            string sPath = DATA_DIR + (*it).sPath + "/image";
            imageSolver2.addFolder(sPath);
        }

        imageSolver2.solve(vFuncContainer, l>0, "re", "re", dScalar);
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

    if(v)
    {
        CImgDisplay histo = imageSolver.histogram(folder, vFuncContainer);
        histo.show();
        // std::thread histoLoop(
    }



    std::thread plotLoop;
    if(p) 
    { 
        plot::plot();
        image_fmt imgPlot("graph.png");
        CImgDisplay plot_disp(imgPlot, "graph.png : graph for all images in folder",0, false, true);	
        plotLoop = thread(renderImage, plot_disp);
    }


    if(c && f)
    {
        imageSolver.clearFolders();
        imageSolver.addFolder(folder, "when trying to show rendered images (-c flag)");
        imageSolver.renderImages(folder, vFuncContainer);
    }

    plotLoop.join();

    return 0;
}

#endif /* _MAIN.cpp */
