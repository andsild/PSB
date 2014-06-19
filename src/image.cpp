#ifndef _IMAGE_CPP
#define _IMAGE_CPP   1

#define cimg_debug 0 

#include <fstream>
#include <iostream>
#include <iomanip>
#include <limits>
#include <map>
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
        int iTimeRemaining;

    public:
        struct winsize w;
        LoadingBar(int iNumIterations) : iNumIterations(iNumIterations)
    {
        this->dProgress = 0;
        this->dStepSize = ((double)100 / iNumIterations);
        this->iTimeRemaining = 100;//filenames.size() * vIf.size() 
                             //* (pow( (img.height() * img.width()), 0.3));
    }
    string getTimeLeft() const
    {
        int iMinutesLeft = this->iTimeRemaining / 60;
        int iSecondsLeft = this->iTimeRemaining - (iMinutesLeft * 60);
        return string(to_string(iMinutesLeft) + "m"
                + to_string(iSecondsLeft) + "s");
    }
    void increaseProgress() const
    {
        this->dProgress += this->dStepSize;
    }
    void increaseProgress(int iIterations)
    {
        this->dProgress += this->dStepSize * iIterations;
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
    int iColWidth = obj.w.ws_col - 10; 
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

CImgList<double> readImage(CImg<double> image,
                           const char *fileName, vector<iterative_function> vIf,
                           LoadingBar &loadbar)
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

    //http://sourceforge.net/p/cimg/discussion/334630/thread/6a560357/
    image.sRGBtoRGB();
    CImg<double> grayscale(iWidth, iHeight, 1, 1, 0);
    for(int iPos = 0; iPos < iWidth; iPos++)
    {
        for(int jPos = 0; jPos < iHeight; jPos++)
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

    for(int iPos = 0; iPos < iHeight; iPos++)
    {
        for(int jPos = 0; jPos < iWidth; jPos++)
        {
            image_vec.push_back(grayscale(jPos, iPos, 0, 0));
            //TODO: push back makes it bigger than it should be
            //      (should pre-compute size instead)
        }
    }

    image = CImg<double>(1, 1, 1, 1, 1); // TODO: does this get memory back?
    grayscale = CImg<double>(1, 1, 1, 1, 1); // TODO: does this get memory back?
    U = vector<double>(iDim, 0); // wait to declare this until memory is freed?

    /* Solve */
    for (vector<iterative_function>::iterator it = vIf.begin();
        it != vIf.end();
        ++it)
    {
        double *dImage = new double[image_vec.size()];

        cout << "beginning image " << fileName << endl;
        cout << loadbar << endl;
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

        cout << "\rfinished image " << fileName << endl;
        CImg<double> test(dImage, iWidth, iHeight,
                          1, 1, false);
                          // iDepth, iSpectrum, false);
        retList.push_back(test);
        // delete dImage;
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

void readSingleImage(vector<iterative_function> vIf)
{
    // CImg<unsigned char> image("./media/109201.jpg"); //example
    char cImageName[] = "./small_media/104000.jpg";
    string sDest = get_path() + string(cImageName);
    // CImg<unsigned char> image(sDest.c_str()); //example
    CImg<double> image ;
    try{
        image = getImage(sDest.c_str());
    }
    catch(CImgIOException cioe) {
        cout << cioe.what() << endl;
        exit(EXIT_FAILURE);
    }
    // CImgList<double> cil = readImage(image, cImageName, vIf);
}

/** Read a series of images and solve them
 *
 */
void readFolder(string sDir, vector<iterative_function> vIf)
{
    CImgList<double> images; 
    vector<string> filenames;
    int iTotalIterations;
    double dProgress = 0;
    double dStepSize;


    try{
        filenames = getFilesInFolder(sDir);
    }
    // catch(std::ios_base::failure &f) {
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
            // iTotalIterations -= vIf.size();
            // dStepSize = (100 / iTotalIterations);

            continue;
        }


        /* Returns images for each iteration */
        CImgList<double> cil = readImage(img, filenames[iPos].c_str(),
                                         vIf, loadBar);
        images.insert(cil, images.size());
    }
    cout << loadBar << endl;// if last image has errors, this might be necessary

    map<iterative_function, string> fileLocations;
    fileLocations[iterate_gauss] = "gauss/";
    fileLocations[iterate_jacobi] = "jacobi/";
    fileLocations[iterate_sor] = "sor/";

    // cout << "ERDWAD     " << (int)(images.size() / vIf.size()) << endl;

    for(vector<int>::size_type iPos = 0;
            iPos < (int)(images.size() / vIf.size());
            iPos++) 
    {
        trimLeadingFileName(filenames[iPos]);

        for(vector<int>::size_type jPos = 0;
                jPos < vIf.size();
                jPos++) 
        {
            string sDest = DATA_DIR + fileLocations[vIf[jPos]];
            string sImageDest = get_path() + sDest + "/image/" + filenames[iPos];

            images[iPos+jPos].save(sImageDest.c_str());
            calculateAverage(sDest);
            //FIXME: line above causes segfault
        }
    }
}

void calculateAverage(string sFilePath)
{
    sFilePath += "/";
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

}

#endif 
