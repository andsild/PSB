#ifndef WAVELET_H
#define  WAVELET_H

#include "image_types.hpp"

namespace wavelet
{

void wavelet_5x5(const image_fmt &arg1, image_fmt &arg2);
void wavelet_7x7(const image_fmt &arg1, image_fmt &arg2);
void hermite_wavelet(const image_fmt &arg1, image_fmt &arg2);

} /* EndOfNamespace */

#endif 
