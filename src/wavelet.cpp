/** Pyramid convolution.

  The precision format will induce a certain error. To reduce this, a scale
  should be used... (currently not implemented)
*/

#include "wavelet.hpp"
#include <math.h>
#include <iostream>

#include "CImg.h"
#include "loginstance.hpp"
#include "image2.hpp"
#include "image_types.hpp"

using namespace cimg_library;
using namespace logging;
using namespace image_psb;

namespace wavelet
{


inline image_fmt upSample(const image_fmt &input, const int iNewWidth, const int iNewHeight)
{
    // image_fmt ret(input.width() * 2, input.height() * 2, 1, 1, 0);
    image_fmt ret(iNewWidth, iNewHeight, 1, 1, 0);
    data_fmt *copyPtr = input._data;
    // int iHeightStop = (ret.height() % 2 == 1) ? ret.height() + 2: ret.height(),
    //     iWidhtStop  = (ret.width() % 2 == 1) ? ret.width()   + 2: ret.width();
    int iHeightStop = (ret.height() % 2 == 1) ? ret.height() + 0: ret.height(),
        iWidhtStop  = (ret.width() % 2 == 1) ? ret.width()   + 0: ret.width();
    for(int yPos = 0; yPos < iHeightStop; yPos += 2)
    {
        for(int xPos = 0; xPos < iWidhtStop; xPos+= 2)
        {
            ret(xPos, yPos) = *(copyPtr++);
        }
    }

    return ret;
}

inline image_fmt downSample(int iNewWidth, int iNewHeight, const image_fmt &input)
{
    int iStartX = iNewWidth  / 2 - input.width()  / 2,
        iStartY = iNewHeight / 2 - input.height() / 2;
    image_fmt ret(iNewWidth, iNewHeight, 1, 1, 0);
    ret.draw_image(iStartX, iStartY, input);
    return ret;
}

const inline image_fmt cubic_spline1(const int iWidth)
{
    const double dScale = (2 / (double)iWidth);
    image_fmt ret(iWidth, 1, 1, 1);
    data_fmt *ptr = ret.data();
    double x;
    for(x = -1; x < 0; x+=dScale)
    {
        *(ptr++) = pow((1.0 + x), 2) * (1 - (2.0 * x));
    }
    //TODO:
    // *(ptr++) = 0;
    //x = x + dScale;
    for(x ; x < 1; x+=dScale)
    {
        *(ptr++) = pow((1.0 - x), 2) * (1 + (2.0 * x));
    }

    return ret;
}

const inline image_fmt cubic_spline2(const int iWidth)
{
    const double dScale = (2 / (double)iWidth);
    image_fmt ret(iWidth, 1, 1, 1);
    data_fmt *ptr = ret.data();
    double x;
    for(x = -1; x < 0; x+=dScale)
    {
        *(ptr++) = pow((1.0 + x), 2) * x;}
    }
    for(x ; x < 1; x+=dScale)
    {
        *(ptr++) = pow((1.0 - x), 2) * x;
    }

    return ret;
}


const imageList_fmt getHighPass()
{
    image_fmt negative(4,1,1,1,
                    -2, -21, 1, 9),
              zero(4,1,1,1,
                    4,0,0,12),
              positive(4,1,1,1,
                    -2, 21, -1,9);
    imageList_fmt ret(negative, zero, positive);
    return ret;
}

const imageList_fmt getLowPass()
{
    image_fmt negative(4,1,1,1,
                    0.5, 0.75, -0.125, -0.125),
              zero(4,1,1,1,
                    1,0,0,0.5),
              positive(4,1,1,1,
                    0.5, 0.75, 0.125, -0.125);
    imageList_fmt ret(negative, zero, positive);
    return ret;
}
const imageList_fmt H = getLowPass(),
                    G = getHighPass();

const inline void getControlMatrix(int iLevel,
        imageList_fmt &control_matrix)
{
    image_fmt lowTier = control_matrix.front();
    image_fmt midTier,newTier;
    for(int iPos = 0; iPos < iLevel; iPos++)
    {
        midTier = control_matrix.back();
        control_matrix.push_back((lowTier + midTier).get_convolve( H[0 + 1]));
        lowTier = midTier;
    }
}

void hermite_wavelet(const image_fmt &field, image_fmt &retImg)
{
    // const data_fmt divisor = (cimg::abs(field.min()) > field.max()) ? field.min() : field.max();
    //TODO: confirm
    const data_fmt divisor = sqrt(field.dot(field));
    image_fmt useField = field.get_crop(1,1,0,0,
            field.width() - 2, field.height() - 2, 0, 0);
    MLOG(severity_type::debug, "field\n", printImageAligned(useField));
    image_fmt phi1(useField, "xyz", 0), phi2(useField, "xyz", 0);
    image_fmt spline1 = cubic_spline1(useField.width()),
              spline2 = cubic_spline2(useField.width());
    // retImg = spline2;
    // data_fmt *fieldPtr = useField.data();
    // cimg_for(phi1, ptr, data_fmt)
    // {
    //     *ptr = cubic_spline1(*(fieldPtr++));
    // }
    // fieldPtr = useField.data();
    // cimg_for(phi2, ptr, data_fmt)
    // {
    //     *ptr = cubic_spline2(*(fieldPtr++));
    // }
    //
    // const image_fmt p1 = useField * phi1.get_transpose(),
    //                 p2 = useField * phi2.get_transpose();
    // imageList_fmt control_matrix(p1,p2);
    //
    // getControlMatrix(5, control_matrix);

    MLOG(severity_type::debug, "returned image\n", printImage(retImg));
}



data_fmt vals_5x5[] = { 0.15, 0.5, 0.7, 0.175, 0.547};
const int WEIGHTS_LEN_5x5 = 5;
const image_fmt weights_5x5(vals_5x5, WEIGHTS_LEN_5x5);

image_fmt tmp_5x5 = weights_5x5.get_resize(3,1,1,1,0)
                        .resize(3 + 3 - 1, 1, 1, 1, 0, 0);
image_fmt h1_5x5 = tmp_5x5.draw_image(3 - 1,0,0,0, tmp_5x5.get_resize(3,1,1,1,0).mirror('x'));
image_fmt forward_mask_5x5 = h1_5x5.get_transpose() * h1_5x5;

const image_fmt backward_mask_5x5 = forward_mask_5x5;

image_fmt tmp2_5x5 = weights_5x5.get_crop(3,0,0,0, WEIGHTS_LEN_5x5, 0, 0, 0)
                     .resize(WEIGHTS_LEN_5x5 - 3 + 1, 1, 1, 1, 0, 0);
image_fmt gg_5x5 = tmp2_5x5.draw_image(2,0,0,0,tmp2_5x5.get_resize(1,1,1,1,0).mirror('x'));
const image_fmt g_5x5 = gg_5x5.get_transpose() * gg_5x5;


void wavelet_5x5(const image_fmt &field, image_fmt &retImg)
{
    const int SCALE = 2;
    const int iMaxLevel = ceil(cimg::log2(cimg::max(field.width(), field.height())))
                        - 1;
    // MLOG(severity_type::debug, "forw:\n", printImageAligned(forward_mask_5x5));
    // MLOG(severity_type::debug, "back:\n", printImageAligned(backward_mask_5x5));
    // MLOG(severity_type::debug, "g:\n", printImageAligned(g_5x5));

    image_fmt initPyr = downSample(field.width() + forward_mask_5x5.width() * 2,
                                field.height() + forward_mask_5x5.width() * 2,
                                field);
    imageList_fmt forw_pyramid(initPyr);
    
    /* Forward transform */
    for(int iPos = 0; iPos < iMaxLevel; iPos++) 
    {
        image_fmt curPyr = forw_pyramid.back();
        curPyr.convolve(forward_mask_5x5);
        int iCropHeight = curPyr.height(), iCropWidth = curPyr.width();
        if(iCropHeight % 2 == 1)
        {
            iCropHeight++;
            curPyr.shift(0,-1);
        }
        if(iCropWidth % 2 == 1) // TODO
        {
            iCropWidth++;
            curPyr.shift(-1);
        }

        image_fmt imgCore = curPyr.get_resize(iCropWidth / SCALE,
                                              iCropHeight / SCALE, 1, 1, 1);
        forw_pyramid.push_back(downSample(imgCore.width() + forward_mask_5x5.width() * 2,
                                imgCore.height() + forward_mask_5x5.height() * 2,
                                imgCore));
    }

    imageList_fmt back_pyramid(forw_pyramid.back().get_convolve(g_5x5));
    forw_pyramid.pop_back();
    /* Backward transform */
    const int iIterCount = forw_pyramid.size(); /*< Store iter count, since we
                                                 are going to pop() from list */
    for(int iPos = 0; iPos < iIterCount; iPos++)
    {
        image_fmt imgCore = back_pyramid.back().get_crop(
                backward_mask_5x5.width() + 0, backward_mask_5x5.height() + 0, 0, 0,
                back_pyramid.back().width() - backward_mask_5x5.width() - 1,
                back_pyramid.back().height() - backward_mask_5x5.height() - 1, 0, 0);
        imgCore = upSample(imgCore, forw_pyramid.back().width(), forw_pyramid.back().height());
        back_pyramid.push_back(
                imgCore.get_convolve(backward_mask_5x5)
                + forw_pyramid.back().get_convolve(g_5x5));
        forw_pyramid.pop_back();
    }
    retImg = back_pyramid.back().get_crop(
                backward_mask_5x5.width(), backward_mask_5x5.height(), 0, 0,
                back_pyramid.back().width() - backward_mask_5x5.width() - 1,
                back_pyramid.back().height() - backward_mask_5x5.height() - 1, 0, 0);

    MLOG(severity_type::debug, "pyr return\n", image_psb::printImageAligned(retImg));
}




data_fmt vals[] = { 0.06110, 0.26177, 0.53034, 0.65934, 0.51106, 0.05407, 0.24453, 0.57410};
const int WEIGHTS_LEN = 8;
const image_fmt weights(vals, WEIGHTS_LEN);

image_fmt tmp = weights.get_resize(4,1,1,1,0)
                        .resize(WEIGHTS_LEN - 1, 1, 1, 1, 0, 0);
image_fmt h1 = tmp.draw_image(4,0,0,0, tmp.get_resize(3,1,1,1,0).mirror('x'));
image_fmt forward_mask = h1.get_transpose() * h1;

const image_fmt backward_mask = forward_mask * weights(4);
image_fmt tmp2 = weights.get_crop(5,0,0,0, WEIGHTS_LEN - 1, 0, 0, 0)
                     .resize(WEIGHTS_LEN - 3, 1, 1, 1, 0, 0);
image_fmt gg = tmp2.draw_image(3,0,0,0,tmp2.get_resize(2,1,1,1,0).mirror('x'));
image_fmt g = gg.get_transpose() * gg;



void wavelet_7x7(const image_fmt &field, image_fmt &retImg)
{
    const int SCALE = 2;
    const int iMaxLevel = ceil(cimg::log2(cimg::max(field.width(), field.height())))
                        - 1;

    image_fmt initPyr = downSample(field.width() + forward_mask.width() * 2,
                                field.height() + forward_mask.width() * 2,
                                field);
    imageList_fmt forw_pyramid(initPyr);
    
    /* Forward transform */
    for(int iPos = 0; iPos < iMaxLevel; iPos++) 
    {
        image_fmt curPyr = forw_pyramid.back();
        curPyr.convolve(forward_mask);
        int iCropHeight = curPyr.height(), iCropWidth = curPyr.width();
        if(iCropHeight % 2 == 1)
        {
            iCropHeight++;
            curPyr.shift(0,-1);
        }
        if(iCropWidth % 2 == 1) // TODO
        {
            iCropWidth++;
            curPyr.shift(-1);
        }

        image_fmt imgCore = curPyr.get_resize(iCropWidth / SCALE,
                                              iCropHeight / SCALE, 1, 1, 1);
        forw_pyramid.push_back(downSample(imgCore.width() + forward_mask.width() * 2,
                                imgCore.height() + forward_mask.height() * 2,
                                imgCore));
    }

    imageList_fmt back_pyramid(forw_pyramid.back().get_convolve(g));
    forw_pyramid.pop_back();
    /* Backward transform */
    const int iIterCount = forw_pyramid.size(); /*< Store iter count, since we
                                                 are going to pop() from list */
    for(int iPos = 0; iPos < iIterCount; iPos++)
    {
        image_fmt imgCore = back_pyramid.back().get_crop(
                backward_mask.width() + 0, backward_mask.height() + 0, 0, 0,
                back_pyramid.back().width() - backward_mask.width() - 1,
                back_pyramid.back().height() - backward_mask.height() - 1, 0, 0);
        imgCore = upSample(imgCore, forw_pyramid.back().width(), forw_pyramid.back().height());
        back_pyramid.push_back(
                imgCore.get_convolve(backward_mask)
                + forw_pyramid.back().get_convolve(g));
        forw_pyramid.pop_back();
    }
    retImg = back_pyramid.back().get_crop(
                backward_mask.width(), backward_mask.height(), 0, 0,
                back_pyramid.back().width() - backward_mask.width() - 1,
                back_pyramid.back().height() - backward_mask.height() - 1, 0, 0);

    MLOG(severity_type::debug, "pyr return\n", image_psb::printImageAligned(retImg));
}

} /* EndOfNamespace */
