#ifndef _MAIN_CPP
#define _MAIN_CPP   1

#define DATA_DIR "./data/"
#define DATA_EXTENSION ".dat"

#include <fstream>
#include <getopt.h>
#include <iostream>
#include <string>
#include <sys/stat.h>

#include <dirent.h>
#include <unistd.h>
#include <sys/types.h>


#include "../lib/CImg-1.5.8/CImg.h"
#include "solvers/iterative_solvers.cpp"

#define no_argument 0
#define required_argument 1 
#define optional_argument 2

using namespace cimg_library;
using namespace pe_solver;
using namespace std;

CImg<double> inputkernel(double entries[], int iNewDim)
{
    CImg<double> image(entries, iNewDim, iNewDim, 1, 1, false);
    return image;
}

CImg<double> inputkernel(double entries[], int iNewWidth, int iNewHeight)
{
    CImg<double> image(entries, iNewWidth, iNewHeight, 1, 1, false);
    return image;
}

void readImage(CImg<double> image)
{
    double max_error = 1;
    double D[9] = {0,1,0, 1,-4,1, 0,1,0};
    CImg<double> mask = inputkernel(D, 3); 

    CImg<double> masked = image.get_convolve(mask); 
        //FIXME: ... this is not the right way to mask, is it?
    CImg<double> F = masked.get_vector(); //FIXME: has > inf excess pixels..
    int iHeight = F.height();
    d1 image_vec(iHeight, 0);

    /* Load F into a vector */
    for(int iPos = 0; iPos < F.height(); iPos++)
    {
        image_vec[iPos] = F(0, iPos);
    }
    F = CImg<double>(1, 1, 1, 1, 1); // TODO: does this get memory back?
    d1 U(iHeight, 0); // wait to declare this until memory is freed?

    /* Solve */
    iterative_solve(iterate_gauss,
                    image_vec, U,
                    max_error, F.height(), image.width());
    // two_grid(1, U, vec1, image.width(), 5);
}

void readSingleImage()
{
    CImg<unsigned char> image("./media/109201.jpg"); //example
    readImage(image);
}

/** Read a series of images and solve them
 *
 */
void readFolder(char *dir)
{
    CImgList<double> imgList;

    ifstream fin;
    string filepath;
    int num;
    DIR *dp;
    struct stat filestat;
    struct dirent *dirp;

    dp = opendir( dir );
    if (dp == NULL)
    {
        cout << "Error in openning (errcode: " << errno << ")" << endl;
        return;
    }

    while ((dirp = readdir( dp )))
    {
        filepath = strcat(dir, "/");
        filepath = filepath + dirp->d_name;

        // Check for valid file(s)
        if (stat( filepath.c_str(), &filestat )) continue;
        if (S_ISDIR( filestat.st_mode ))         continue;

        CImg<unsigned char> img(filepath.c_str());
        imgList.push_back(img);
    }

    for (CImgList<double>::iterator it = imgList.begin(); it != imgList.end(); ++it)
    {
        readImage(*it);
    }
}

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
    
    // opterr=1; //turn off getopt error message

    while(iarg != -1)
    {
        iarg = getopt_long(argc, argv, "if:", longopts, &index);

        switch (iarg)
        {
            case 'i':
                readSingleImage();
                break;

            case 'f':
                std::cout << optarg << std::endl;
                readFolder(optarg);
                break;

            default: // always do something
                readSingleImage(); 
                break; 
        }
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

    // for(int iPos = 0; iPos < 3; iPos++)
    // {
    //     for(int jPos = 0; jPos < 3; jPos++)
    //     {
    //         cout << mask(iPos, jPos);
    //     }
    //     cout << endl;
    // }
    // return;
    //
#endif /* _MAIN.cpp */
