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
#include <fstream>
#include <sstream> 
#include <map>


#include "../lib/CImg-1.5.8/CImg.h"
#include "solvers/iterative_solvers.cpp"

#define no_argument 0
#define required_argument 1 
#define optional_argument 2

using namespace cimg_library;
using namespace pe_solver;
using namespace std;

void calculateAverage(string);

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

void readImage(CImg<double> image, vector<iterative_function> vIf)
{
    double max_error = 1;
    double D[9] = {0,1,0, 1,-4,1, 0,1,0};
    CImg<double> mask = inputkernel(D, 3); 

    map<iterative_function, string> fileLocations;
    fileLocations[iterate_gauss] = "gauss";
    fileLocations[iterate_jacobi] = "jacobi";
    fileLocations[iterate_sor] = "sor";


    CImg<double> masked = image.get_convolve(mask); 
       //FIXME: ... this is not the right way to mask, is it?
    CImg<double> F = masked.get_vector(); //FIXME: has > inf excess pixels..
    F = image;
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
    for (vector<iterative_function>::iterator it = vIf.begin();
            it != vIf.end();
            ++it)
    {
    // iterative_solve(*it, fileLocations[*it],
    //                 image_vec, U,
    //                 max_error, F.height(), image.width());
        iterative_solve(*it, fileLocations[*it],
                        image_vec, U,
                        max_error, F.height(), image.width());
    }
    // two_grid(1, U, vec1, image.width(), 5);
}

void readSingleImage(vector<iterative_function> vIf)
{
    // CImg<unsigned char> image("./media/109201.jpg"); //example
    CImg<unsigned char> image("./small_media/104000.jpg"); //example
    readImage(image, vIf);
}

/** Read a series of images and solve them
 *
 */
void readFolder(char *dir, vector<iterative_function> vIf)
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

    filepath = strcat(dir, "/");

    while ((dirp = readdir( dp )))
    {
        string readFile = filepath + dirp->d_name;

        // Check for valid file(s)
        if (stat( readFile.c_str(), &filestat )) continue;
        if (S_ISDIR( filestat.st_mode ))         continue;

        CImg<unsigned char> img(readFile.c_str());
        imgList.push_back(img);
    }


    for (CImgList<double>::iterator it = imgList.begin(); it != imgList.end(); ++it)
    {
        readImage(*it, vIf);
    }

    map<iterative_function, string> fileLocations;
    fileLocations[iterate_gauss] = "gauss";
    fileLocations[iterate_jacobi] = "jacobi";
    fileLocations[iterate_sor] = "sor";


    for (vector<iterative_function>::iterator it = vIf.begin();
            it != vIf.end();
            ++it)
    {
        string sDest = DATA_DIR;
        sDest += fileLocations[*it];
        calculateAverage(sDest);
    }
}

void calculateAverage(string sFilePath)
{
    ifstream fin;
    string filepath;
    int num;
    DIR *dp;
    struct stat filestat;
    struct dirent *dirp;

    vector<string> files;
    sFilePath += "/";

    dp = opendir( sFilePath.c_str() );
    if (dp == NULL)
    {
        cout << "Error in openning (errcode: " << errno << ")" << endl;
        return;
    }

    while ((dirp = readdir( dp )))
    {
        string readFile = sFilePath + dirp->d_name;

        // Check for valid file(s)
        if (stat( readFile.c_str(), &filestat)
        || (S_ISDIR( filestat.st_mode ))
        || readFile.compare("./data/average.dat") == 0)
        {
            continue;
        }
        files.push_back(readFile);
    }

    double dNumFiles = files.size();
    vector<double> average(990); // can give undererror
    for (vector<string>::iterator it = files.begin();
            it != files.end();
            ++it)
    {
        ifstream infile;
        infile.open(*it);
        int iPos = 0;
        double dNum;
        while(infile >> dNum)
        {
            average[iPos] = dNum;
            iPos++;
        }
    }

    string sFileName = "average";
    ofstream data_file(sFilePath + "/" + sFileName + DATA_EXTENSION);
    for (vector<double>::iterator it = average.begin();
            it != average.end();
            ++it)
    {
        if(*it == 0) { break; }
        data_file << (*it /= dNumFiles) << endl;
        // cout << (*it /= dNumFiles) << endl;
    }
    data_file.close();
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
    
    int f = 0, g = 0, i = 0, j = 0, s = 0;
    char *folder;
    vector<iterative_function> test;

    while(iarg != -1)
    {
        iarg = getopt_long(argc, argv, "f:gijs", longopts, &index);

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

            default: 
                break; 
        }
    } 


    if(f) {
        readFolder(folder, test);
    }
    else if(i)
    {
        readSingleImage(test);
    }
    else //default
    {
        test.push_back(iterate_gauss);
        readSingleImage(test);
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
