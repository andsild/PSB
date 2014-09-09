#ifndef _QUEUE_H
#define  _QUEUE_H

#include <string>

#include "image_types.hpp"

void processImage(std::string, double, double, data_fmt, 
        const bool, const bool, const bool, /* Iterative */
        const bool, const bool,             /* FFT */
        const bool, const bool, const bool); /* wavelet */

#endif
