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

void renderImage(CImgDisplay disp)
{
    disp.show();
    while(!disp.is_closed() && ! disp.is_keyQ() )
    {
        disp.wait();
    }
}

void display_histogram(image_fmt image)
{
    image.display_graph("Histogram", 3);
}


void printImage(image_fmt image)
{
    for(int iPos = 0; iPos < image.height(); iPos++)
    {
        for(int jPos = 0; jPos < image.width(); jPos++)
        {
            printf("%5.1f ",image(jPos,iPos));
            // cout << image(jPos, iPos) << " ";
        }
        cout << endl;
    }
}


class ImageException: public exception
{
    public:
        explicit ImageException(const std::string& message):
            msg_(message)
         {}

        virtual ~ImageException() throw (){}
        virtual const char* what() const throw()
        {
            string ret = string("") + msg_;
            return ret.c_str();
        }
    protected:
        std::string msg_;
};


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

    double dScalar;
        
        image_fmt sRGBtoGrayscale()
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
            image_fmt grayscale(this->image.width(), this->image.height(),1, 1, 0);

            if(this->image.spectrum() == 1)
            {
               grayscale = this->image;
            }
            else
            {
                grayscale = this->image.RGBtoHSI().get_channel(2);
                // grayscale = sRGBtoGrayscale();
            }

            this->image_vec.reserve(this->iDim);

            if(this->dScalar == 1.0)
            {
                for(int yPos = 0; yPos < grayscale.height(); yPos++)
                {
                    for(int xPos = 0; xPos < grayscale.width(); xPos++)
                    {
                        double newVal = (double)grayscale(xPos, yPos, 0, 0) * this->dScalar;
                        this->image_vec.push_back(newVal);
                    }
                }
            }

        }
        vector<string> vOutput;
        d1 U;
            
    public:
        d1 getGuess() { return this->U; }


        double dMaxErr;
        ImageProcess(image_fmt image, const char *fileName, double dMaxErr,
                     double dScalar)
                : fileName(fileName), dMaxErr(dMaxErr),
                  image(image), dScalar(dScalar)

        {
            this->iHeight = image.height();
            this->iWidth = image.width();
            this->iSpectrum = image.spectrum();
            this->iDepth = image.depth();
            this->iDim = image.height() * image.width();

            this->convertImage();
        }

        void clearFuncVectors()
        {
            this->vOutput.clear();
            this->U.clear();
        }
        void makeInitialGuess(bool bExtractBordes = true)
        {
            if(this->image_vec.size() < 1) 
            { 
                throw ImageException("Image vector was not initialized before creating border");
            }

           U.resize(iDim, 0);

           if(!bExtractBordes) { return ; }
           
           /* Upper border */
           for(int iPos = 0; iPos < this->iWidth; iPos++) 
            {
                U[iPos] = this->image_vec[iPos];                                
            }

           /* Bottom border */
           for(int iPos = (int)this->image_vec.size() - iWidth;
                iPos < this->image_vec.size();
                iPos++) 
            {
                U[iPos] = this->image_vec[iPos];
            }

           /* Left border */
           for(int iPos = 0;
                iPos < (int)this->image_vec.size();
                iPos+= this->iWidth) 
            {
                U[iPos] = this->image_vec[iPos];
            }

           /* Right border */
           for(int iPos = this->iWidth - 1;
                iPos < (int)this->image_vec.size();
                iPos+= this->iWidth) 
            {
                U[iPos] = this->image_vec[iPos];
            }

        }

        void solve(iterative_function func)
       {
           d1 rho = computeFieldRho(this->image_vec, this->U, this->iWidth);
            this->vOutput =  iterative_solve(func,
                                       this->image_vec, this->U, rho,
                                       this->dMaxErr, this->iWidth);

            if(this->vOutput.size() < 1) 
            {
                throw ImageException("Solver had no results from function");
            }
       }

        void computeLine(const char *fileDir, int iRow = -1)
        {
            if(iRow < 0) { iRow = this->iHeight / 2; }
            int iRowStart = iRow * this->iWidth;
            vector<string> vOrigImage, vNewImage;
            for(int iPos = iRowStart; iPos < iRowStart + this->iWidth; iPos++)
            {
                vOrigImage.push_back(std::to_string(this->image_vec[iPos]));
                vNewImage.push_back(std::to_string(this->U[iPos]));
            }
            string sFileName = string(this->fileName);
            trimLeadingFileName(sFileName);
            string sOrigFile = "orig" + sFileName,
                   sNewImage = "new" + sFileName;

            try{
                writeToFile(vOrigImage, sOrigFile, fileDir);
                writeToFile(vNewImage,  sNewImage, fileDir);
            }
            catch(...)
            {
                throw ImageException("could not write result file to " + string(fileDir));
            }
        }

        void writeResultToFile(string fileDir)
        {
            string sFilename = string(this->fileName);
            try{
            writeToFile(this->vOutput, sFilename, fileDir);
            }
            catch(...)
            {
                throw ImageException("could not write result file to " + fileDir);
            }
        }
        void writeImageToFile(const char *fileDir)
        {
            double *dImage = new double[this->U.size()];
            string sFileName = string(this->fileName);
            trimLeadingFileName(sFileName);
            string sFilename = string(fileDir) + "/" + sFileName;

            for(vector<int>::size_type iPos = 0;
                    iPos < this->U.size();
                    iPos++) 
            {
                dImage[iPos] = abs(U[iPos]);
            }

            cimg::exception_mode(0);
            CImg<double> test(dImage, iWidth, iHeight,
                              1, 1, false);
            printImage(test);
            delete dImage;
            try
            {
                // test.get_normalize(0,255).save(sFilename.c_str());
                // test.get_quantize(256).save(sFilename.c_str());
                mkdirp(fileDir);
                test.save(sFilename.c_str());
            }
            catch(CImgIOException &cioe)
            {
                cout << cioe.what() << endl;
                test.save(sFilename.c_str());
            }

        }
};

class ImageSolver
{
    private:
        static constexpr int NEXT_IMAGE = 0, PREV_IMAGE = 1, NEXT_SOLVER = 2, PREV_SOLVER = 3,
                EXIT = 10;
        typedef map<string, vector<string> > map_gallery;

    public:
        LoadingBar loadBar;
        vector<string> filenames;
        void addFolder(string sFolder, const char *errMsg = "")
        {
            try{
                getFilesInFolder(sFolder, this->filenames);
            }
            catch(const file_IO::DirNotFound& f)
            {
                cout << f.what() << " " << errMsg << endl;
                exit(EXIT_FAILURE);
            }
        }

        ImageSolver() { }


        void renderImages(string sImageRoot, function_container vIf, const char *cImagePath = "/image/")
        {
            if(this->filenames.size() < 1) 
            { 
                cout << "No files added!" << endl;
                exit(EXIT_FAILURE); 
            }

            // sort the list of filenames
            vector<string>::const_iterator it = this->filenames.begin();

            map_gallery mapFiles;

            cimg::exception_mode(0);
            for(vector<string>::iterator it = filenames.begin();
                it != filenames.end();
                it++)
            {
                string s = (*it);
                trimLeadingFileName(s);
                vector<string> vOutputs;
                mapFiles[s] = vOutputs;
            }

            this->filenames.clear();

            for (function_container::iterator subIt = vIf.begin();
                subIt != vIf.end();
                ++subIt)
            {
                string sImageDir = DATA_DIR + (*subIt).sPath + string(cImagePath);
                this->addFolder(sImageDir, "when trying to show rendered images (-c flag)");
            }

            for(vector<string>::iterator it = filenames.begin();
                it != filenames.end();
                it++)
            {
                string s = (*it);
                trimLeadingFileName(s);
                mapFiles[s].push_back(*it);
            }

            doImageDisplay(mapFiles, sImageRoot);

        }

        bool loadImage(string sFileDest, image_fmt &image)
        {
            cimg::exception_mode(0);
            try {
                image.load(sFileDest.c_str());
            }
            catch(CImgIOException cioe)
            {
                cout << cioe.what() << endl;
                return false;
            }

            return true;
        }

        void clearFolders()
        {
            this->filenames.clear();
        }

        void doImageDisplay(map_gallery &mapFiles, string sImageRoot)
        {
            map_gallery::iterator it = mapFiles.begin();


            vector<string> out = it->second;
            string mainFile = it->first;
            image_fmt main_image, solved_image;
            int iIndex = 0;
            int iImageIndex = 0;

            string sImageDest = sImageRoot + it->first;
            if(!loadImage(sImageDest, main_image) || !loadImage(out[iIndex], solved_image)) return;

            const double blackWhite[] = {255};
            image_fmt visu(500, 400, 1, 1, 0);
            CImgDisplay main_disp(main_image, mainFile.c_str() ,0);	
            CImgDisplay mask_disp(solved_image, out[iIndex].c_str() ,0);	
            CImgDisplay graph_disp(visu, "Color intensities" ,0);	

            while (!main_disp.is_closed() && !mask_disp.is_closed() && !graph_disp.is_closed())
            {
                if (main_disp.button() && main_disp.mouse_y()>=0) 
                {
                    const int yPos = main_disp.mouse_y();
                    image_fmt main_cropped =  main_image.get_crop(
                            0, yPos, 0, 0, main_image.width()-1, yPos, 0, 0);
                    image_fmt side_cropped = solved_image.get_crop(
                            0, yPos, 0, 0, solved_image.width()-1, yPos, 0, 0);

                    // data, color, opacity, plot_type, verttex_type, ymin
                    visu.fill(0).draw_graph(main_cropped, blackWhite, 1, 1, 0, 255, 0);
                    visu.draw_graph(side_cropped, blackWhite, 1, 1, 0, 255, 0);
                    visu.display(graph_disp);
                }
                switch (main_disp.key()) 
                {
                    case cimg::keyARROWUP:
                        mainFile = it->first;
                        out = it->second;
                        
                        if(iImageIndex == mapFiles.size() - 1) {it = mapFiles.begin(); iImageIndex = 0;}
                        else{ iImageIndex++ ; it++; }

                        sImageDest = sImageRoot + mainFile;
                        if(!loadImage(sImageDest, main_image) || !loadImage(out[iIndex], solved_image)) { break; }
                        main_disp = main_image;
                        main_disp.set_title(mainFile.c_str());
                        mask_disp = solved_image;
                        mask_disp.set_title(out[iIndex].c_str());
                        break;
                    case cimg::keyARROWDOWN:
                        mainFile = it->first;
                        out = it->second;
                        
                        if(iImageIndex == 0) {it = mapFiles.end(); iImageIndex = mapFiles.size() - 1 ;}
                        else{ iImageIndex-- ; }
                        it--;

                        sImageDest = sImageRoot + it->first;
                        if(!loadImage(sImageDest, main_image) || !loadImage(out[iIndex], solved_image)) { it--; break; }
                        main_disp = main_image;
                        main_disp.set_title(mainFile.c_str());
                        mask_disp = solved_image;
                        mask_disp.set_title(out[iIndex].c_str());
                        break;
                    case cimg::keyARROWLEFT:
                        if(iIndex == 0) { iIndex = out.size(); }
                        iIndex--;
                        if(!loadImage(out[iIndex], solved_image)) {break; }
                        mask_disp = solved_image;
                        mask_disp.set_title(out[iIndex].c_str());
                        break;
                    case cimg::keyARROWRIGHT:
                        if(iIndex == out.size() - 1) { iIndex = -1; }
                        iIndex++;
                        if(!loadImage(out[iIndex], solved_image)) {break; }
                        mask_disp = solved_image;
                        mask_disp.set_title(out[iIndex].c_str());
                        break;
                    case cimg::keyQ:
                        return;
                }
                main_disp.wait();

            }

        }

        imageList_fmt histogram(string sDir, function_container vIf)
        {

            // for (function_container::iterator subIt = vIf.begin();
            //     subIt != vIf.end();
            //     ++subIt)
            // {
            image_fmt bigImg;
            imageList_fmt images;
            cimg::exception_mode(0);

            for(vector<string>::iterator it = filenames.begin();
                it != filenames.end();
                it++)
            {
                image_fmt image;
                if(!loadImage((*it), image)) 
                {
                    continue;
                }
                bigImg.append(image, 'x');
            }
            images.push_back(bigImg.histogram(256));


            for (function_container::iterator it = vIf.begin();
                it != vIf.end();
                ++it)
            {
                this->filenames.clear();
                string sImageDir = DATA_DIR + (*it).sPath + string("image/");
                this->addFolder(sImageDir);
                image_fmt appender;

                for(vector<string>::iterator subIt = filenames.begin();
                    subIt != filenames.end();
                    subIt++)
                {
                    image_fmt image;
                    if(!loadImage((*subIt), image)) 
                    {
                        continue;
                    }
                    appender.append(image, 'x');
                }
                images.push_back(appender.histogram(256));
            }

            return images;
        }

        void solve(function_container vIf, bool bComputeLines = false,
                   const char *cImagePath = "/image/", const char *cResultPath = "/./"
                   , double dScalar = 1)
        {
            if(this->filenames.size() < 1
            || vIf.size() < 1) {
                cout << "Solve called without files (or functions?)" << endl;
                return;
            }

            int iTotalIterations;
            double dProgress = 0;
            double dStepSize;

            iTotalIterations = this->filenames.size() * vIf.size();
            dStepSize = ((double)100 / iTotalIterations);
            int iTimeRemaining = filenames.size() * vIf.size();
            loadBar.initialize(iTotalIterations);

            for(vector<string>::iterator it = filenames.begin();
                it != filenames.end();
                it++)
            {
                cimg::exception_mode(0);
                image_fmt image;
                if(!loadImage((*it), image)) 
                {
                    loadBar.increaseProgress(vIf.size() - 1);
                    cout << loadBar << endl;
                    continue;
                }

                double ERROR_TOLERANCE = 0.01;
                ImageProcess<double> ipImage(image, (*it).c_str(), ERROR_TOLERANCE, dScalar);
                cout << "Beginning image " << (*it) << endl;

                for (function_container::iterator subIt = vIf.begin();
                    subIt != vIf.end();
                    ++subIt)
                {
                    string sImageDir = DATA_DIR + (*subIt).sPath + string(cImagePath);
                    try{
                        ipImage.makeInitialGuess(true);
                        ipImage.solve((*subIt).func);
                        cout << loadBar << endl;
                        if(bComputeLines) { ipImage.computeLine("lines"); }
                        ipImage.writeResultToFile(string(cResultPath) + (*subIt).sPath);
                        ipImage.writeImageToFile(sImageDir.c_str());
                        ipImage.clearFuncVectors();
                    }
                    catch(ImageException &ie)
                    {
                        cout << ie.what() << endl;
                    }
                }                
            }
            cout << loadBar << endl;// if last image has errors, this might be necessary
        }
};


void calculateAverage(string sFilePath)
{
    vector<string> files;

    string sReadFolder = DATA_DIR + sFilePath;

    try
    {
        getFilesInFolder(sReadFolder.c_str(), files);
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
