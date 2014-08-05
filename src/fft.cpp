#include "fft.hpp"

#include <math.h>
#include <iostream>

#include "CImg.h"
#include "image_types.hpp"
#include "loginstance.hpp"
#include "image2.hpp"

using namespace cimg_library;
using namespace logging;
using namespace image_psb;

namespace solver
{


inline image_fmt padZeroes(int iNewWidth, int iNewHeight, const image_fmt &input)
{
    int iStartX = iNewWidth  / 2 - input.width()  / 2,
        iStartY = iNewHeight / 2 - input.height() / 2;
    image_fmt ret(iNewWidth, iNewHeight, 1, 1, 0);
    ret.draw_image(iStartX, iStartY, input);
    return ret;
}

void FFT_DCT(const image_fmt &field, image_fmt &ret)
{
    image_fmt useField = field;
    useField *= -1;
    image_fmt average(1,1,1,1, field.mean());
    imageList_fmt FFT = useField.get_FFT();
    image_fmt factor(FFT[0].width(),FFT[0].height());
    cimg_forXY(factor,x,y) factor(x,y) = 
                -(4-2*std::cos(2*x*cimg::PI/factor.width())
                   -2*std::cos(2*y*cimg::PI/factor.height()));
    factor(0,0) = 1;
    FFT[0].div(factor);
    FFT[1].div(factor);
    image_fmt res0 = FFT.get_FFT(true)[0];
    // res0+=average.get_resize(res0.width(),res0.height());
    res0+= average.get_resize(res0.width(), res0.height());
    MLOG(severity_type::debug, printImageAligned(average));
    MLOG(severity_type::debug, printImageAligned(res0));
}

const inline image_fmt getDST(const int iWidth, const int iHeight)
{
    image_fmt DST(iWidth, iHeight, 1, 1);
    const data_fmt tmpR = cimg::PI / (2*(iWidth+1)),
                   tmpC = cimg::PI / (2*(iHeight+1));
    data_fmt tmpD, tmp;

    //DST coefficient
    for(int y = 1; y < iHeight + 1; y++)
    {
        tmpD = std::sin((data_fmt)y * tmpC);
        tmp = tmpD * tmpD;
        for(int x = 1; x < iWidth + 1; x++)
        {
            tmpD = std::sin(x * tmpR);
            DST(x-1,y-1) = 4.0 * (tmp + (tmpD*tmpD));
        }
    }
    return DST;
}

void FFT_DST(const image_fmt &field, image_fmt &ret)
{
    // N, M
    const int iWidth = field.width(), iHeight = field.height();
    const image_fmt DST = getDST(iWidth-2, iHeight-2);
    const data_fmt normalization = 1.0 / (4.0 * (iHeight-1) * (iWidth-1));

    image_fmt InnerField = field.get_crop(1, 1, 0, 0,
                    iWidth - 2, iHeight- 2, 0, 0);
    image_fmt InnerResult(DST.width(), DST.height(), 1, 1, 0);
    image_fmt fft_result(InnerField.width(), InnerField.height(), 1, 1, 0);

    fftwf_plan dct_fw, dct_bw;
    
    //forward
    dct_fw = fftwf_plan_r2r_2d(InnerField.height(),InnerField.width(),
                             InnerField.data(),fft_result.data(),
                    FFTW_RODFT00, FFTW_RODFT00, FFTW_ESTIMATE | FFTW_DESTROY_INPUT);
    fftwf_execute(dct_fw);
    fft_result.div(DST);
    
    //backward
    dct_bw = fftwf_plan_r2r_2d(InnerField.height(), InnerField.width(),
                                fft_result.data(), InnerResult.data(),
                FFTW_RODFT00, FFTW_RODFT00,FFTW_ESTIMATE | FFTW_DESTROY_INPUT);
    fftwf_execute(dct_bw);
    
    InnerResult *= normalization;
    ret = padZeroes(iWidth, iHeight, InnerResult);
    
    fftwf_destroy_plan(dct_fw);
    fftwf_destroy_plan(dct_bw);
    fftwf_cleanup();

    MLOG(severity_type::debug, "Result\n", image_psb::printImageAligned(ret));
}

} /* EndOfNamespace */
