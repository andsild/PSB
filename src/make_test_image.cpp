#ifndef _TEST_MAKE
#define _TEST_MAKE 1

#define cimg_debug 0 
#define DEFAULT_MODE      S_IRWXU | S_IRGRP |  S_IXGRP | S_IROTH | S_IXOTH

#include <fstream>
#include <iostream>
#include <iomanip>
#include <limits>
#include <list>
#include <sstream> 
#include <string>

#include <dirent.h>
#include <getopt.h>
#include <math.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>


#include "CImg.h"

#include <sys/ioctl.h>

using namespace cimg_library;
using namespace std;

namespace test
{

typedef CImg<double> image_fmt;

void createImage(double d[], int iWidth, int iHeight, const char *name)
{
    image_fmt image(d, iWidth, iHeight, 1, 1, false);
    const char *folder = "./test_images/";
    mkdir(folder, DEFAULT_MODE);
    string s = string(folder) + string(name) + ".png";
    image.save(s.c_str());
}

}
using namespace test;
int main(int argc, char *argv[]) 
{
    double D1[] = {0,0,0, 0,8,0, 0,0,0};
    double D2[] = {0,1,0, 1,-4,1, 0,1,0};
    double D4[] = {1,1,1, 1,-0,1, 1,1,1};

    double D3[] = {1,1,1,1,1, 1,0,0,0,1, 1,0,0,0,1, 1,0,0,0,1,  1,1,1,1,1};


    createImage(D1, 3, 3, "simpleTest");
    createImage(D2, 3, 3, "kernel matrix");
    createImage(D3, 5, 5, "5x5border");
    createImage(D4, 3, 3, "simpleTestDense");
}


// } /* EndOfNamespace */

#endif
