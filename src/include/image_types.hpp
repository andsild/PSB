#ifndef IMAGE_TYPE_H
#define  IMAGE_TYPE_H

#include <string>
#include <vector>

#include "CImg.h"

typedef double data_fmt;
typedef cimg_library::CImgList<data_fmt> imageList_fmt;
typedef cimg_library::CImg<data_fmt> image_fmt;


typedef void (*iterative_func)(const image_fmt &arg1, image_fmt &arg2,
                                double &arg3, const int, const int) ;
typedef void (*direct_func)(const image_fmt &arg1, image_fmt &arg2);


typedef std::vector<data_fmt> rawdata_fmt;

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
