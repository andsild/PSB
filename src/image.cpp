#ifndef _IMAGE_CPP
#define _IMAGE_CPP   1

#define cimg_debug 0 

#include <fstream>
#include <iostream>
#include <iomanip>
#include <limits>
#include <sstream> 
#include <string>

#include <dirent.h>
#include <getopt.h>
#include <math.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>


#include "CImg.h"
#include "./file.cpp"
#include "solvers/iterative_solvers.cpp"
#include "plot.cpp"

#include <sys/ioctl.h>

using namespace cimg_library;
using namespace pe_solver;
using namespace file_IO;
using namespace plot;


namespace image_psb
{

class LoadingBar
{
    private:
        int iNumIterations;
        double dStepSize;
        mutable double dProgress;
        mutable int iTimeRemaining;
        mutable int iImageSize;

    public:
        struct winsize w;
        LoadingBar(int iNumIterations) : iNumIterations(iNumIterations)
    {
        this->dProgress = 0;
        this->dStepSize = ((double)100 / iNumIterations);
        this->iTimeRemaining = 0;//filenames.size() * vIf.size() 
                             //* (pow( (img.height() * img.width()), 0.3));
    }
    string getTimeLeft() const
    {
        int iMinutesLeft = this->iTimeRemaining / 60;
        int iSecondsLeft = this->iTimeRemaining - (iMinutesLeft * 60);
        return string(to_string(iMinutesLeft) + "m"
                + to_string(iSecondsLeft) + "s");
    }
    void updateTimeRemaining(int iImageSize) const
    {
        this->iImageSize = iImageSize;
        int iIterationsLeft = int((100 - this->getProgress()) 
                                   / this->dStepSize);
        // rough estimate
        this->iTimeRemaining = iIterationsLeft * pow(iImageSize, 0.3); 
    }
    void increaseProgress(int iIterations = 1) const
    {
        this->dProgress += this->dStepSize * iIterations;
        updateTimeRemaining(this->iImageSize);
    }
    double getProgress() const { return this->dProgress; }
    friend ostream& operator<< (ostream &str, const LoadingBar& obj);

};

ostream& operator<< (ostream &str, const LoadingBar& obj)
{
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &obj.w);

    if(obj.getProgress() > 100) 
    { 
        // str << "(error: loading bar exceeded 100 %)" << endl;
        return str;
    }

    const int xPos = 0, yPos = 0;
    // To compensate for []>::space:: symbols
    int iColWidth = obj.w.ws_col - 20; 
    int iSignPercentage = (int) ((obj.getProgress() * iColWidth) / 100),
        iPercentage     = (int)obj.getProgress();
    string sMarker(iColWidth, '=');

    // printf("\033[%d;%dH[%s>%*c] %3d%%\n", xPos, yPos
    printf("\r[%s>%*c] %3d%% ETA %s"
                             , sMarker.substr(0,iSignPercentage).c_str()
                             , iColWidth - iSignPercentage, ' '
                             , iPercentage
                             , obj.getTimeLeft().c_str());
    obj.increaseProgress();
    return str;
}

void calculateAverage(string);

typedef CImg<double> image_fmt;
typedef CImgList<double> imageList_fmt;

CImg<double> sRGBtoGrayscale(CImg<double> image)
{
    image.sRGBtoRGB();
    CImg<double> grayscale(image.width(), image.height(), 1, 1, 0);
    for(int iPos = 0; iPos < image.width(); iPos++)
    {
        for(int jPos = 0; jPos < image.height(); jPos++)
        {
            double r = image(iPos, jPos, 0, 0); // First channel RED
            double g = image(iPos, jPos, 0, 1);
            double b = image(iPos, jPos, 0, 2);

            double grayValue = (r + g + b) / 3 ;
            grayscale(iPos, jPos, 0, 0) = grayValue;


            // cout << image(iPos, jPos) << "\t" << image(iPos, jPos, 0, 0) 
            //     << "\t" << image(iPos, jPos, 0, 1) << "\t" <<
            //     image(iPos, jPos, 0, 2) << endl;
            // average method (per gimp)
            image(jPos, iPos) = (r / 3)  ;
        }
    }

    return image;
}

d1 imageTo1d(CImg<double> image)
{
    d1 image_vec;

    for(int iPos = 0; iPos < image.height(); iPos++)
    {
        for(int jPos = 0; jPos < image.width(); jPos++)
        {
            image_vec.push_back(image(jPos, iPos, 0, 0));
            //TODO: push back makes it bigger than it should be
            //      (should pre-compute size instead)
        }
    }
    return image_vec;
}

class SolverMeta
{
    public:
        iterative_function func;
        string sPath;
        vector<string> vSubDirs;
        SolverMeta(iterative_function func,
                          string sPath,
                          vector<string> *vSubDirs = NULL)
            : func(func), sPath(sPath), vSubDirs(*vSubDirs)
        {
        }
        SolverMeta(iterative_function func,
                          string sPath,
                          string sSubDir)
            : func(func), sPath(sPath)
        {
            this->vSubDirs.push_back(sSubDir);
        }
};

typedef vector<SolverMeta> function_container;

CImgList<double> readImage(CImg<double> image,
                           const char *fileName, function_container vIf,
                           LoadingBar &loadbar)
{
    double max_error = .9;
    int iHeight = image.height(), iWidth = image.width(),
         iDepth = image.depth(), iSpectrum = image.spectrum();
    int iDim = iWidth * iHeight;
    d1 U = vector<double>(iDim, 0); 
    CImg<double> grayscale = sRGBtoGrayscale(image);
    CImgList<double> retList;
    d1 image_vec = imageTo1d(grayscale);

    loadbar.updateTimeRemaining(iHeight * iWidth);

    /* Solve */
    for (function_container::iterator it = vIf.begin();
        it != vIf.end();
        ++it)
    {
        double *dImage = new double[image_vec.size()];

        cout << "beginning image " << fileName << endl;
        cout << loadbar << endl;
        vector<string> vOutput =  iterative_solve((*it).func, 
                                                   image_vec, U,
                                                   max_error, iDim, iWidth);
        if(vOutput.size() < 1)
        {
            cout << "Solver had no results for " << fileName << endl;
            continue;
        }
        writeToFile(vOutput, fileName, (*it).sPath);

        for(vector<int>::size_type iPos = 0;
                iPos < image_vec.size();
                iPos++) 
        {
            dImage[iPos] = image_vec.at(iPos);
        }

        cout << "\rfinished image " << fileName << endl;
        CImg<double> test(dImage, iWidth, iHeight,
                          1, 1, false);
        retList.push_back(test);
    }
    return retList;
}

CImg<double> getImage(string sDest)
{
    CImg<double> image;
    cimg::exception_mode(0);
    try
    {
        image = CImg<double>(sDest.c_str()); //example
    }
    catch(CImgIOException cioe)
    {
        string sMsg = string("Unable to open image: ") + sDest;
        throw CImgIOException(sMsg.c_str(), &cioe);
    }

    return image;
}

/** Read a series of images and solve them
 *
 */
void readFolder(string sDir, function_container vIf)
{
    CImgList<double> images; 
    vector<string> filenames;
    int iTotalIterations;
    double dProgress = 0;
    double dStepSize;


    try{
        filenames = getFilesInFolder(sDir);
    }
    catch(const file_IO::DirNotFound& f)
    {
        cout << f.what() << endl;
        exit(EXIT_FAILURE);
    }

    iTotalIterations = filenames.size() * vIf.size();
    dStepSize = ((double)100 / iTotalIterations);
    int iTimeRemaining = filenames.size() * vIf.size();
    LoadingBar loadBar(iTotalIterations);


    for(vector<int>::size_type iPos = 0;
            iPos < filenames.size();
            iPos++) 
    {
        CImg<double> img;
        try {
            cimg::exception_mode(0);
            img = getImage(filenames[iPos].c_str());
        }
        catch(CImgIOException &cioe) {
            cout << cioe.what() << endl;
            loadBar.increaseProgress(vIf.size() - 1);
            cout << loadBar << endl;

            continue;
        }

        /* Returns images for each iteration */
        CImgList<double> cil = readImage(img, filenames[iPos].c_str(),
                                         vIf, loadBar);
        images.insert(cil, images.size());
    }
    cout << loadBar << endl;// if last image has errors, this might be necessary

    for(vector<int>::size_type iPos = 0;
            iPos < (int)(images.size());
            iPos += vIf.size()) 
    {
        trimLeadingFileName(filenames[iPos]);

        string sDest;
        for(vector<int>::size_type jPos = 0;
                jPos < vIf.size();
                jPos++) 
        {
            sDest = DATA_DIR + vIf[jPos].sPath + "/image/";
            string sImageDest = sDest + filenames[iPos];

            cimg::exception_mode(0);
            try {
            images[iPos+jPos].save(sImageDest.c_str());
            }
            catch(CImgIOException cioe)
            {
                mkdirp(sDest.c_str());
                images[iPos+jPos].save(sImageDest.c_str());
                cout << cioe.what() << endl;
                continue;
            }
        }
    }

    for (function_container::iterator it = vIf.begin();
        it != vIf.end();
        ++it)
    {
        string sPath = DATA_DIR + (*it).sPath;
        calculateAverage(sPath);
    }
}

void calculateAverage(string sFilePath)
{
    sFilePath += "/"; // just in case
    vector<string> files;

    try
    {
        files = getFilesInFolder(sFilePath.c_str());
    }
    catch(file_IO::DirNotFound &f)
    {
        cout << f.what() << endl;
        exit(EXIT_FAILURE);
    }

    vector<double> average; // can give undererror
    int iLineCount = numeric_limits<int>::max();
    //FIXME: doesn't exclude average.dat
    for (vector<string>::iterator it = files.begin();
        it != files.end();
        ++it)
    {
        ifstream infile;
        int iPos = 0;
        double dNum;
        infile.open(*it);

        while(infile >> dNum && iPos < iLineCount) // read whole file or stop
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
    string sDest = sFilePath + "/" + sFileName + DATA_EXTENSION;
    writeToFile(average, "average", sDest);
}

}

#endif 
