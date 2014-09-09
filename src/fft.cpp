/** Perform either a DST or DCT transform of a gradient field
*/

#include "fft.hpp"

#include <math.h>
#include <iostream>

#include <fftw3.h>

#include "CImg.h"
#include "image_types.hpp"
#include "loginstance.hpp"
#include "image2.hpp"

using namespace cimg_library;
using namespace logging;
using namespace image_psb;

namespace solver
{

/** Perform a gradient reconstriction using Discrete Cosine Transform.

  @param field is padded with zeroes 1 pixel to each side
    (Neumann border condition)
  @param ret is the reconstructed gradient field.
*/
void FFT_DCT(const image_fmt &field, image_fmt &ret)
{
    imageList_fmt FFT = field.get_FFT();
    image_fmt factor(FFT[0].width(),FFT[0].height());
    cimg_forXY(factor,x,y) factor(x,y) = 
                -(4-2*std::cos(2*x*cimg::PI/factor.width())
                   -2*std::cos(2*y*cimg::PI/factor.height()));
    factor(0,0) = 1;
    FFT[0].div(factor);
    FFT[1].div(factor);
    ret = FFT.get_FFT(true)[0];
}

/** Get the DST coefficient
*/
const inline image_fmt getDST(const int iWidth, const int iHeight)
{
    image_fmt DST(iWidth, iHeight, 1, 1);
    const data_fmt tmpR = cimg::PI / (2*(iWidth+1)),
                   tmpC = cimg::PI / (2*(iHeight+1));
    data_fmt tmpD, tmp;

    cimg_forY(DST, y)
    {
        tmpD = std::sin((data_fmt)(y+1) * tmpC);
        tmp = tmpD * tmpD;
        cimg_forX(DST, x)
        {
            tmpD = std::sin((x+1) * tmpR);
            DST(x,y) = 4.0 * (tmp + (tmpD*tmpD));
        }
    }
    return DST;
}

/** Comput a DST on a field

  @param field is padded with zeroes 1 pixel to each side
    (Neumann border condition)
  @param ret is the reconstructed gradient field.
*/
void FFT_DST(const image_fmt &field, image_fmt &ret)
{
    const int iWidth = field.width(), iHeight = field.height();
    const image_fmt DST = getDST(iWidth, iHeight);
    const data_fmt normalization = 1.0 / (4.0 * (iHeight-1) * (iWidth-1));
    image_fmt fft_result(iWidth, iHeight, 1, 1, 0);

    fftwf_plan dct_fw, dct_bw;
    
    //forward
    dct_fw = fftwf_plan_r2r_2d(iHeight,iWidth,
                             field._data,fft_result.data(),
                    FFTW_RODFT00, FFTW_RODFT00, FFTW_ESTIMATE | FFTW_DESTROY_INPUT);
    fftwf_execute(dct_fw);
    fft_result.div(DST);
    
    //backward
    dct_bw = fftwf_plan_r2r_2d(iHeight, iWidth,
                                fft_result.data(), ret.data(),
                FFTW_RODFT00, FFTW_RODFT00,FFTW_ESTIMATE | FFTW_DESTROY_INPUT);
    fftwf_execute(dct_bw);
    
    ret *= normalization;
    
    fftwf_destroy_plan(dct_fw);
    fftwf_destroy_plan(dct_bw);
    fftwf_cleanup();
}

} /* EndOfNamespace */
