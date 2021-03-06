#ifndef FFT_H
#define FFT_H

#include "image_types.hpp"

namespace solver
{
    // void FFT2D(const image_fmt &arg1, image_fmt &arg2);
    void FFT_DCT(const image_fmt &arg1, image_fmt &arg2);
    void FFT_DST(const image_fmt &arg1, image_fmt &arg2);
}

#endif
