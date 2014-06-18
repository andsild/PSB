#ifndef _MAIN_CPP
#define _MAIN_CPP   1

#define DATA_DIR "./data/"
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

int main(int argc, char *argv[]) 
{
    const struct option longopts[] =
    {
        {"image",   no_argument,        0, 'i'},
        {"folder",     required_argument,  0, 'f'},
        {0,0,0,0},
    };

    int index;
    int iarg=0;
    extern char *optarg;
    
    int f = 0, g = 0, i = 0, j = 0, p = 0, s = 0;
    char *folder;
    vector<iterative_function> test;

    while(iarg != -1)
    {
        iarg = getopt_long(argc, argv, "f:gijps", longopts, &index);

        switch (iarg)
        {
            case 'i':
                i++;
                break;

            case 'f':
                f++;
                folder = optarg;
                break;

            case 'g':
                test.push_back(iterate_gauss);
                break;

            case 'j':
                test.push_back(iterate_jacobi);
                break;

            case 's':
                test.push_back(iterate_sor);
                break;

            case 't':
                break;
            
            case 'p':
                p++;

            default: 
                break; 
        }
    } 

    if(f) {
        //FIXME: doesn't add a (necessary) trailing  slash
        readFolder(folder, test);
    }
    else if(i)
    {
        readSingleImage(test);
    }
    else //default
    {
        test.push_back(iterate_gauss);
        string sDir = "small_media/";
        readFolder(sDir, test);
        // readSingleImage(test);
    }

    if(p) 
    { 
        plot::plot();
    }
    
    //show image(s)
    // CImg<unsigned char> image("./media/icon_img.png");
	// CImgDisplay main_disp(image,"Image",0);	
	// CImgDisplay mask_disp(x2,"Image",0);	
    //
    // while (!main_disp.is_closed() && 
    //        !main_disp.is_keyESC() &&
    //        !main_disp.is_keyQ()) {
    //     main_disp.wait();
    // }
    //
    return 0;
}

#endif /* _MAIN.cpp */
