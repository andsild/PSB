#ifndef IMAGE_DISPLAY_H
#define IMAGE_DISPLAY_H

#include <string>

#include "CImg.h"

#include "image_types.hpp"
#include <vector>

namespace image_display
{

class ImageContainer
{
    private:
    std::vector<std::string> vSolvedImages,
                      vResolvedImages;
    std::string fileName;
    public:
    static int iSolvedIndex, iResolvedIndex;
    ImageContainer(std::string);
    
    void addSolverImage(std::string);
    void addResolvedImage(std::string);

    bool hasResolvedImages();

    std::string getFileName() const;
    std::string getMain() const;
    std::string getSolved() const;
    std::string getResolved() const;
    std::string getNextSolver();
    std::string getPrevSolver();
    std::string getNextResolver();
    std::string getPrevResolver();
    bool hasResolved() const;
};

class ImageDisplay
{
    private:
    std::vector<ImageContainer> vMainImages;
    int iIndex;
    cimg_library::CImgDisplay main_disp,
                solved_disp,
                resolved_disp;
    image_fmt main_image,
              // solved_image,
              resolved_image;
    cimg_library::CImg<unsigned char> solved_image;
    image_fmt visu;
    cimg_library::CImgDisplay graph_disp;
    ImageContainer getCurrent();
    void nextImage();
    void prevImage();
    void nextSolver();
    void prevSolver();
    void loadImmy(std::string &arg1, std::string &arg2, std::string &arg3);
    public:
    void loop();
    ImageDisplay();
    void addMainImage(std::string);
    void addResolvedImage2(std::string sFilename, std::string sCommon, bool);
    void show();
};


void scanAndAddImage(std::string sRootdir, std::string sSolverdir);

}

#endif
