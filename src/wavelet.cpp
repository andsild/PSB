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

/* Boundary conditions:
    0 = pad with zero
    1 = repeat last value
    2 = repeat from start
*/

void pyconv(const image_fmt &field, image_fmt &retImg)
{

    MLOG(severity_type::debug, "Input field:\n", image_psb::printImage(field));
    return;

    const int SCALE = 2;
    const int WEIGHTS_LEN = 8;
   
    data_fmt vals[] = { 0.06110, 0.26177, 0.53034, 0.65934, 0.51106, 0.05407, 0.24453, 0.57410};
    const image_fmt weights(vals, WEIGHTS_LEN);
    MLOG(severity_type::debug, "Weights:\n", image_psb::printImage(weights));

    image_fmt forward_mask = weights.get_resize(4,1,1,1,0);
    forward_mask.resize(WEIGHTS_LEN - 1, 1, 1, 1, 0, 0);
    forward_mask.draw_image(4,0,0,0,forward_mask.get_resize(3,1,1,1,0).mirror('x'));
    forward_mask = forward_mask.get_transpose() * forward_mask;

    MLOG(severity_type::debug, "Forward mask:\n", image_psb::printImage(forward_mask));

    const image_fmt backward_mask = forward_mask * weights(4);
    MLOG(severity_type::debug, "Backward mask:\n", image_psb::printImage(backward_mask));

    image_fmt g = weights.get_crop(5,0,0,0, WEIGHTS_LEN - 1, 0, 0, 0);
    g.resize(WEIGHTS_LEN - 3, 1, 1, 1, 0, 0);
    g.draw_image(3,0,0,0,g.get_resize(2,1,1,1,0).mirror('x'));
    g = g.get_transpose() * g;
    MLOG(severity_type::debug, "Hacky G:\n", image_psb::printImage(g));

    int iPyrWidth = field.width(),
        iPyrHeight = field.height();
    int iMaxLevel = ceil(cimg::log2(cimg::max(iPyrWidth, iPyrHeight)));
    
    image_fmt curPyr = field.get_resize(iPyrWidth, iPyrWidth, 1, 1, 0, 0);
    
    iPyrWidth = floor((double)iPyrWidth / (double)SCALE);
    iPyrHeight = floor((double)iPyrHeight / (double)SCALE);
    // iPyrWidth--;iPyrHeight--;
    
    const int iDrawXpos = (field.width()/ SCALE) - (iPyrWidth/ 2),
             iDrawYpos = (field.height()/ SCALE) - (iPyrHeight/ 2);
    
    MLOG(severity_type::debug, "Max level set to: ", iMaxLevel);
    MLOG(severity_type::debug, "Before iterations\n",
                                image_psb::printImage(curPyr));
    
    /* Forward transform */
    for(int iPos = 0; iPos < iMaxLevel; iPos++)
    {
        curPyr.convolve(forward_mask);
        image_fmt tmp = curPyr.get_shift(-1,-1, 0, 0, 1)
                        .get_resize(iPyrWidth, iPyrHeight, 1, 1, 1);
        curPyr = curPyr.get_fill(0)
                       .draw_image( (field.width() / SCALE) - (iPyrWidth/ 2),
                                    (field.height()/ SCALE) - (iPyrHeight / 2),
                                    0, 0, tmp, 1);
        MLOG(severity_type::debug, "Forward analysis: step ", iPos, "\n", image_psb::printImage(tmp));
        MLOG(severity_type::debug, "Forward analys: step ", iPos, ", drawing at ", iDrawXpos, ",", iDrawYpos, "\n", image_psb::printImage(curPyr));
    
    }
    /* Backward transform */
    for(int iPos = 0; iPos < iMaxLevel; iPos++)
    {
        image_fmt tmp = curPyr.get_crop(iDrawXpos, iDrawYpos, 0, 0,
                        iDrawXpos + iPyrWidth,
                        iDrawYpos + iPyrHeight, 0, 0);
        MLOG(severity_type::debug, "Backward analysis: step ", iPos, "\n",
                                    image_psb::printImage(tmp));
        tmp.resize(field.width(), field.height(), 1, 1,
                      4, 0);
        curPyr = tmp.get_convolve(backward_mask) + curPyr.get_convolve(g);
        MLOG(severity_type::debug, "Backward analysis: step ", iPos, "\n", image_psb::printImage(curPyr));
    }
}

} /* EndOfNamespace */
