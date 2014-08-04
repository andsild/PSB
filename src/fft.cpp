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

/*
void FFT2D(const image_fmt &field, image_fmt &ret)
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
*/

void FFT2D(const image_fmt &field, image_fmt &ret)
{


    int M = field.height();
    int N = field.width();
    int Total = (M-2)*(N-2);
    // MLOG(severity_type::debug, M, "\t", N);
    image_fmt DST(M-2, N-2, 1, 1, 0);
    const data_fmt tmpR = cimg::PI / (2*(M-1)),
                   tmpC = cimg::PI / (2*(N-1)),
                   normalization = 1.0 / (4 * M * N);
    data_fmt tmpD, tmp;
    
    //DST coefficient
    for(int y = 1; y < DST.height() + 1; y++)
    {
        tmpD = std::sin((data_fmt)y * tmpR);
        tmp = tmpD * tmpD;
        // MLOG(severity_type::debug, tmpD);
        for(int x = 1; x < DST.width() + 1; x++)
        {
            tmpD = std::sin(x * tmpC);
            DST(x-1,y-1) = 4.0 * (tmp + (tmpD*tmpD));
        }
    }
    // MLOG(severity_type::debug, "const sine useField\n", printImageAligned(DST));
    //take inside
    image_fmt useField = field;
    // useField *= -1;
    
    image_fmt InnerField = useField.get_crop(1, 1, 0, 0,
                    useField.width() - 1, useField.height() - 1, 0, 0);
    // MLOG(severity_type::debug, "RHS\n", printImageAligned(InnerField));
    
    //FFT transform
    fftwf_plan dct_fw, dct_bw;
    
    //forward
    // dct_fw = fftwf_plan_r2r_2d(M-2, N-2,seq, fft_result.data(),FFTW_RODFT00, FFTW_RODFT00,FFTW_ESTIMATE | FFTW_DESTROY_INPUT);
    image_fmt fft_result(InnerField.width(), InnerField.height(), 1, 1, 0);
    // fft_result = InnerField.get_FFT()[0];
    dct_fw = fftwf_plan_r2r_2d(InnerField.height(),InnerField.width(),
                             InnerField.data(),fft_result.data(),
                    FFTW_RODFT00, FFTW_RODFT00, FFTW_ESTIMATE | FFTW_DESTROY_INPUT);
    fftwf_execute(dct_fw);
    // imageList_fmt myFFT = InnerField.get_FFT();
    // myFFT[0].mul(DST);
    // myFFT[1].mul(DST);
    // ret = myFFT.get_FFT(true)[0];
    // ret *= normalization;
    
    MLOG(severity_type::debug, "fft_result\n", image_psb::printImageAligned(fft_result));
    
    fft_result.mul(DST);
    MLOG(severity_type::debug, "fft_result, div by DST\n", image_psb::printImageAligned(fft_result));
    
    //backward
    image_fmt InnerResult(DST.width(), DST.height(), 1, 1, 0);
    dct_bw = fftwf_plan_r2r_2d(InnerField.height(), InnerField.width(),
                                fft_result.data(), InnerResult.data(),
                FFTW_RODFT00, FFTW_RODFT00,FFTW_ESTIMATE | FFTW_DESTROY_INPUT);
    fftwf_execute(dct_bw);
    // InnerResult *= normalization;
    
    // ret = padZeroes(M, N, InnerResult);
    MLOG(severity_type::debug, "Result\n", image_psb::printImageAligned(ret));
    
    fftwf_destroy_plan(dct_fw);
    fftwf_destroy_plan(dct_bw);
    fftwf_cleanup();
}

} /* EndOfNamespace */
