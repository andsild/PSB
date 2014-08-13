#ifndef IMAGE_DISPLAY_H
#define IMAGE_DISPLAY_H

#include <string>

#include "CImg.h"

#include "image_types.hpp"
#include <vector>

namespace image_display
{

template <class baseIter>
class circularIterator
{
    private:
       baseIter cur;
        baseIter begin;
        baseIter end;
    public:
        circularIterator() {};
        void setIterator(baseIter b, baseIter e, baseIter i)
        {
            this->begin = b;
            this->end = e;
            this->cur = i;
        }
        circularIterator & operator ++() 
        {
            ++cur;
            if(cur == end)
                cur = begin;
            return cur;
        }
        circularIterator & operator ++(int)
        {
            operator++();
            return cur;
        }
        circularIterator & operator --()
        {
            --cur;
            if(std::distance(begin, cur) == 0)
                cur = end;
            return cur;
        }
        // circularIterator & operator ()(void) {
        //     return cur;
        // }
};

class ImageContainer;
class ImageDisplay
{
    private:
    int iIndex;
    cimg_library::CImgDisplay main_disp,
                solved_disp,
                resolved_disp,
                noised_disp,
                orignoised_disp;
    image_fmt main_image,
              solved_image,
              resolved_image,
              noised_image,
              orig_noised_image;
    image_fmt visu;
    cimg_library::CImgDisplay graph_disp;
    ImageContainer getCurrent();
    void nextImage();
    void prevImage();
    void nextSolver();
    void prevSolver();
    void loadImmy(std::string &arg1, std::string &arg2,
                    std::string &arg3, std::string &arg4, std::string &arg5);
    public:
    std::vector<ImageContainer> vMainImages;
    void loop();
    ImageDisplay();
    void addMainImage(std::string);
    void addResolvedImage2(std::string, std::string, bool, bool, bool);
    void show();
};


void scanAndAddImage(std::string sRootdir, std::string sSolverdir);

} /* EndOfNamespace */

#endif
