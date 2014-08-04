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


inline image_fmt padZeroes(int iNewWidth, int iNewHeight, const image_fmt &input)
{
    int iStartX = iNewWidth  / 2 - input.width()  / 2,
        iStartY = iNewHeight / 2 - input.height() / 2;
    image_fmt ret(iNewWidth, iNewHeight, 1, 1, 0);
    ret.draw_image(iStartX, iStartY, input);
    return ret;
}


    /* FFT: convolutions are multiplications in fourier space.. */
void FFT2D(const image_fmt &field, image_fmt &ret)
{
    // int M = field.height();
    // int N = field.width();
    // int Total = (M-2)*(N-2);
    // const data_fmt tmpR = cimg::PI/(2*M-2),
    //                tmpC = cimg::PI/(2*N-2),
    //                normalization = 1.0 / (4 * M * N);
    // data_fmt tmpD, tmp;
    //
    // //take inside
    // image_fmt InnerField(M-2,N-2, 1, 1,0);
    // image_fmt InnerResult = field.get_crop(1, 1, 0, 0, field.width() - 1, field.height() - 1, 0, 0);
    //
    // //DST coefficient
    // image_fmt DST(M-2, N-2, 1, 1, 0);
    // for(int y = 1; y < DST.height() + 1; y++)
    // {
    //     tmpD = std::sin((y) * tmpR);
    //     tmp = tmpD * tmpD;
    //     for(int x = 1; x < DST.width() + 1; x++)
    //     {
    //         tmpD = std::sin((x) * tmpC);
    //         DST(x-1,y-1) = 1.0 / (4 * (tmp + tmpD *  tmpD));
    //     }
    // }
    //
    // //FFT transform
    // fftwf_plan dct_fw, dct_bw;
    // image_fmt fft_result(DST, "xyz", 0);
    //
    // //forward
    // dct_fw = fftwf_plan_r2r_2d(M-2, N-2,InnerField.data(), fft_result.data(),FFTW_RODFT00, FFTW_RODFT00,FFTW_ESTIMATE | FFTW_DESTROY_INPUT);
    // fftwf_execute(dct_fw);
    // MLOG(severity_type::debug, "fft_result\n", image_psb::printImageAligned(fft_result));
    //
    // fft_result.mul(DST);
    //
    // //backward
    // dct_bw = fftwf_plan_r2r_2d(M-2, N-2,fft_result.data(), InnerResult.data(),FFTW_RODFT00, FFTW_RODFT00,FFTW_ESTIMATE | FFTW_DESTROY_INPUT);
    // fftwf_execute(dct_bw);
    //
    // InnerResult *= normalization;
    // MLOG(severity_type::debug, "pre-up\n", image_psb::printImageAligned(InnerResult));
    //
    // //Pad the InnerResult to Result
    // image_fmt Result = padZeroes(M, N, InnerResult);
    // MLOG(severity_type::debug, "Result\n", image_psb::printImageAligned(Result));
    // // Result = Pad(InnerResult);
    //
    // ret = Result;
    //
    // fftwf_destroy_plan(dct_fw);
    // fftwf_destroy_plan(dct_bw);
    // fftwf_cleanup();
}

} /* EndOfNamespace */
 /* EndOfNamespace */
