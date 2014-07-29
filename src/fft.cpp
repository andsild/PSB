#include "fft.hpp"

#include <math.h>
#include <iostream>

#include "CImg.h"
#include "image_types.hpp"
#include "loginstance.hpp"
#include "image2.hpp"

using namespace cimg_library;
using namespace logging;

namespace solver
{

    /* FFT: convolutions are multiplications in fourier space.. */
void FFT2D(const image_fmt &field, image_fmt &ret)
{
    // const imageList_fmt gradient = field.get_gradient("xy",1);
    
    // Remember average value for each channel (needed for normalizing the reconstruction).
    const image_fmt average = field.get_resize(1,1,1,1,2);
    
    // Step 1.  Estimate divergence of the gradient field (use backward finite difference scheme).
    // const image_fmt divergence = gradient[0].get_gradient("x",-1)[0] + gradient[1].get_gradient("y",-1)[0];

    // Step 2. Invert Laplacian operator using FFT.
    image_fmt factor(field, "xyz", 0);
    cimg_forXY(factor,x,y) factor(x,y) = -(4
                - 2 * std::cos(2*x*cimg::PI/factor.width())
                - 2 * std::cos(2*y*cimg::PI/factor.height()));
    factor(0,0) = 1;

    imageList_fmt  FFT = field.get_FFT();
    FFT[0].div(factor);
    // FFT[1].div(factor);
    ret = FFT.get_FFT(true)[0];
    ret += average.get_resize(field.width(), field.height());
    // ret.cut(0,255);
    // res0.display("Reconstruction");
    //

    // std::cerr << res0.MSE(field) << " "
    //           << field.MSE(res0) << " "
    //           << field.MSE(field) << " "
    //         << std::endl;

}

} /* EndOfNamespace */
