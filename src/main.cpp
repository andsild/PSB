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

//returns the forward 2nd derivative w.r.t x and y (d2u/dxdy)
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
    double D[9] = {0,1,0, 0,-4,1, 0,1,0};
    CImg<double> mask = inputkernel(D, 3);
    //FIXME: specified wrong order
    mask(0, 1) = 1;
    //FIXME: ... this is not the right way to mask, is it?
    CImg<double> masked = image.get_convolve(mask);
    CImg<double> F = masked.get_vector();
    // cannot use RAW array, running out of stack space
    int height = F.height();
    d1 vec1(height, 0);
    for(int iPos = 0; iPos < F.height(); iPos++)
    {
        vec1[iPos] = F(0, iPos);
    }
    F = CImg<double>(1, 1, 1, 1, 1); // null out, get memory back
    d1 U(height, 0);

    // matrix_type x2 = iterative_solve(vec1, U, max_error, F.height(), image.width());
    cout << "reading image" << endl;
    // two_grid(1, U, vec1, image.width(), 5);
    
}

void readSingleImage()
{
    CImg<unsigned char> image("./media/109201.jpg"); //example
    readImage(image);
}

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

        // If the file is a directory (or is in some way invalid) we'll skip it 
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

            default:
                readSingleImage();
                break; // do nothing
        }
    } 
    
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
