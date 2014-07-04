#ifndef _IMAGE_H
#define _IMAGE_H   1

#define cimg_debug 0 

#include <iomanip>
#include <list>
#include <map>
#include <sstream> 
#include <string>

#include "CImg.h"
#include "loadingbar.hpp"
#include "iterative_solvers.hpp"

namespace image_psb
{

class SolverMeta
{
    public:
        pe_solver::iterative_function func;
        std::string sPath;
        SolverMeta(pe_solver::iterative_function func,
                          std::string sPath)
            : func(func), sPath(sPath)
        {
        }
};

typedef cimg_library::CImgList<double> imageList_fmt;
typedef std::vector<SolverMeta> function_container;

typedef cimg_library::CImg<double> image_fmt;


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
                         const char *cImagePath = "/image/");
        bool loadImage(std::string, image_fmt &);
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
