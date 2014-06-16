#ifndef _MAIN_CPP
#define _MAIN_CPP   1

#define DATA_DIR "./data/"
#define DATA_EXTENSION ".dat"
#define PRECISION 20

#include <fstream>
#include <getopt.h>
#include <iostream>
#include <iomanip>
#include <limits>
#include <map>
#include <sstream> 
#include <string>

#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "../lib/CImg-1.5.8/CImg.h"
#include "./file.cpp"
#include "solvers/iterative_solvers.cpp"

#define no_argument 0
#define required_argument 1 
#define optional_argument 2

using namespace cimg_library;
using namespace pe_solver;
using namespace file_IO;
using namespace std;

void calculateAverage(string);

typedef CImg<double> image;
typedef CImgList<double> imageList;

CImgList<double> readImage(CImg<double> image, const char *fileName, vector<iterative_function> vIf)
{
    double max_error = 1;
    int iHeight = image.height();

    CImgList<double> retList;
    d1 image_vec(iHeight, 0);
    d1 U;
    map<iterative_function, string> fileLocations;

    fileLocations[iterate_gauss] = "gauss";
    fileLocations[iterate_jacobi] = "jacobi";
    fileLocations[iterate_sor] = "sor";

    /* Load F into a vector */
    for(int iPos = 0; iPos < iHeight; iPos++)
    {
        image_vec[iPos] = image(0, iPos);
    }
    image = CImg<double>(1, 1, 1, 1, 1); // TODO: does this get memory back?
    U = vector<double>(iHeight, 0); // wait to declare this until memory is freed?

    /* Solve */
    for (vector<iterative_function>::iterator it = vIf.begin();
            it != vIf.end();
            ++it)
    {
        double *dImage = new double[image_vec.size()];

        vector<string> vOutput =  iterative_solve(*it, 
                                       image_vec, U,
                                       max_error, image.height(), image.width());

        writeToFile(vOutput, strcat(fileLocations[*it], fileName), fileLocations[*it]);

        for(vector<int>::size_type iPos = 0;
                iPos < image_vec.size();
                iPos++) 
        {
            dImage[iPos] = image_vec.at(iPos);
        }

        CImg<double> test(dImage, image.width(), image.height(),
                          image.depth(), image.spectrum(), true);
        retList.push_back(test);
        delete dImage;
    }

    return retList;
}

void readSingleImage(vector<iterative_function> vIf)
{
    // CImg<unsigned char> image("./media/109201.jpg"); //example
    char cImageName[] = "./small_media/104000.jpg";
    CImg<unsigned char> image(cImageName); //example
    CImgList<double> cil = readImage(image, cImageName, vIf);
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
    vector<string> filenames;

    while ((dirp = readdir( dp )))
    {
        string readFile = filepath + dirp->d_name;

        // Check for valid file(s)
        if (stat( readFile.c_str(), &filestat )) continue;
        if (S_ISDIR( filestat.st_mode ))         continue;

        CImg<unsigned char> img(readFile.c_str());
        imgList.push_back(img);
        filenames.push_back(readFile);
    }


    CImgList<double> images; 
    for(vector<int>::size_type iPos = 0;
            iPos < filenames.size();
            iPos++) 
    {
        /* Returns images for each iteration */
        CImgList<double> cil = readImage(imgList[iPos], filenames[iPos].c_str(), vIf);
        cil.move_to(images);
    }


    map<iterative_function, string> fileLocations;
    fileLocations[iterate_gauss] = "gauss/";
    fileLocations[iterate_jacobi] = "jacobi/";
    fileLocations[iterate_sor] = "sor/";


    for (CImgList<double>::iterator imageIt = images.begin();
         imageIt != images.end();
         ++imageIt)
    {
        for (vector<iterative_function>::iterator funcIt = vIf.begin();
                funcIt != vIf.end();
                ++funcIt)
        {
            string sDest = DATA_DIR;
            sDest += fileLocations[*funcIt];
            string sImageDest = sDest + "/file.png";

            (*imageIt).save(sImageDest.c_str());
            calculateAverage(sDest);
        }
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
        || readFile.compare("average.dat") == 0)
        {
            continue;
        }
        files.push_back(readFile);
    }

    double dNumFiles = files.size();
    vector<double> average; // can give undererror
    int iLineCount = numeric_limits<int>::max();
    cout << files.size() << endl;
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
            if(iPos >= average.size())
                average.push_back(dNum);
            else
                average[iPos] += dNum;
            iPos++;
        }
        /* Smallest file */
        if (iPos < iLineCount) { iLineCount = iPos; }
    }

    string sFileName = "average";
    ofstream data_file(sFilePath + "/" + sFileName + DATA_EXTENSION);
    for(vector<int>::size_type iPos = 0;
            iPos < iLineCount;
            iPos++) 
    {
        data_file << setprecision(PRECISION) << fixed << (average[iPos] /= dNumFiles) << endl;
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
        char s1[] = "small_media/";
        readFolder(s1, test);
        // readSingleImage(test);
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
