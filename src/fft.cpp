#include "fft.hpp"

#include <math.h>
#include <iostream>

#include "CImg.h"
#include "image_types.hpp"

using namespace cimg_library;

namespace solver
{

    /* FFT: convolutions are multiplications in fourier space.. */
void FFT2D(const image_fmt &input, image_fmt &ret)
{
    const imageList_fmt gradient = input.get_gradient("xy",1);
    
    // Remember average value for each channel (needed for normalizing the reconstruction).
    const image_fmt average = input.get_resize(1,1,1,3,2);
    
    // Step 1.  Estimate divergence of the gradient field (use backward finite difference scheme).
    const image_fmt divergence = gradient[0].get_gradient("x",-1)[0] + gradient[1].get_gradient("y",-1)[0];
    
    // Step 2. Invert Laplacian operator using FFT.
    imageList_fmt  FFT = divergence.get_FFT();
    image_fmt  factor(FFT[0].width(),FFT[0].height());
    cimg_forXY(factor,x,y) factor(x,y) = -(4-2*std::cos(2*x*cimg::PI/factor.width())-2*std::cos(2*y*cimg::PI/factor.height()));
    factor(0,0) = 1;
    FFT[0].div(factor);
    FFT[1].div(factor);
    image_fmt res0 = FFT.get_FFT(true)[0];
    res0+=average.get_resize(res0.width(),res0.height());
    res0.cut(0,255);
    res0.display("Reconstruction");

    std::cout << res0.MSE(input) << " "
              << input.MSE(res0) << " "
              << input.MSE(input) << " "
            << std::endl;


}

} /* EndOfNamespace */
