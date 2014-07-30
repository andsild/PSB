#include "wavelet.hpp"

#include <math.h>
#include <iostream>

#include "CImg.h"
#include "loginstance.hpp"
#include "image2.hpp"
#include "image_types.hpp"

using namespace cimg_library;
using namespace logging;

namespace wavelet
{


image_fmt padCore(int iNewWidth, int iNewHeight, const image_fmt &input)
{
    int iStartX = iNewWidth  / 2 - input.width()  / 2,
        iStartY = iNewHeight / 2 - input.height() / 2;
    image_fmt ret(iNewWidth, iNewHeight, 1, 1, 0);
    ret.draw_image(iStartX, iStartY, input);
    return ret;
}

/* Boundary conditions:
    0 = pad with zero
    1 = repeat last value
    2 = repeat from start
*/

void pyconv(const image_fmt &field, image_fmt &retImg)
{

    const int SCALE = 2;
    const int WEIGHTS_LEN = 8;
   
    data_fmt vals[] = { 0.06110, 0.26177, 0.53034, 0.65934, 0.51106, 0.05407, 0.24453, 0.57410};
    const image_fmt weights(vals, WEIGHTS_LEN);

    image_fmt forward_mask = weights.get_resize(4,1,1,1,0);
    forward_mask.resize(WEIGHTS_LEN - 1, 1, 1, 1, 0, 0);
    forward_mask.draw_image(4,0,0,0,forward_mask.get_resize(3,1,1,1,0).mirror('x'));
    forward_mask = forward_mask.get_transpose() * forward_mask;

    const image_fmt backward_mask = forward_mask * weights(4);

    image_fmt g = weights.get_crop(5,0,0,0, WEIGHTS_LEN - 1, 0, 0, 0);
    g.resize(WEIGHTS_LEN - 3, 1, 1, 1, 0, 0);
    g.draw_image(3,0,0,0,g.get_resize(2,1,1,1,0).mirror('x'));
    g = g.get_transpose() * g;

    int iPyrWidth = field.width(),
        iPyrHeight = field.height();
    int iMaxLevel = ceil(cimg::log2(cimg::max(iPyrWidth, iPyrHeight)));
    
    iPyrWidth = floor((double)iPyrWidth / (double)SCALE);
    iPyrHeight = floor((double)iPyrHeight / (double)SCALE);
    // iPyrWidth--;iPyrHeight--;
    
    const int iDrawXpos = (field.width()/ SCALE) - (iPyrWidth/ 2),
             iDrawYpos = (field.height()/ SCALE) - (iPyrHeight/ 2);
    
    // MLOG(severity_type::debug, "Weights:\n", image_psb::printImage(weights));
    // MLOG(severity_type::debug, "Forward mask:\n", image_psb::printImage(forward_mask));
    //
    // MLOG(severity_type::debug, "Backward mask:\n", image_psb::printImage(backward_mask));
    // MLOG(severity_type::debug, "Hacky G:\n", image_psb::printImage(g));
    // MLOG(severity_type::debug, "Max level set to: ", iMaxLevel);
    image_fmt useField = field.get_crop(1,1,0,0,field.width() - 2, field.height() -2, 0, 0);
    image_fmt curPyr = padCore(useField.width() + forward_mask.width() * 2,
                    useField.height() + forward_mask.width() * 2,
                    useField);
                    
    MLOG(severity_type::debug, "Curpyr before iter:\n", image_psb::printImage(curPyr));
    
    /* Forward transform */
    for(int iPos = 0; iPos < iMaxLevel - 1; iPos++)
    {
        curPyr.convolve(forward_mask);
        image_fmt imgCore = curPyr.get_resize(curPyr.width() / SCALE,
                                              curPyr.height() / SCALE, 1, 1, 1);
        curPyr = padCore(curPyr.width() + forward_mask.width() * 2,
                        curPyr.height() + forward_mask.height() * 2,
                        imgCore);

        MLOG(severity_type::debug, "Forward analysis: step ", iPos, "\n", image_psb::printImage(imgCore));
        MLOG(severity_type::debug, "Forward analys: step ", iPos, "\n", image_psb::printImage(curPyr));
    
    }

    MLOG(severity_type::debug, "Forward analys: finished", "\n", image_psb::printImage(curPyr));
    return;
    /* Backward transform */
    for(int iPos = 0; iPos < iMaxLevel; iPos++)
    {
        image_fmt imgCore = curPyr.get_crop(iDrawXpos, iDrawYpos, 0, 0,
                        iDrawXpos + iPyrWidth,
                        iDrawYpos + iPyrHeight, 0, 0);
        MLOG(severity_type::debug, "Backward analysis: step ", iPos, "\n",
                                    image_psb::printImage(imgCore));
        imgCore.resize(field.width(), field.height(), 1, 1,
                      4, 0);
        curPyr = imgCore.get_convolve(backward_mask) + curPyr.get_convolve(g);
        MLOG(severity_type::debug, "Backward analysis: step ", iPos, "\n", image_psb::printImage(curPyr));
    }
}



} /* EndOfNamespace */
