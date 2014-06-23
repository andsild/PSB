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

class SolverMeta;

typedef CImg<double> image_fmt;
typedef CImgList<double> imageList_fmt;
typedef vector<SolverMeta> function_container;

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
        LoadingBar() {}
        LoadingBar(int iNumIterations) : iNumIterations(iNumIterations)
    {
        this->dProgress = 0;
        this->dStepSize = ((double)100 / iNumIterations);
        this->iTimeRemaining = 0;//filenames.size() * vIf.size() 
                             //* (pow( (img.height() * img.width()), 0.3));
    }

    void initialize(int iNumIterations)
    {
        this->iNumIterations = iNumIterations;
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

template <class T> class ImageProcess : CImg<T>
{
    private:
    typedef map<iterative_function,vector<string> > mapfuncres;
        image_fmt image;
        int iHeight, iWidth, iDepth, iSpectrum, iDim;
        const char *fileName;
        mapfuncres mapFuncRes;
        d1 image_vec;

        void genImageVec(double dScalar = 1)
        {
            for(int iPos = 0; iPos < this->iHeight; iPos++)
            {
                for(int jPos = 0; jPos < this->iWidth; jPos++)
                {
                    double newVal = (double)this->image(jPos, iPos, 0, 0) * dScalar;
                    this->image_vec.push_back(newVal);
                }
            }
        }
        
        CImg<double> sRGBtoGrayscale()
        {
            this->image.sRGBtoRGB();
            for(int iPos = 0; iPos < this->image.width(); iPos++)
            {
                for(int jPos = 0; jPos < this->image.height(); jPos++)
                {
                    double r = this->image(iPos, jPos, 0, 0); // First channel RED
                    double g = this->image(iPos, jPos, 0, 1);
                    double b = this->image(iPos, jPos, 0, 2);

                    double grayValue = (r + g + b) / 3 ;
                    image(jPos, iPos) = (r / 3)  ;
                }
            }

            return image;
        }

        void convertImage()
        {
            CImg<double> grayscale;
            d1 image_vec;

            if(this->image.spectrum() == 1)
            {
               grayscale = this->image;
            }
            else
            {
                grayscale = sRGBtoGrayscale();
            }

            for(int iPos = 0; iPos < grayscale.height(); iPos++)
            {
                for(int jPos = 0; jPos < grayscale.width(); jPos++)
                {
                    double newVal = (double)grayscale(jPos, iPos, 0, 0) * 1.0;
                    image_vec.push_back(newVal);
                    //TODO: push back makes it bigger than it should be
                    //      (should pre-compute size instead)
                }
            }
        }
            
    public:
        double dMaxErr;
        ImageProcess(image_fmt image, const char *fileName, double dMaxErr)
                : fileName(fileName), dMaxErr(dMaxErr),
                  image(image)

        {
            this->convertImage();
            this->genImageVec();

            this->iHeight = image.height();
            this->iWidth = image.width();
            this->iSpectrum = image.spectrum();
            this->iDepth = image.depth();
            this->iDim = image.height() * image.width();

        }
        vector<string> vOutput;

        void solve(iterative_function func)
       {
            d1 U = vector<double>(iDim, 0); 
            this->vOutput =  iterative_solve(func,
                                       this->image_vec, U,
                                       this->dMaxErr, iDim, iWidth);
       }

        void writeResultToFile(string fileDir)
        {
            string sFilename = string(this->fileName);
            writeToFile(this->vOutput, sFilename, fileDir);
        }
        void writeImageToFile(const char *fileDir)
        {
            double *dImage = new double[image_vec.size()];
            string sFileName = string(this->fileName);
            trimLeadingFileName(sFileName);
            string sFilename = string(fileDir) + "/" + sFileName;

            if(this->image_vec.size() < 1)
            {
                cout << "Solver had no results for " << fileName << endl;
                return;
            }

            for(vector<int>::size_type iPos = 0;
                    iPos < image_vec.size();
                    iPos++) 
            {
                dImage[iPos] = image_vec.at(iPos);
            }

            cimg::exception_mode(0);
            CImg<double> test(dImage, iWidth, iHeight,
                              1, 1, false);
            try
            {
                test.save(sFilename.c_str());
            }
            catch(CImgIOException &cioe)
            {
                cout << cioe.what() << endl;
                mkdirp(fileDir);
                test.save(sFilename.c_str());
            }
        }
};

class ImageSolver
{
    public:
        LoadingBar loadBar;
        vector<string> filenames;
        ImageSolver(string sFolder, function_container vIf)
        {
            this->solve(sFolder, vIf);
        }
        ImageSolver() { }
        void solve(string sFolder, function_container vIf)
        {
            int iTotalIterations;
            double dProgress = 0;
            double dStepSize;

            try{
                this->filenames = getFilesInFolder(sFolder);
            }
            catch(const file_IO::DirNotFound& f)
            {
                cout << f.what() << endl;
                exit(EXIT_FAILURE);
            }

            iTotalIterations = filenames.size() * vIf.size();
            dStepSize = ((double)100 / iTotalIterations);
            int iTimeRemaining = filenames.size() * vIf.size();
            loadBar.initialize(iTotalIterations);

            for(vector<string>::iterator it = filenames.begin();
                it != filenames.end();
                it++)
            {
                cimg::exception_mode(0);
                image_fmt image;
                try {
                image.load((*it).c_str());
                }
                catch(CImgIOException cioe)
                {
                    cout << cioe.what() << endl;
                    loadBar.increaseProgress(vIf.size() - 1);
                    cout << loadBar << endl;
                    continue;
                }
                ImageProcess<double> ipImage(image, (*it).c_str(), .9);
                for (function_container::iterator subIt = vIf.begin();
                    subIt != vIf.end();
                    ++subIt)
                {
                    string sImageDir = DATA_DIR + (*subIt).sPath + "/image/";
                    ipImage.solve((*subIt).func);
                    cout << loadBar << endl;
                    ipImage.writeResultToFile((*subIt).sPath);
                    ipImage.writeImageToFile(sImageDir.c_str());
                }                
            }
            // CImgList<double> images = processImages(filenames, vIf, loadBar, dScalar, resolve);
            //make it so that process images do not write .... just solve and return
            cout << loadBar << endl;// if last image has errors, this might be necessary
            // saveImages(filenames, images, vIf);
            
        }
};


void calculateAverage(string sFilePath)
{
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


} /* EndOfNameSpace */

#endif
