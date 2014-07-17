#ifndef _IMAGE_H
#define _IMAGE_H

#include <cstdarg>
#include <iomanip>
#include <list>
#include <map>
#include <sstream> 
#include <string>
#include <vector>

#include "CImg.h"

#include "image_types.hpp"
#include "loadingbar.hpp"

namespace image_psb
{

class SolverMeta
{
    public:
        iterative_function func;
        std::string sPath;
        SolverMeta(iterative_function func,
                          std::string sPath)
            : func(func), sPath(sPath)
        {
        }
};
typedef std::vector<SolverMeta> function_container;


void toGrayScale(image_fmt &arg);
double imageDiff(const image_fmt &arg1, const image_fmt &arg2);
bool readImage(image_fmt &arg1, const std::string);
image_fmt makeRho(const image_fmt &input);
image_fmt makeInitialGuess(const image_fmt &input, bool);
void roundValues(image_fmt &arg);


void renderImage(cimg_library::CImgDisplay);
void display_histogram(image_fmt);
std::string format(const char *arg, ...);
std::string printImage(image_fmt);

class ImageSolver
{
    private:
        static constexpr int NEXT_IMAGE = 0, PREV_IMAGE = 1, NEXT_SOLVER = 2, PREV_SOLVER = 3,
                EXIT = 10;
        typedef std::map<std::string, std::vector<std::string> > map_gallery;

    public:
        loadbar::LoadingBar loadBar;
        std::vector<std::string> filenames;
        ImageSolver();
        void addFolder(std::string, const char *vararg = "");
        void renderImages(std::string, function_container,
                         const char *cImagePath = "/image/",
                         const char *cResolved = "NOT");
        void clearFolders();
        void doImageDisplay(map_gallery &arg, std::string);
        imageList_fmt histogram(std::string, function_container);
        void solve(function_container, bool var1 = false,
                    double var2 = 0.3,
                    const char *var3 = "/image/", const char *arg4 = "/./",
                    double var5 = 1.0);
};



void calculateAverage(std::string sFilePath);
    
} /* EndOfNamespace */

#endif
