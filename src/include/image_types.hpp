#ifndef IMAGE_TYPE_H
#define  IMAGE_TYPE_H

#include <string>
#include <vector>

#include "CImg.h"

typedef cimg_library::CImgList<double> imageList_fmt;
typedef cimg_library::CImg<double> image_fmt;

typedef std::vector<double> rawdata_fmt;

typedef void (*iterative_function)(const cimg_library::CImg<double> &arg1,
                                    cimg_library::CImg<double> &arg2,
                                    double, int, double &arg3, double) ;

typedef void (*iterative_func)(const image_fmt &arg1, image_fmt &arg2,
                                double &arg3) ;
typedef void (*direct_func)(const image_fmt &arg1, image_fmt &arg2);

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



#endif
