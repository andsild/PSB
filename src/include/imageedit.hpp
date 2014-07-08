#ifndef IMAGE_DISPLAY_H
#define IMAGE_DISPLAY_H

#include "CImg.h"

#include "image2.hpp"
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
    // std::string getPrevReSolver();
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
    image_psb::image_fmt main_image,
              solved_image,
              resolved_image;
    image_psb::image_fmt visu;
    cimg_library::CImgDisplay graph_disp;
    ImageContainer getCurrent();
    void nextImage();
    void prevImage();
    void nextSolver();
    void prevSolver();
    public:
    void loop();
    ImageDisplay();
    void addMainImage(std::string);
    void addSolverImage(std::string);
    void addResolvedImage(std::string);
    void computeLine();
    void show();
};

}

#endif
