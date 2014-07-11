#include "fft.hpp"

#include <math.h>

#include "CImg.h"
#include "image2.hpp"
#include <iostream>

using namespace cimg_library;

namespace fft
{

    /* FFT: convolutions are multiplications in fourier space.. */
int FFT2D(image_psb::image_fmt input)
{
    const image_psb::imageList_fmt gradient = input.get_gradient("xy",1);
    
    // Remember average value for each channel (needed for normalizing the reconstruction).
    const image_psb::image_fmt average = input.get_resize(1,1,1,3,2);
    
    // Image estimation from gradient of 'input' starts here.
    //-----------------------------------------------------
    
    // Step 1.  Estimate divergence of the gradient field (use backward finite difference scheme).
    const image_psb::image_fmt divergence = gradient[0].get_gradient("x",-1)[0] + gradient[1].get_gradient("y",-1)[0];
    
    // Step 2. Invert Laplacian operator using FFT.
    image_psb::imageList_fmt  FFT = divergence.get_FFT();
    image_psb::image_fmt  factor(FFT[0].width(),FFT[0].height());
    cimg_forXY(factor,x,y) factor(x,y) = -(4-2*std::cos(2*x*cimg::PI/factor.width())-2*std::cos(2*y*cimg::PI/factor.height()));
    factor(0,0) = 1;
    FFT[0].div(factor);
    FFT[1].div(factor);
    image_psb::image_fmt res0 = FFT.get_FFT(true)[0];
    res0+=average.get_resize(res0.width(),res0.height());
    res0.cut(0,255);
    res0.display("Reconstruction");

    std::cout << res0.MSE(input) << " "
              << input.MSE(res0) << " "
              << input.MSE(input) << " "
            << std::endl;


}

} /* EndOfNamespace */
