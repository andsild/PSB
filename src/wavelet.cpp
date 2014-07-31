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

inline image_fmt upSample(const image_fmt &input)
{
    image_fmt ret(input.width() * 2, input.height() * 2, 1, 1, 0);
    data_fmt *copyPtr = input._data;
    int iHeightStop = (ret.height() % 2 == 1) ? ret.height() + 2 : ret.height(),
        iWidhtStop  = (ret.width() % 2 == 1) ? ret.width() + 2 : ret.width();
    for(int yPos = 0; yPos < iHeightStop; yPos += 2)
    {
        for(int xPos = 0; xPos < iWidhtStop; xPos+= 2)
        {
            ret(xPos, yPos) = *(copyPtr++);
        }
    }

    return ret;
}

inline image_fmt padCore(int iNewWidth, int iNewHeight, const image_fmt &input)
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
    // MLOG(severity_type::debug, "[[ SCALE set to: ", SCALE, " ]]\n");
    image_fmt useField = field.get_crop(1,1,0,0,field.width() - 2, field.height() -2, 0, 0);
    cimg_for(useField, ptr, data_fmt) *ptr *= -1;
    image_fmt initPyr = padCore(useField.width() + forward_mask.width() * 2,
                    useField.height() + forward_mask.width() * 2,
                    useField);
    // MLOG(severity_type::debug, "[[ current pyramid before iter:\n", curPyr.print(), " ]]\n");
    // MLOG(severity_type::debug, "Curpyr before iter:\n", image_psb::printImage(curPyr));
    // MLOG(severity_type::debug, "Padding new core to xdim,ydim: ", useField.width() + forward_mask.width() * 2, ", ", useField.height() + forward_mask.height() * 2, "\n");
    imageList_fmt pyramid(initPyr);
    
    /* Forward transform */
    for(int iPos = 0; iPos < iMaxLevel - 1; iPos++)
    {
        image_fmt curPyr = pyramid.back();
        curPyr.convolve(forward_mask);
        image_fmt imgCore = curPyr.get_resize(curPyr.width() / SCALE,
                                              curPyr.height() / SCALE, 1, 1, 1);
        curPyr = padCore(imgCore.width() + forward_mask.width() * 2,
                         imgCore.height() + forward_mask.height() * 2,
                        imgCore);
        pyramid.push_back(curPyr);
        MLOG(severity_type::debug, "Forward analysis: step ", iPos, "\n", image_psb::printImage(imgCore));
        // MLOG(severity_type::debug, "Forward analysis: step ", iPos, "\n", image_psb::printImage(curPyr));
    }

    // MLOG(severity_type::debug, "Forward analysis: finished\n");
    pyramid.back().convolve(g);

    /* Backward transform */
    cimglist_for(pyramid,_)
    {
        image_fmt imgCore = pyramid.back().get_crop(
                backward_mask.width(), backward_mask.height(), 0, 0,
                pyramid.back().width() - backward_mask.width() - 1,
                pyramid.back().height() - backward_mask.height() - 1, 0, 0);
        // MLOG(severity_type::debug, "Backward analysis: step:\n",
        //                             image_psb::printImageAligned(imgCore));
        // curPyr.print();
        // imgCore.print();
        imgCore = upSample(imgCore);
    
        pyramid.pop_back();
        MLOG(severity_type::debug, "Backward analysis: step\n", image_psb::printImageAligned(pyramid.back().get_convolve(g)));
        pyramid.back() = imgCore.get_convolve(backward_mask) + pyramid.back().get_convolve(g);
        // curPyr = imgCore.get_convolve(backward_mask) + pyramid.back().get_convolve(g);
        // MLOG(severity_type::debug, "Backward analysis: step ", iPos, "\n", image_psb::printImageAligned(curPyr));
        // MLOG(severity_type::debug, "Backward analysis: step ", iPos, "\n", image_psb::printImageAligned(curPyr.get_convolve(g)));
    }
    // MLOG(severity_type::debug, "Returned image:\n", image_psb::printImageAligned(pyramid.front()));
    
    retImg = pyramid.front().get_crop(
                backward_mask.width(), backward_mask.height(), 0, 0,
                pyramid.front().width() - backward_mask.width() - 1,
                pyramid.front().height() - backward_mask.height() - 1, 0, 0);
    
    MLOG(severity_type::debug, "Returned image:\n", image_psb::printImageAligned(retImg));
    retImg = padCore(retImg.width() + 2, retImg.height() + 2, retImg);
}



} /* EndOfNamespace */

        // MLOG(severity_type::debug, "Backward analysis: step ", iPos, "\n",
        //                             image_psb::printImageAligned(imgCore));
        // image_fmt test(3,3,1,1,
        //             1,2,3,//11,
        //             4,5,6,//12,
        //             7,8,9);//13);
        // MLOG(severity_type::debug, "Backward analysis: step ", iPos, "\n",
        //                             image_psb::printImageAligned(test));
        // test = upSample(test);
        // MLOG(severity_type::debug, "Backward analysis: step ", iPos, "\n",
       //                             image_psb::printImageAligned(test));
 
