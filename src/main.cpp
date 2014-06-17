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

typedef CImg<double> image_fmt;
typedef CImgList<double> imageList_fmt;

CImgList<double> readImage(CImg<double> image,
                           const char *fileName, vector<iterative_function> vIf)
{
    double max_error = .9;
    int iHeight = image.height(), iWidth = image.width(),
         iDepth = image.depth(), iSpectrum = image.spectrum();
    int iDim = iWidth * iHeight;

    CImgList<double> retList;
    // d1 image_vec(iDim, 0);
    d1 image_vec;
    d1 U;
    map<iterative_function, string> fileLocations;

    fileLocations[iterate_gauss] = "gauss";
    fileLocations[iterate_jacobi] = "jacobi";
    fileLocations[iterate_sor] = "sor";

    for(int iPos = 0; iPos < image.height(); iPos++)
    {
        for(int jPos = 0; jPos < image.width(); jPos++)
        {
            image_vec.push_back(image(jPos, iPos));
            //TODO: push back makes it bigger than it should be
        }
    }

    image = CImg<double>(1, 1, 1, 1, 1); // TODO: does this get memory back?
    U = vector<double>(iDim, 0); // wait to declare this until memory is freed?

    /* Solve */
    for (vector<iterative_function>::iterator it = vIf.begin();
        it != vIf.end();
        ++it)
    {
        double *dImage = new double[image_vec.size()];

        vector<string> vOutput =  iterative_solve(*it, 
                                       image_vec, U,
                                       max_error, iDim, iWidth);

        string fileLocation = (string)fileLocations[*it] + fileName;
        writeToFile(vOutput, fileName, fileLocations[*it]);

        for(vector<int>::size_type iPos = 0;
                iPos < image_vec.size();
                iPos++) 
        {
            dImage[iPos] = image_vec.at(iPos);
        }

        CImg<double> test(dImage, iWidth, iHeight,
                          iDepth, iSpectrum, false);
        retList.push_back(test);
        // delete dImage;
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
    CImgList<double> images; 
    vector<string> filenames;

    try{
        filenames = getFilesInFolder(dir);
    }
    catch(...) {
        cout << "dadwa" << endl;
    }

    for(vector<int>::size_type iPos = 0;
            iPos < filenames.size();
            iPos++) 
    {
        CImg<double> img(filenames[iPos].c_str());

        /* Returns images for each iteration */
        CImgList<double> cil = readImage(img, filenames[iPos].c_str(),
                                         vIf);
        images.insert(cil, images.size());
    }

    map<iterative_function, string> fileLocations;
    fileLocations[iterate_gauss] = "gauss/";
    fileLocations[iterate_jacobi] = "jacobi/";
    fileLocations[iterate_sor] = "sor/";

    for(vector<int>::size_type iPos = 0;
            iPos < filenames.size();
            iPos++) 
    {
        for (vector<iterative_function>::iterator funcIt = vIf.begin();
            funcIt != vIf.end();
            ++funcIt)
        {
            string sDest = DATA_DIR;
            sDest += fileLocations[*funcIt];
            trimLeadingFileName(filenames[iPos]);
            string sImageDest = sDest + "image/" + filenames[iPos];

            CImg<double> debug = images[iPos];
            cout << debug.width() << endl;
            images[iPos].save(sImageDest.c_str());
            calculateAverage(sDest);
        }
    }
}

void calculateAverage(string sFilePath)
{
    sFilePath += "/";
    vector<string> files = getFilesInFolder(&sFilePath[0]);

    vector<double> average; // can give undererror
    int iLineCount = numeric_limits<int>::max();
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
        data_file << setprecision(PRECISION) << fixed 
                  << (average[iPos] /= files.size()) << endl;
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
