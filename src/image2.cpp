#include "image2.hpp"

#include <cstdarg>
#include <iostream>
#include <iomanip>
#include <list>
#include <map>
#include <sstream> 
#include <string>

#include "CImg.h"

#include "loginstance.hpp"
#include "loadingbar.hpp"
#include "iterative_solvers.hpp"
#include "file.hpp"
#include "plot.hpp"

using namespace cimg_library;
using namespace file_IO;
using namespace plot;
using namespace logging;
using namespace loadbar;
using namespace pe_solver;

namespace image_psb
{

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


std::string format(const char* fmt, ...)
{
    int size = 512;
    char* buffer = 0;
    buffer = new char[size];
    va_list vl;
    va_start(vl, fmt);
    int nsize = vsnprintf(buffer, size, fmt, vl);
    if(size<=nsize)
    {
        delete[] buffer;
        buffer = 0;
        buffer = new char[nsize+1]; //+1 for /0
        nsize = vsnprintf(buffer, size, fmt, vl);
    }
    std::string ret(buffer);
    va_end(vl);
    delete[] buffer;
    return ret;
}



std::string printImage(const image_fmt image)
{
    std::stringstream ss;
    for(int iPos = 0; iPos < image.height(); iPos++)
    {
        for(int jPos = 0; jPos < image.width(); jPos++)
        {
            ss << format("%5.1f  ", image(jPos, iPos));
            // printf("%5.1f ",image(jPos,iPos));
        }
        ss << "\n";
    }

    return ss.str();
}


class ImageException: public std::exception
{
    public:
        explicit ImageException(const std::string& message):
            msg_(message)
         {}

        virtual ~ImageException() throw (){}
        virtual const char* what() const throw()
        {
            std::string ret = std::string("") + msg_;
            return ret.c_str();
        }
    protected:
        std::string msg_;
};


void toGrayScale(image_fmt &image)
{
    if(image.spectrum() == 1)
    {
        return;
    }
    image_fmt grayscale(image.width(), image.height(),1, 1, 0);

        // image = image.get_sRGBtoRGB();
        // image.sRGBtoRGB();
        grayscale = image.get_norm().normalize(0,255);
        // grayscale = image.get_sRGBtoRGB().RGBtoHSI().get_channel(2);
        // grayscale = sRGBtoGrayscale();

    image = grayscale;
}


template <class T> class ImageProcess 
{
    private:
    typedef std::map<iterative_function,std::vector<std::string> > mapfuncres;
        image_fmt image;
        const char *fileName;
        mapfuncres mapFuncRes;
        std::vector<std::string> vOutput;
        image_fmt U;
        image_fmt rho;
        
        image_fmt sRGBtoGrayscale()
        {
            return this->image.get_norm();
        }

            
    public:
        image_fmt getGuess() { return this->U; }
        image_fmt getImage () { return this->image; }
        image_fmt getRho() { return this->rho; }

        void multiply(double dScalar)
        {
            cimg_forXY(this->rho, x,y)
                this->rho(x,y) = this->rho(x,y) * dScalar;
            LOG(severity_type::info)("Multiplied rho by ", dScalar);
            CLOG(severity_type::info)("Multiplied rho by ", dScalar);
        }

        double dMaxErr;
        ImageProcess(image_fmt image, const char *fileName, double dMaxErr)
                : fileName(fileName), dMaxErr(dMaxErr),
                  image(image)

        {
        }

        void clearImages()
        {
            this->U.clear();
            this->vOutput.clear();
        }
        void makeInitialGuess(bool bExtractBordes = true)
        {
            if(this->image.size() < 1) 
            { 
                throw ImageException("Image std::vector was not initialized"
                                     " before creating border");
            }

            int BORDER_SIZE = 1;
            int DEFAULT_GUESS_VAL = 0;
            this->U.assign(this->image, "xyz", DEFAULT_GUESS_VAL);

            if(bExtractBordes == true)
            {
                cimg_for_borderXY(this->image,x,y,BORDER_SIZE)
                {
                    this->U(x,y) = this->image(x,y); 
                }
            }
        }

        void makeRho()
        {
            int iKernDim = 3;
            // int BORDER_SIZE = 1;
            // image_fmt kernel(iKernDim, iKernDim, 1, 1,
            //                  0,1,0,
            //                  1,-4,1,
            //                  0,1,0);
            // this->rho = this->image.get_convolve(kernel, 0);

            this->rho.assign(this->image, "xyz", 0);
            int BORDER_SIZE = 1;
            CImg_3x3(I,double);
            cimg_for_in3x3(this->image, BORDER_SIZE, BORDER_SIZE,
                           this->image.width() - BORDER_SIZE - 1, this->image.height() - BORDER_SIZE - 1,
                           x,y,0,0,I,double) // uses Neumann borders
            {
                double dNewVal = (Icn + Icp + Ipc + Inc - (4 * Icc));
                this->rho(x,y) = dNewVal;
            }

            cimg_for_borderXY(this->image,x,y,BORDER_SIZE)
            {
                // this->rho(x,y) = this->image(x,y); 
                this->rho(x,y) = NAN;
            }
        }

        void solve(iterative_function func,logging::Logger< logging::FileLogPolicy > &logInstance)
       {
            this->vOutput =  iterative_solve(func,
                                       this->image, this->U, this->rho,
                                       this->dMaxErr, this->image.width(),
                                       logInstance);

            if(this->vOutput.size() < 1) 
            {
                throw ImageException("Solver had no results from function");
            }
       }

        void computeLine(const char *fileDir, int iRow = -1)
        {
            // if(iRow < 0) { iRow = this->iHeight / 2; }
            // int iRowStart = iRow * this->iWidth;
            // std::vector<std::string> vOrigImage, vNewImage;
            // for(int iPos = iRowStart; iPos < iRowStart + this->iWidth; iPos++)
            // {
            //     vOrigImage.push_back(std::to_std::string(this->image_vec[iPos]));
            //     vNewImage.push_back(std::to_std::string(this->U[iPos]));
            // }
            // std::string sFileName = string(this->fileName);
            // trimLeadingFileName(sFileName);
            // std::string sOrigFile = "orig" + sFileName,
            //        sNewImage = "new" + sFileName;
            //
            // try{
            //     writeToFile(vOrigImage, sOrigFile, fileDir);
            //     writeToFile(vNewImage,  sNewImage, fileDir);
            // }
            // catch(...)
            // {
            //     throw ImageException("could not write result file to " + std::string(fileDir));
            // }
        }

        void writeInitdata(std::string filename)
        {
            // DO_IF_LOGLEVEL(severity_type::extensive)
            // {
            //     std::string sDir = LOG_DIR + "/init/";
            //     mkdirp(sDir);
            //     std::string sAscii = sDir + filename + "RHO.txt";
            //     this->rho.get_round(2)./*get_crop(1,1,0,0,rho.width() - 1, rho.height() - 1, rho.depth(), rho.spectrum()).*/save_ascii(sAscii.c_str());
            //     std::string sImage = filename + "IMAGE.txt";
            //     this->image.get_round(2).save_ascii(sImage.c_str());
            //     std::string sInitGuess = filename + "GUESS.txt";
            //     this->U.get_round(2).save_ascii(sInitGuess.c_str());
            // }
        }

        void writeResultToFile(std::string fileDir)
        {
            std::string sFilename = std::string(this->fileName);
            try{
            writeToFile(this->vOutput, sFilename, fileDir);
            }
            catch(...)
            {
                throw ImageException("could not write result file to " + fileDir);
            }
        }

        void roundValues()
        {
            this->U.round(0);
            this->U.cut(0, 255);
        }
        void writeImageToFile(const char *fileDir)
        {
            std::string sFileName = std::string(this->fileName);
            trimLeadingFileName(sFileName);
            std::string sFilename = std::string(fileDir) + "/" + sFileName;
            cimg::exception_mode(0);
            try
            {
                mkdirp(fileDir);
                this->U.save(sFilename.c_str());
                // this->U.get_normalize(0,255).save(sFilename.c_str());
            }
            catch(CImgIOException &cioe)
            {
                CLOG(severity_type::error)(cioe.what());
                LOG(severity_type::error)(cioe.what());
            }

        }
};


ImageSolver::ImageSolver() {}
void ImageSolver::addFolder(std::string sFolder, const char *errMsg)
{
    try{
        getFilesInFolder(sFolder, this->filenames);
    }
    catch(const file_IO::DirNotFound& f)
    {
        CLOG(severity_type::error)(f.what());
        LOG(severity_type::error)(f.what());
        exit(EXIT_FAILURE);
    }
}

void ImageSolver::renderImages(std::string sImageRoot, function_container vIf,
                               const char *cImagePath,
                               const char *cResolved)
{
        if(this->filenames.size() < 1) 
        { 
            LOG(severity_type::error)("No files added!");
            CLOG(severity_type::error)("No files added!");
            exit(EXIT_FAILURE);
        }

        std::vector<std::string>::const_iterator it = this->filenames.begin();

        map_gallery mapFiles;

        cimg::exception_mode(0);
        for(std::vector<std::string>::iterator it = filenames.begin();
            it != filenames.end();
            it++)
        {
            std::string s = (*it);
            trimLeadingFileName(s);
            std::vector<std::string> vOutputs;
            mapFiles[s] = vOutputs;
        }

        this->filenames.clear();

        for (function_container::iterator subIt = vIf.begin();
            subIt != vIf.end();
            ++subIt)
        {
            std::string sImageDir = DATA_DIR + (*subIt).sPath + std::string(cImagePath);
            this->addFolder(sImageDir,
                    "when trying to show rendered images (-c flag)");
        }

        for(std::vector<std::string>::iterator it = filenames.begin();
            it != filenames.end();
            it++)
        {
            std::string s = (*it);
            trimLeadingFileName(s);
            mapFiles[s].push_back(*it);
        }
        // if(strcmp(cResolved, "NOT") != 0)
        // {
        //     for (function_container::iterator subIt = vIf.begin();
        //         subIt != vIf.end();
        //         ++subIt)
        //     {
        //         std::string sImageDir = DATA_DIR + (*subIt).sPath + std::string(cImagePath);
        //         this->addFolder(DATA_DIR + cResolved);
        //     }
        // }

        doImageDisplay(mapFiles, sImageRoot);

}

bool ImageSolver::loadImage(std::string sFileDest, image_fmt &image)
{
    cimg::exception_mode(0);
    try {
        image.load(sFileDest.c_str());
        LOG(severity_type::debug)("Loaded image ", sFileDest,
                                  "to code format at ", &image);
        CLOG(severity_type::debug)("Loaded image ", sFileDest,
                                  "to code format at ", &image);
        toGrayScale(image);
        LOG(severity_type::debug)("Converted ", sFileDest, " to grayscale");
        CLOG(severity_type::debug)("Converted ", sFileDest, " to grayscale");
    }
    catch(CImgIOException cioe)
    {
        std::cout << cioe.what() << std::endl;
        return false;
    }

    return true;
}

void ImageSolver::clearFolders()
{
    this->filenames.clear();
}

class ImageContainer
{
    private:
    image_fmt mainImage;
    std::vector<image_fmt> vSolvedImages,
                      vResolvedImages;
    const char *fileName;
    public:
    ImageContainer(image_fmt, const char *arg);
    
    void addSolverImage(std::string, image_fmt);
    void addReSolvedImage(std::string, image_fmt);

    void next();
    void prev();
};

ImageContainer::ImageContainer(image_fmt image, const char *fileName)
               : mainImage(image), fileName(fileName)
{
}

void ImageContainer::addSolverImage(std::string sMainImage, image_fmt image)
{
}

void ImageContainer::addReSolvedImage(std::string sMainImage, image_fmt image)
{
}

class ImageDisplay
{
    private:
    std::vector<ImageContainer> vMainImages;
    int iIndex;
    public:
    ImageDisplay();
    void addMainImage(image_fmt, std::string);
    void nextImage();
    void prevImage();
    void computeLine();
};

ImageDisplay::ImageDisplay()
{
    int iIndex = 0;
}

void ImageDisplay::addMainImage(image_fmt image, std::string sFilename)
{
    ImageContainer ic(image, sFilename.c_str());
    this->vMainImages.push_back(ic);
}

void ImageSolver::doImageDisplay(map_gallery &mapFiles, std::string sImageRoot)
{
    map_gallery::iterator it = mapFiles.begin();

    std::vector<std::string> out = it->second;
    std::string mainFile = it->first;
    image_fmt main_image, solved_image;
    int iIndex = 0;
    int iImageIndex = 0;

    // class container with an image and a list of children
    // constructor initialies
    // the next-> prev 

    std::string sImageDest = sImageRoot + it->first;
    if(!loadImage(sImageDest, main_image) || !loadImage(out[iIndex], solved_image)) return;

    const double blackWhite[] = {255};
    CImgDisplay main_disp(main_image, mainFile.c_str() ,0);	
    CImgDisplay mask_disp(solved_image, out[iIndex].c_str() ,0);	
    // main_disp.resize();
    // mask_disp.resize();
    image_fmt visu(500, 300, 1, 1, 0);
    // image_fmt visu(main_disp);
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
                if(iImageIndex == mapFiles.size() - 1) {it = mapFiles.begin(); iImageIndex = 0;}
                else{ iImageIndex++ ; it++; }

                mainFile = it->first;
                out = it->second;
                

                sImageDest = sImageRoot + mainFile;
                if(!loadImage(sImageDest, main_image) || !loadImage(out[iIndex], solved_image)) { break; }
                main_disp = main_image;
                main_disp.set_title(mainFile.c_str());
                mask_disp = solved_image;
                mask_disp.set_title(out[iIndex].c_str());
                // main_disp.resize();
                // mask_disp.resize();
                break;
            case cimg::keyARROWDOWN:
                if(it == mapFiles.begin()) {it = mapFiles.end(); it--; iImageIndex = mapFiles.size() - 1;}
                else{ it--; iImageIndex--; }

                mainFile = it->first;
                out = it->second;

                sImageDest = sImageRoot + mainFile;
                if(!loadImage(sImageDest, main_image) || !loadImage(out[iIndex], solved_image)) { it--; break; }
                main_disp = main_image;
                main_disp.set_title(mainFile.c_str());
                mask_disp = solved_image;
                mask_disp.set_title(out[iIndex].c_str());
                // main_disp.resize();
                // mask_disp.resize();
                break;
            case cimg::keyARROWLEFT:
                if(iIndex == 0) { iIndex = out.size(); }
                iIndex--;
                if(!loadImage(out[iIndex], solved_image)) {break; }
                mask_disp = solved_image;
                mask_disp.set_title(out[iIndex].c_str());
                // mask_disp.resize();
                break;
            case cimg::keyARROWRIGHT:
                if(iIndex == out.size() - 1) { iIndex = -1; }
                iIndex++;
                if(!loadImage(out[iIndex], solved_image)) {break; }
                mask_disp = solved_image;
                mask_disp.set_title(out[iIndex].c_str());
                // mask_disp.resize();
                break;
            case cimg::keyQ:
                return;
        }
        main_disp.wait();

    }

}

imageList_fmt ImageSolver::histogram(std::string sDir, function_container vIf)
{

    // for (function_container::iterator subIt = vIf.begin();
    //     subIt != vIf.end();
    //     ++subIt)
    // {
    image_fmt bigImg;
    imageList_fmt images;
    cimg::exception_mode(0);

    for(std::vector<std::string>::iterator it = filenames.begin();
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
        std::string sImageDir = DATA_DIR + (*it).sPath + std::string("image/");
        this->addFolder(sImageDir);
        image_fmt appender;

        for(std::vector<std::string>::iterator subIt = filenames.begin();
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

void ImageSolver::solve(function_container vIf, bool bComputeLines,
            double dTolerance, const char *cImagePath, const char *cResultPath
           , double dScalar)
{
    if(this->filenames.size() < 1
    || vIf.size() < 1) {
        LOG(severity_type::error)("Solve called without files (or functions?)");
        CLOG(severity_type::error)("Solve called without files (or functions?)");
        return;
    }

    int iTotalIterations;
    double dProgress = 0;
    double dStepSize;

    iTotalIterations = this->filenames.size() * vIf.size();
    dStepSize = ((double)100 / iTotalIterations);
    int iTimeRemaining = filenames.size() * vIf.size();
    loadBar.initialize(iTotalIterations);
    for(std::vector<std::string>::iterator it = filenames.begin();
        it != filenames.end();
        it++)
    {
        cimg::exception_mode(0);
        image_fmt image;
        if(!loadImage((*it), image)) 
        {
            loadBar.increaseProgress(vIf.size() - 1);
            std::cout << loadBar << std::endl;
            continue;
        }

        ImageProcess<double> ipImage(image, (*it).c_str(), dTolerance);
        // toGrayScale(ipImage.getImage());
        ipImage.makeRho();
        if(dScalar != 1.0) { ipImage.multiply(dScalar); }

        bool BORDERS=true;

        for (function_container::iterator subIt = vIf.begin();
            subIt != vIf.end();
            ++subIt)
        {
            std::string sRedo = "";
            if(dScalar != 1.0)
                sRedo = "re";

            std::string sImageDir = DATA_DIR + (*subIt).sPath + std::string(cImagePath);
            std::string sLogPath = LOG_DIR + (*subIt).sPath;
            std::string sLogFile = sLogPath + (*it);
            trimTrailingFilename(sLogFile);
            trimLeadingFileName(sLogFile);
            sLogFile = sRedo + sLogFile + ".log";
            mkdirp(sLogPath.c_str());


            logging::Logger< logging::FileLogPolicy > logInstance(sLogPath + sLogFile);
            logInstance.setLevel(log_inst.getLevel());

            ipImage.makeInitialGuess(BORDERS);
            if(logInstance.getLevel() >= severity_type::extensive)
            {
                (logInstance.print<severity_type::extensive>)("Initial image: ", (*it).c_str(), "\n", printImage(ipImage.getImage()));
                (logInstance.print<severity_type::extensive>)("Initial guess\n", printImage(ipImage.getGuess()));
                (logInstance.print<severity_type::extensive>)("Initial rho\n", printImage(ipImage.getRho()));
            }
            if(CGETLEVEL >= severity_type::extensive)
            {
                CLOG(severity_type::extensive)("Initial image\n", printImage(ipImage.getImage()));
                CLOG(severity_type::extensive)("Initial guess\n", printImage(ipImage.getGuess()));
                CLOG(severity_type::extensive)("Initial rho\n", printImage(ipImage.getRho()));
            }
            (logInstance.print<severity_type::info>)("Beginning solver: ", (*subIt).sPath);
            CLOG(severity_type::info)("Beginning solver: ", (*subIt).sPath);
            CLOG(severity_type::info)("Beginning solver: ", (*subIt).sPath, " for image ", (*it));
            LOG(severity_type::info)("Beginning solver: ", (*subIt).sPath, " for image ", (*it));

            try{

                ipImage.solve((*subIt).func, logInstance);
                CLOG(severity_type::extensive)("Finished image: \n", printImage(ipImage.getGuess()));
                (logInstance.print<severity_type::extensive>)("Finished image: \n", printImage(ipImage.getGuess()));
                std::cout << loadBar << std::endl;

                if(bComputeLines) { ipImage.computeLine("lines"); }
                ipImage.writeResultToFile(std::string(cResultPath) + (*subIt).sPath);

                ipImage.roundValues();
                CLOG(severity_type::extensive)("Finished image (rounded and cut): \n", printImage(ipImage.getGuess()));
                (logInstance.print<severity_type::extensive>)("Finished image(rounded and cut): \n", printImage(ipImage.getGuess()));
                ipImage.writeImageToFile(sImageDir.c_str());
                ipImage.clearImages();
            }
            catch(ImageException &ie)
            {
                LOG(severity_type::warning)(ie.what());
                CLOG(severity_type::warning)(ie.what());
            }
        }                

    }
    std::cout << loadBar << std::endl;// if last image has errors, this might be necessary
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
    //     LOG(severity_type::error)(f.what());
    //     CLOG(severity_type::error)(f.what());
    // }
    //
    // std::vector<double> average; // can give undererror
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

} /* EndOfNameSpace */
