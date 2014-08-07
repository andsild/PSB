/** Pyramid convolution.
*/
#include "wavelet.hpp"

#include <math.h>
#include <iostream>
#include <vector>

#include "CImg.h"
#include "loginstance.hpp"
#include "image2.hpp"
#include "image_types.hpp"

using namespace cimg_library;
using namespace logging;
using namespace image_psb;

namespace wavelet
{

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
        *(ptr++) = pow((1.0 + x), 2) * x;
    }
    for(x ; x < 1; x+=dScale)
    {
        *(ptr++) = pow((1.0 - x), 2) * x;
    }

    return ret;
}

inline image_fmt upSample(const image_fmt &input, const int iNewWidth, const int iNewHeight)
{
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

const inline data_fmt getSpline_1_Value(const double x)
{
    if(x < -1 || x > 1) return 0;
    if(x < 0) return  (1.0 + x)*(1.0-x) * (1.0 - 2.0*x);
    if(x >= 0) return (1.0 - x)*(1.0-x) * (1.0 + 2.0*x);
}


const inline data_fmt getSpline_2_Value(const double x)
{
    if(x < -1.0 || x > 1.0) return 0;
    if(x < 0.0) return ( pow(1.0 + x, 2) * x);
    if(x >= 0.0) return ( pow(x - 1, 2) * x);
}

image_fmt getControlMatrix(int iLevel, const data_fmt p1, const data_fmt p2)
{
    data_fmt lowP = p1,
             midP = p2,
             newP = 0;
    const int iLevelCap = pow(2, iLevel + 1);
    image_fmt wavelet1(2,1,1,1), wavelet2(2,1,1,1);
    image_fmt control_matrix(iLevelCap, 1, 1, 1);
        control_matrix(1) = p1; control_matrix(2) = p2;

    data_fmt *updateIndex = control_matrix.data() + 2; // since p1,p2 are in
    for(int iPos = 2; iPos < iLevelCap; iPos++)
    {
        wavelet1.fill(lowP, midP);
        cimglist_for(H, index) newP += H[index].dot(wavelet1);
        // control_matrix(iPos, 1) = newP;
        // cdawontrol_matrix(iPos,  newP1) = 10;
        *(updateIndex++) = newP;
        lowP = midP;
        midP = newP;
        newP = 0;
    }

    return control_matrix;
}

const image_fmt scalingFunction(const int iIndex, const int iWidth)
{
    image_fmt ret(iWidth, 1, 1, 1);
    double scalar,
           pos,
           unitLength = (1.0 / (iWidth + -1));
    pos = unitLength;
    pos = 0;

    switch(iIndex)
    {
        case 1:
            scalar = (5.0 / 24.0);
            cimg_for(ret, ptr, data_fmt)
            {
                *ptr = sqrt(scalar * getSpline_1_Value( (2.0 * pos) - 1));
                pos += unitLength;
            }
            break;
        case 2:
            scalar = (15.0 / 4.0);
            cimg_for(ret, ptr, data_fmt)
            {
                // MLOG(severity_type::debug, "\nPos: ", pos,
                // "\t\tval to sqrt: ", scalar * getSpline_2_Value(2.0 * pos),
                // "\t val: "    , sqrt(scalar * getSpline_2_Value(2.0 * pos)));
                *ptr = sqrt(scalar * getSpline_2_Value(2.0 * pos));
                pos += unitLength;
            }
            break;
        case 3:
            scalar = (15.0 / 8.0);
            cimg_for(ret, ptr, data_fmt)
            {
                //FIXME: wavelets positive
                // MLOG(severity_type::debug, "\nPos: ", pos,
                    // "\t\tval to sqrt: ", scalar * getSpline_2_Value(2.0 * pos- 1),
                    // "\t val: ", -1 * sqrt(cimg::abs(scalar * getSpline_2_Value(2.0 * pos - 1))));
                data_fmt inVal = scalar * getSpline_2_Value(2.0 * pos - 1);
                if(inVal < 0) inVal = cimg::abs(inVal);
                *ptr = sqrt(inVal);
                if(pos < 0.5) *ptr = *ptr * -1;
                pos += unitLength;
            }
            break;
        case 4:
            scalar = (-1.0 * 15.0 / 4.0);
            cimg_for(ret, ptr, data_fmt)
            {
                // MLOG(severity_type::debug, "\nPos: ", pos,
                // "\t\tval to sqrt: ", scalar * getSpline_2_Value(2.0 * pos - 2),
                // "\t val: "    , sqrt(scalar * getSpline_2_Value(2.0 * pos - 2)));
                *ptr = -1.0 * sqrt(scalar * getSpline_2_Value(2.0 * pos - 2));
                pos += unitLength;
            }
            break;
    }
    return ret;
}

image_fmt waveletSpace(const int iLevel, const int iWidth)
{
    const int iLevelCap = pow(2,iLevel);
    image_fmt retWavelet(iWidth, iLevelCap, 1, 1);
    for(int yPos = 0; yPos < 4; yPos++)
    {
        // iterate over each image
    }
    int x = 1;//FIXME
    retWavelet(1) = (pow(2.0, ((-1 * iLevel) / 2.0)) 
                        /
                    sqrt(76.8)
            ) * getSpline_2_Value(pow(2.0, iLevel) * x);

    //TODO: do work here
    for(int kPos = 2; kPos < iLevelCap; kPos++)
    {
        retWavelet(kPos) = (pow(2.0, ((-1 * iLevel) / 2.0)) 
                                    /
                                    sqrt(729.6)
                            ) * getSpline_1_Value(pow(2, iLevel) * x - (kPos / 2));


        retWavelet(kPos) = (pow(2, ((-1 * iLevel) / 2.0)) 
                                    /
                                    sqrt(153.6)
                            ) * getSpline_2_Value(pow(2, iLevel) * x - ((kPos - 1) / 2));
        
    }
    if(iWidth % 2 != 0)
    {
    }
        

    retWavelet(retWavelet.width() - 1) = 
            (pow(2.0, ((-1 * iLevel) / 2.0)) 
                        /
                    sqrt(76.8)
            ) *getSpline_1_Value(pow(2.0, iLevel) * x - pow(2.0, iLevel));
}

data_fmt getP(image_fmt field, image_fmt wavelet)
{
    data_fmt *ptr1 = field.data();
    data_fmt sum = 0;
    cimg_for(wavelet, ptr2, data_fmt)
    {
        sum += *(ptr1++) * *ptr2;
    }

    return sum;
}

void test_splines()
{
    image_fmt test(100, 1, 1, 1);
    double dPos = -1;
    cimg_for(test, ptr, data_fmt)
    {
        *ptr = getSpline_1_Value(dPos);
        dPos += 0.02;
    }
    MLOG(severity_type::debug, "spline1\n", printImage(test));


    dPos = -1; test.fill(0);
    cimg_for(test, ptr, data_fmt)
    {
        *ptr = getSpline_2_Value(dPos);
        dPos += 0.02;
    }
    MLOG(severity_type::debug, "spline2\n", printImage(test));
}

void hermite_wavelet(const image_fmt &field, image_fmt &retImg)
{
    // const data_fmt divisor = (cimg::abs(field.min()) > field.max()) ? field.min() : field.max();
    //TODO: confirm
    const data_fmt divisor = sqrt(field.dot(field));
    image_fmt useField = field.get_crop(1,1,0,0,
            field.width() - 2, field.height() - 2, 0, 0);
    useField.crop(0,0,0,0,
                  3,0,0,0);useField(3) = 0;
    MLOG(severity_type::debug, "field\n", printImage(useField));
    const int iWidth = useField.width();
    // image_fmt spline1 = cubic_spline1(iWidth + 100),
    //           spline2 = cubic_spline2(iWidth);
    // const data_fmt p1 = useField.dot(spline1),
    //                p2 = useField.dot(spline2);
    // image_fmt control_matrix = getControlMatrix(1, p1, p2);

    image_fmt psi1 = scalingFunction(1, iWidth),
              psi2 = scalingFunction(2, iWidth),
              psi3 = scalingFunction(3, iWidth),
              psi4 = scalingFunction(4, iWidth);
    data_fmt  p1 = getP(useField, psi1),
              p2 = getP(useField, psi2),
              p3 = getP(useField, psi3),
              p4 = getP(useField, psi4);
    // test_splines();


    MLOG(severity_type::debug, "psi1\n", printImage(psi1));
    MLOG(severity_type::debug, "psi2\n", printImage(psi2));
    MLOG(severity_type::debug, "psi3\n", printImage(psi3));
    MLOG(severity_type::debug, "psi4\n", printImage(psi4));
    MLOG(severity_type::debug, "\np1: ", p1, "\t\tp2: ", p2, "\t\tp3: ", p3, "\t\tp4: ", p4);
    std::vector<data_fmt> pees;
    pees.push_back(p1);    pees.push_back(p2);  pees.push_back(p3);pees.push_back(p4);

    imageList_fmt testBase(psi1,psi2,psi3,psi4);
    retImg.crop(2, 1, 0, 0, 
                useField.width() + 1, retImg.height() - 2, 0, 0);
    retImg.fill(0);
    int iCount = 0;
    cimg_for(retImg, ptr, data_fmt)
    {
        for(int iPos = 0; iPos < 4; iPos++)
        {
            *ptr += testBase[iPos][iCount++] * pees[iPos];
        }
    }
    // cimglist_for(testBase, i)
    // {
    //     retImg += ( testBase[i] * pees.at(i) ) ;
    // }

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
    }

    } /* EndOfNamespace */
