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


data_fmt vals[] = { 0.06110, 0.26177, 0.53034, 0.65934, 0.51106, 0.05407, 0.24453, 0.57410};
const int WEIGHTS_LEN = 8;
const image_fmt weights(vals, WEIGHTS_LEN);

// image_fmt forward_mask = weights.get_resize(4,1,1,1,0);
image_fmt tmp = weights.get_resize(4,1,1,1,0)
                        .resize(WEIGHTS_LEN - 1, 1, 1, 1, 0, 0);
image_fmt h1 = tmp.draw_image(4,0,0,0, tmp.get_resize(3,1,1,1,0).mirror('x'));
image_fmt forward_mask = h1.get_transpose() * h1;

const image_fmt backward_mask = forward_mask * weights(4);
image_fmt tmp2 = weights.get_crop(5,0,0,0, WEIGHTS_LEN - 1, 0, 0, 0)
                     .resize(WEIGHTS_LEN - 3, 1, 1, 1, 0, 0);
image_fmt gg = tmp2.draw_image(3,0,0,0,tmp2.get_resize(2,1,1,1,0).mirror('x'));
image_fmt g = gg.get_transpose() * gg;


inline image_fmt upSample(const image_fmt &input)
{
    image_fmt ret(input.width() * 2, input.height() * 2, 1, 1, 0);
    data_fmt *copyPtr = input._data;
    int iHeightStop = (ret.height() % 2 == 1) ? ret.height() + 2: ret.height(),
        iWidhtStop  = (ret.width() % 2 == 1) ? ret.width()   + 2: ret.width();
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
    // if(input.height() % 2 == 1) iNewHeight++;
    MLOG(severity_type::debug, iNewHeight, "\t", input.height());
    MLOG(severity_type::debug, "width: ", iNewWidth, "\t", input.width());
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
    const int iMaxLevel = ceil(cimg::log2(cimg::max(field.width(), field.height())))
                        - 1;
    MLOG(severity_type::debug, "[[ Max level set to : ", iMaxLevel, " ]]\n");
    
    // image_fmt useField = field.get_crop(0,0,0,0,field.width(), field.height() - 2, 0, 0);
    image_fmt useField = field;
    useField *= -1;
    image_fmt initPyr = downSample(useField.width() + forward_mask.width() * 2,
                                useField.height() + forward_mask.width() * 2,
                                useField);
    imageList_fmt forw_pyramid(initPyr);
    // MLOG(severity_type::debug, "Curpyr before iter:\n", image_psb::printImageAligned(initPyr));
    
    /* Forward transform */
    for(int iPos = 0; iPos < iMaxLevel; iPos++) //XXX: index starts at 1
    {
        image_fmt curPyr = forw_pyramid.back();
        curPyr.convolve(forward_mask);
        int iCropHeight = curPyr.height(), iCropWidth = curPyr.width();
        if(curPyr.height() % 2 == 1)
        {
            iCropHeight++;
            curPyr.shift(0,-1);
        }
        if(curPyr.width() % 2 == 1) // TODO
        {
            iCropWidth++;
            curPyr.shift(-1);
        }

        image_fmt imgCore = curPyr.get_resize(iCropWidth / SCALE,
                                              iCropHeight / SCALE, 1, 1, 1);
        // MLOG(severity_type::debug, "back_pyramid level : \n", image_psb::printImageAligned(imgCore));
        forw_pyramid.push_back(downSample(imgCore.width() + forward_mask.width() * 2,
                                imgCore.height() + forward_mask.height() * 2,
                                imgCore));
        // MLOG(severity_type::debug, "back_pyramid level : \n", image_psb::printImageAligned(forw_pyramid.back()));
    }
    //XXX: equal to matlab except that they wound up with an extra row of zero at the end, which can play a role..

    imageList_fmt back_pyramid(forw_pyramid.back().get_convolve(g));
    forw_pyramid.pop_back();
    /* Backward transform */
    for(int iPos = 0; iPos < forw_pyramid.size(); iPos++)
    {
        // MLOG(severity_type::debug, "back_pyramid level : \n", image_psb::printImageAligned(back_pyramid.back()));
        MLOG(severity_type::debug, "back_pyramid level : \n", image_psb::printImageAligned(forw_pyramid.back()));
        // MLOG(severity_type::debug, "\n\n\n");
        image_fmt imgCore = back_pyramid.back().get_crop(
                backward_mask.width(), backward_mask.height(), 0, 0,
                back_pyramid.back().width() - backward_mask.width(),
                back_pyramid.back().height() - backward_mask.height(), 0, 0);
        imgCore = upSample(imgCore);
    
        back_pyramid.push_back(
                imgCore.get_convolve(backward_mask)
                + forw_pyramid.back().get_convolve(g));

        forw_pyramid.pop_back();
    }
    // return;
    MLOG(severity_type::debug, "size: ", forw_pyramid.size());
    retImg = back_pyramid.back().get_crop(
                backward_mask.width(), backward_mask.height(), 0, 0,
                forw_pyramid.back().width() - backward_mask.width() - 1,
                forw_pyramid.back().height() - backward_mask.height() - 1, 0, 0);
    
    MLOG(severity_type::debug, "Returned image:\n", image_psb::printImageAligned(retImg.get_round(0).cut(0,255)));
    retImg = downSample(retImg.width() + 2, retImg.height() + 2, retImg);
}

} /* EndOfNamespace */
