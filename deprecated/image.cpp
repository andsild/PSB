                    //TODO: push back makes it bigger than it should be
                    //      (should pre-compute size instead)
#ifndef _IMAGE_CPP
#define _IMAGE_CPP   1

#define cimg_debug 0 

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


void display_histogram(image_fmt image)
{
    image.display_graph("Histogram", 3);
}



rawdata_fmt averageResult(const std::vector<rawdata_fmt> &vInput, const int iDivSize)
{
    std::vector<rawdata_fmt> vInterpolatedInput;
    int iShortest = INT_MAX, iLongest = INT_MIN;
    for(auto const it : vInput)
    {
        iShortest = (iShortest < it.size()) ? iShortest : it.size();
        iLongest  = (iLongest  > it.size()) ? it.size() :  it.size();
    }
    // rawdata_fmt vRes(iShortest, 0);
    rawdata_fmt vRes(iLongest, 0);
    
    for(const auto & it : vInput)
    {
        rawdata_fmt interpolatedData = sampleRateConversion(it, vRes.size());
        vInterpolatedInput.push_back(interpolatedData);
    }
    
    for(const auto it : vInterpolatedInput)
    {
        for(int iPos = 0; iPos < iShortest; iPos++)
        {
            vRes.at(iPos) += it.at(iPos);
        }
    }

    for(auto & it : vRes)
    {
        it = (double)( it / (double)iDivSize);
    }

    return vRes;


    // for(int iPos = 0; iPos < vInput.size(); iPos++)
    // {
    //     double dElem = vInput[iPos];
    //     int iIndex = iPos % iMod;
    //     if(dElem == SPLIT_VALUE || (iIndex == 1 && iIndex > iPrev))
    //     {
    //         iPrev = iPos + 1;
    //         if(iIndex > iMod)
    //             iMod = iPos + 1;
    //         continue;
    //     }
    //     vRes[iPos % iMod] += vInput[iPos];
    // }
    // const double dDiv = (double)iDivSize;
    //
    // for(rawdata_fmt::iterator it = vRes.begin();
    //         it != vRes.end(); it++)
    // {
    //     if( (*it) == 0)
    //     {
    //         vRes.erase(it, vRes.end());
    //         break;
    //     }
    //     (*it) /= dDiv;
    // }

    // vRes.pop_back(); // a minus 1 that sneaks in
    return vRes;
}


//FIXME: must be EXTRAPOLATION, not interpolation!
rawdata_fmt sampleRateConversion(const rawdata_fmt &vData, const int iNewsize)
{
    if(vData.size() == 1 || vData.size() == iNewsize)
    {
        rawdata_fmt s(vData); // to avoid memory issues later
        return s;
    }
    if(vData.size() > iNewsize)
    {
        const int iInterpolationSize = ceil((double)vData.size() / iNewsize);
        rawdata_fmt s(iNewsize, 0);

        double dAccumulator = 0;
        for(int iPos = 0; iPos < vData.size(); iPos++)
        {
            dAccumulator = 0;
            for(int iIndex = 0; iIndex < iInterpolationSize; iIndex++)
            {
                dAccumulator += vData[iPos];
                iPos++;
            }
            iPos--;
            s.at(iPos / iInterpolationSize) = dAccumulator / (double)iInterpolationSize;
        }

        return s;
    }
    const int iExtrapolationSize = floor((double)iNewsize / vData.size()),
              iBorderElements = (iNewsize % vData.size());
    rawdata_fmt s(iNewsize, 0);

    s[0] = vData[0];
    int sPos = 1;
    for(int iPos = 1; iPos < vData.size() - 1; iPos++, sPos++)
    {
        double dDiff = vData[iPos + 1] - vData[iPos];
        double dInc = dDiff / iExtrapolationSize;
        // int sPos = ((iPos / iExtrapolationSize) + iPos);
        s.at(sPos) = vData[iPos];

        for(int iIndex = 1; iIndex < iExtrapolationSize; iIndex++)
        {
            s.at(sPos) = s.at(sPos - 1) + dInc;
            sPos++;
        }
    }

    for(int iPos = vData.size();
            iPos < vData.size() + iBorderElements;
            iPos++)
    {
        s.at(iPos) = vData.back(); // Copy last element
    }

    return s;

}






void calculateAverage(std::string sFilePath)
{
    // std::vector<std::string> files;
    //
    // std::string sReadFolder = DATA_DIR + sFilePath;
    //
    // try
    // {
    //     getFilesInFolder(sReadFolder.c_str(), files);
    // }
    // catch(file_IO::DirNotFound &f)
    // {
    //     std::cout << f.what() << std::endl;
        // MLOG(severity_type::error, f.what());
    // }
    //
    // rawdata_fmt average; // can give undererror
    // int iLineCount = numeric_limits<int>::max();
    // std::string avoid = "average";
    // double dValidFiles = 0;
    // list<int> lLengths;
    // for (std::vector<std::string>::iterator it = files.begin();
    //     it != files.end();
    //     ++it)
    // {
    //     size_t found = (*it).find(avoid);
    //     if(found!=std::string::npos)
    //     {
    //        LOG(severity_type::warning)("average: skipping file", *it);
    //        CLOG(severity_type::warning)("average: skipping file", *it);
    //         continue;
    //     }
    //     dValidFiles++;
    //
    //     ifstream infile;
    //     int iPos = 0;
    //     double dNum;
    //     infile.open(*it);
    //
    //     while(infile >> dNum) // read whole file or stop
    //     {
    //         if(iPos >= average.size())
    //             average.push_back(dNum);
    //         else
    //             average[iPos] += dNum;
    //         iPos++;
    //     }
    //
    //     lLengths.push_back(iPos);
    // }
    //
    // lLengths.sort();
    // for(std::vector<int>::size_type iPos = 0;
    //         iPos < (int)(average.size());
    //         iPos++)
    // {
    //     if(lLengths.size() > 0 &&
    //         iPos >= lLengths.front()) //XXX: or iPos >= ?
    //     {
    //         lLengths.pop_front();
    //         dValidFiles--;
    //     }
    //     average[iPos] /= dValidFiles;
    // }
    //
    // std::string sFilename = string("average") + DATA_EXTENSION;
    // writeToFile(average, sFilename, sFilePath);
}



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



void calculateAverage(string);

typedef CImg<double> image_fmt;
typedef CImgList<double> imageList_fmt;
typedef vector<SolverMeta> function_container;

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

d1 imageTo1d(CImg<double> image, double dScalar = 1)
{
    d1 image_vec;

    for(int iPos = 0; iPos < image.height(); iPos++)
    {
        for(int jPos = 0; jPos < image.width(); jPos++)
        {
            double newVal = (double)image(jPos, iPos, 0, 0) * dScalar;
            image_vec.push_back(newVal);
            //TODO: push back makes it bigger than it should be
            //      (should pre-compute size instead)
        }
    }
    return image_vec;
}


d1 convertImage(CImg<double> image, double dScalar = 1)
{
    CImg<double> grayscale;
    if(image.spectrum() == 1)
    {
       grayscale = image;
    }
    else
    {
        grayscale = sRGBtoGrayscale(image);
    }
    return imageTo1d(grayscale, dScalar);
}
    

CImgList<double> readImage(CImg<double> image,
                           const char *fileName, function_container vIf,
                           LoadingBar &loadbar,
                           vector<vector<string> > &vRet,
                           double dScalar = 1)
{
    ImageProcess<double> test(image, fileName, .9 );


    double max_error = .9;
    int iHeight = image.height(), iWidth = image.width(),
         iDepth = image.depth(), iSpectrum = image.spectrum();
    int iDim = iWidth * iHeight;
    d1 U = vector<double>(iDim, 0); 
    CImgList<double> retList;
    d1 image_vec = convertImage(image, dScalar);
    //TODO: check file type

    loadbar.updateTimeRemaining(iHeight * iWidth);

    /* Solve */
    for (function_container::iterator it = vIf.begin();
        it != vIf.end();
        ++it)
    {

        cout << "beginning image " << fileName << endl;
        cout << loadbar << endl;
        vector<string> vOutput =  iterative_solve((*it).func, 
                                                   image_vec, U,
                                                   max_error, iDim, iWidth);

        double *dImage = new double[image_vec.size()];
        if(vOutput.size() < 1)
        {
            cout << "Solver had no results for " << fileName << endl;
            continue;
        }

        vRet.push_back(vOutput);

        for(vector<int>::size_type iPos = 0;
                iPos < image_vec.size();
                iPos++) 
        {
            dImage[iPos] = image_vec.at(iPos);
        }

        CImg<double> test(dImage, iWidth, iHeight,
                          1, 1, false);
    }
    d1 poo(0,0);
    image_vec = poo;
    return retList;
}

void writeImageList(function_container vIf, const char *filename, vector<vector<string> > err)
{
    if(vIf.size() != err.size() ) { cout << "DADAW" << endl; exit(EXIT_FAILURE); }

    for(vector<int>::size_type iPos = 0;
        iPos < (int)(err.size());
        iPos++) 
    {
        writeToFile(err[iPos], filename, vIf[iPos].sPath);
    }
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


void printImage(image_fmt image)
{
    for(int iPos = 0; iPos < image.width(); iPos++)
    {
        for(int jPos = 0; jPos < image.height(); jPos++)
        {
            cout << image(iPos, jPos, 0, 0) << " " ; 
        }
        cout << endl;
    }
}


CImgList<double> processImages(vector<string> &filenames, 
                               function_container vIf, LoadingBar &loadBar,
                               double dScalar = 1, bool resolve = false)
{
    CImgList<double> images;

    for (vector<string>::iterator it = filenames.begin();
         it != filenames.end();
         //XXX: No increment (using erase)
        )
    {
        CImg<double> img;
        try {
            cimg::exception_mode(0);
            string sNew = (*it);

            // if(resolve)
            // {
            //     string avoid = "image";
            //     size_t found = (*it).find(avoid);
            //     if(found!=string::npos) 
            //     {
            //         sNew = (*it).substr(0, found);
            //         sNew = "/new" + (*it).substr(found+6, (*it).size());
            //     }
            // }
            img = getImage(sNew.c_str());
            // img = getImage((*it).c_str());
        }
        catch(CImgIOException &cioe) {
            cout << cioe.what() << endl;
            loadBar.increaseProgress(vIf.size() - 1);
            cout << loadBar << endl;
            it = filenames.erase(it);

            continue;
        }

        /* Returns images for each iteration */
        vector<vector<string> > vRet;
        CImgList<double> cil = readImage(img, (*it).c_str(),
                                         vIf, loadBar, vRet, dScalar);
        // printImage(img);
        //FIXME: damage is already done
        writeImageList(vIf, (*it).c_str(), vRet);
        images.insert(cil, images.size());

        it++;
    }

    return images;
}
void saveImages(vector<string> filenames, CImgList<double> images,
        function_container vIf)
{
    for(vector<int>::size_type iPos = 0;
            iPos < (int)(filenames.size());
            iPos++) 
    {
        string sFilename = filenames[iPos];
        trimLeadingFileName(sFilename);

        for(vector<int>::size_type jPos = 0;
                jPos < vIf.size();
                jPos++) 
        {
            string sDest = DATA_DIR + vIf[jPos].sPath + "/image/";
            string sImageDest = sDest + sFilename;

            //TODO: all file-writing should be standardized in 
            // file.cpp to avoid errors
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
}


/** Read a series of images and solve them
 *
 */
void readFolder(string sDir, function_container vIf, double dScalar = 1, bool resolve= false)
{
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

    CImgList<double> images = processImages(filenames, vIf, loadBar, dScalar, resolve);
    //make it so that process images do not write .... just solve and return
    cout << loadBar << endl;// if last image has errors, this might be necessary
    saveImages(filenames, images, vIf);
}

void calculateAverage(string sFilePath)
{
    sFilePath += "/"; // just in case
    vector<string> files;

    string sReadFolder = DATA_DIR + sFilePath;

    try
    {
        files = getFilesInFolder(sReadFolder.c_str());
    }
    catch(file_IO::DirNotFound &f)
    {
        cout << f.what() << endl;
        exit(EXIT_FAILURE);
    }


    d1 average; // can give undererror
    int iLineCount = numeric_limits<int>::max();
    //FIXME: doesn't exclude average.dat
    string avoid = "average";
    double dValidFiles = 0;
    list<int> lLengths;
    for (vector<string>::iterator it = files.begin();
        it != files.end();
        ++it)
    {
        size_t found = (*it).find(avoid);
        if(found!=string::npos) 
        {
           cout << "average: skipping file " << *it << endl;
            continue;
        }
        dValidFiles++;

        ifstream infile;
        int iPos = 0;
        double dNum;
        infile.open(*it);

        while(infile >> dNum) // read whole file or stop
        {
            if(iPos >= average.size()) 
                average.push_back(dNum);
            else
                average[iPos] += dNum;
            iPos++;
        }

        lLengths.push_back(iPos);
    }

    lLengths.sort();
    for(vector<int>::size_type iPos = 0;
            iPos < (int)(average.size());
            iPos++) 
    {
        if(lLengths.size() > 0 &&
            iPos >= lLengths.front()) //XXX: or iPos >= ?
        {
            lLengths.pop_front();
            dValidFiles--;
        }
        average[iPos] /= dValidFiles;
    }

    string sFilename = string("average") + DATA_EXTENSION;
    writeToFile(average, sFilename, sFilePath);
}

void re_solve(string sDir, function_container vIf)
{
    vector<string> naughty;
    for (function_container::iterator it = vIf.begin();
        it != vIf.end();
        ++it)
    {
        //XXX
    }

    for (function_container::iterator it = vIf.begin();
        it != vIf.end();
        ++it)
    {
        function_container lazy;
        // sDir = DATA_DIR + (*it).sPath + string("image/");
        sDir = DATA_DIR + (*it).sPath + string("image/");
        (*it).sPath = (*it).sPath + "new/";
        lazy.push_back(*it);
        readFolder(sDir, lazy, 2, true);

        naughty.push_back((*it).sPath);
    }


    for(vector<int>::size_type iPos = 0;
            iPos < (int)(naughty.size());
            iPos++) 
    {
        vIf[iPos].sPath = naughty[iPos];        
    }

}


}

#endif 
