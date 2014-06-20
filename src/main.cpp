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

#include <dirent.h>
#include <getopt.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "CImg.h"
#include "./image.cpp"
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
        {"folder",     required_argument,  0, 'f'},
        {"gauss",   no_argument,        0, 'g'},
        {"help",   no_argument,        0, 'h'},
        {"jacobi",   no_argument,        0, 'j'},
        {"plot",   no_argument,        0, 'p'},
        {"resolve",   no_argument,        0, 'r'},
        {"sor",   no_argument,        0, 's'},
        {0,0,0,0},
    };

    int index;
    int iarg=0;
    extern char *optarg;
    
    int a = 0, f = 0, g = 0, i = 0, j = 0, p = 0, r = 0, s = 0;
    char *folder;
    function_container vFuncContainer;
    SolverMeta smG(iterate_gauss, "gauss/", "images/");
    SolverMeta smJ(iterate_jacobi, "jacobi/", "images/");
    SolverMeta smS(iterate_sor, "sor/", "images/");

    if(argc == 2)
    {
        cout << "Assuming \"-f " << string(argv[1]) << " --gauss" << endl;
        vFuncContainer.push_back(smG);
        readFolder(string(argv[1]), vFuncContainer);
        plot::plot();
        exit(EXIT_SUCCESS);
    }


    while(iarg != -1)
    {
        iarg = getopt_long(argc, argv, "af:gjhprs", longopts, &index);

        switch (iarg)
        {
            case 'a':
                a++;
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

            case 'r':
                r++;
                break;

            case 's':
                vFuncContainer.push_back(smS);
                break;

            case 'p':
                p++;
                break;

            default: 
                // usage();
                break;
        }
    } 

    if(f) {
        if(vFuncContainer.size() < 1)
            cout << "Warning: no iterators chosen" << endl;
        readFolder(folder, vFuncContainer);
    }
    else //default
    {
        vFuncContainer.push_back(smG);
        string sDir = "../small_media/";
        readFolder(sDir, vFuncContainer);
    }

    if(r)
    {
        re_solve(folder, vFuncContainer);
    }

    if(p) 
    { 
        plot::plot();
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


    // //show image(s)
    // // CImg<unsigned char> image("./media/icon_img.png");
	// // CImgDisplay main_disp(image,"Image",0);	
	// // CImgDisplay mask_disp(x2,"Image",0);	
    // //
    // // while (!main_disp.is_closed() && 
    // //        !main_disp.is_keyESC() &&
    // //        !main_disp.is_keyQ()) {
    // //     main_disp.wait();
    // // }
    // //
    // return 0;
}

#endif /* _MAIN.cpp */
