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
    //     iWidthShift  = (ret.width() % 2 == 1) ? ret.width()   + 2: ret.width();
    int iHeightStop = (ret.height() % 2 == 1) ? ret.height() + 0: ret.height(),
        iWidthShift  = (ret.width() % 2 == 1) ? ret.width()   + 0: ret.width();
    for(int yPos = 0; yPos < iHeightStop; yPos += 2)
    {
        for(int xPos = 0; xPos < iWidthShift; xPos+= 2)
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

const inline data_fmt getSplineValue_1(const double x)
{
    if(x < -1 || x > 1) return 0;
    if(x <  0) return  (1.0 + x)*(1.0+x) * (1.0 - 2.0*x);
    if(x >= 0) return  (1.0 - x)*(1.0-x) * (1.0 + 2.0*x);
}


const inline data_fmt getSplineValue_2(const double x)
{
    if(x < -1.0 || x > 1.0) return 0;
    if(x < 0.0) return  ( pow(1.0 + x, 2) * x);
    if(x >= 0.0) return ( pow(x - 1.0, 2) * x);
}

inline data_fmt getMotherWavelet_1(const double x)
{
    return (
            - 2.0 * getSplineValue_1(2.0*x + 1)
            + 4.0 * getSplineValue_1(2.0*x)
            - 2.0 * getSplineValue_1(2.0*x-1)
            - 21.0 * getSplineValue_2(2.0*x+1)
            + 21 * getSplineValue_2(2.0*x-1)
           );
}

inline data_fmt getMotherWavelet_2(const double x)
{
    return (
              getSplineValue_1(2.0*x + 1)
            - getSplineValue_1(2.0*x - 1)
            + 9.0 * getSplineValue_2(2.0*x+1)
            + 12.0 * getSplineValue_2(2.0*x)
            + 9.0 * getSplineValue_2(2.0*x-1)
           );
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

// shift = -1, pos = 0 has some interesting properties
const image_fmt scalingFunction(const int iIndex, const int iWidth)
{
    image_fmt ret(iWidth, 1, 1, 1);
    int iWidthShift = -1;
    double pos, scalar;
    const double unitLength = (1.0 / (iWidth + iWidthShift));
    MLOG(severity_type::debug, "Unit step size: ", unitLength, " from width ", iWidth);
    pos = 0;
    // pos = unitLength;

    int iPos = 1;

    switch(iIndex)
    {
        case 1:
            scalar = sqrt(5.0 / 24.0);
            cimg_for(ret, ptr, data_fmt)
            {
                // pos = (double) iPos++ / unitLength;
                *ptr = scalar * getSplineValue_1( (2.0 * pos) - 1);
                pos += unitLength;
            }
            break;
        case 2:
            scalar = sqrt(15.0 / 4.0);
            cimg_for(ret, ptr, data_fmt)
            {
                // pos = (double) iPos++ / unitLength;
                // MLOG(severity_type::debug, "\nPos: ", pos,
                // "\t\tval to sqrt: ", scalar * getSplineValue_2(2.0 * pos),
                // "\t val: "    , sqrt(scalar * getSplineValue_2(2.0 * pos)));
                *ptr = scalar * getSplineValue_2(2.0 * pos);
                pos += unitLength;
            }
            break;
        case 3:
            scalar = sqrt(15.0 / 8.0);
            cimg_for(ret, ptr, data_fmt)
            {
                // pos = (double) iPos++ / unitLength;
                //FIXME: wavelets positive
                // MLOG(severity_type::debug, "\nPos: ", pos,
                    // "\t\tval to sqrt: ", scalar * getSplineValue_2(2.0 * pos- 1),
                    // "\t val: ", -1 * sqrt(cimg::abs(scalar * getSplineValue_2(2.0 * pos - 1))));
                *ptr = scalar * getSplineValue_2(2.0 * pos - 1);
                pos += unitLength;
            }
            break;
        case 4:
            scalar = sqrt(15.0 / 4.0);
            cimg_for(ret, ptr, data_fmt)
            {
                // pos = (double)iPos++ / unitLength;
                // MLOG(severity_type::debug, "\nPos: ", pos,
                // "\t\tval to sqrt: ", scalar * getSplineValue_2(2.0 * pos - 2),
                // "\t val: "    , sqrt(scalar * getSplineValue_2(2.0 * pos - 2)));
                *ptr = scalar * getSplineValue_2(2.0 * pos - 2);
                pos += unitLength;
            }
            if(pos - unitLength > 1)
               MLOG(severity_type::warning, "pos out of scale in scaling function");
            break;
    }
    return ret;
}

image_fmt waveletSpace(const int iLevel, const int k, const int iWidth)
{
    image_fmt retWavelet(iWidth, 1, 1, 1);
    int iWidthShift = +2;
    const double unitLength = (1.0 / (iWidth + iWidthShift)),
                indexShift = pow(2.0, iLevel);
    double x = unitLength;
    // x = 0;

    if(k == 1) /* Base case, first level */
    {
        const double LHS = pow(2.0, ( (-1 * iLevel) / 2.0)) / sqrt(76.8);
        cimg_for(retWavelet, ptr, data_fmt)
        {
            *ptr = LHS * getMotherWavelet_2(indexShift * x);
            x += unitLength;
        }
    }
    else if(k == pow(2, iLevel+1)) /* Base case, last level */
    {
        const double LHS = pow(2.0, ( (-1 * iLevel) / 2.0)) / sqrt(76.8);
        cimg_for(retWavelet, ptr, data_fmt)
        {
            *ptr = LHS * getMotherWavelet_2(indexShift * x - indexShift);
            x += unitLength;
        }
    }

    else if(k % 2 == 0) /* Even numbers */
    {
        const double LHS = pow(2.0, ((-1 * iLevel) / 2.0)) / sqrt(729.6);
        cimg_for(retWavelet, ptr, data_fmt)
        {
            *ptr = LHS * getMotherWavelet_1(indexShift * x - (k / 2));
            x += unitLength;
        }
    }
    else /* Odd numbers of k */
    {
        const double LHS = pow(2, ((-1 * iLevel) / 2.0)) / sqrt(153.6);

        cimg_for(retWavelet, ptr, data_fmt)
        {
            *ptr = LHS * getMotherWavelet_2(indexShift * x - ((k - 1) / 2));
            x += unitLength;
        }
    }

    return retWavelet;
}

void test_splines(const int iSteps)
{
    image_fmt test(iSteps, 1, 1, 1);
    double dPos = -1;
    double dStepSize = 2 / (double)iSteps;
    cimg_for(test, ptr, data_fmt)
    {
        *ptr = getSplineValue_1(dPos);
        dPos += dStepSize;
    }
    MLOG(severity_type::debug, "spline1\n", printImage(test));


    dPos = -1; test.fill(0);
    cimg_for(test, ptr, data_fmt)
    {
        *ptr = getSplineValue_2(dPos);
        dPos += dStepSize;
    }
    MLOG(severity_type::debug, "spline2\n", printImage(test));
}

void test_motherWavelet(const int iWidth)
{
    image_fmt test(iWidth, 1, 1, 1);
    double dPos = -1;
    double dStepSize = 2 / (double)iWidth;
    cimg_for(test, ptr, data_fmt)
    {
        *ptr = getMotherWavelet_1(dPos);
        dPos += dStepSize;
    }


    dPos = -1;
    image_fmt test2(iWidth, 1, 1, 1);
    cimg_for(test2, ptr, data_fmt)
    {
        *ptr = getMotherWavelet_2(dPos);
        dPos += dStepSize;
    }
    MLOG(severity_type::debug, "Testing mother wavelet 1,2\n", printImage(test)
                                ,"\n",printImage(test2));
}

void test_scales(int iWidth)
{
    const image_fmt psi1 = scalingFunction(1, iWidth),
                    psi2 = scalingFunction(2, iWidth),
                    psi3 = scalingFunction(3, iWidth),
                    psi4 = scalingFunction(4, iWidth);
    MLOG(severity_type::debug, "testing psi's in respective order 1 to 4\n",
        printImage(psi1),"\n", printImage(psi2),"\n", printImage(psi3),"\n",
        printImage(psi4));
}

void test_wavelets(const int iWidth)
{
    const image_fmt wav1 = waveletSpace(1,1, iWidth),
                    wav2 = waveletSpace(1,2, iWidth),
                    wav3 = waveletSpace(1,3, iWidth),
                    wav4 = waveletSpace(1,4, iWidth);
    MLOG(severity_type::debug, "testing wav's in respective order 1 to 4\n",
        printImage(wav1),"\n", printImage(wav2),"\n", printImage(wav3),"\n",
        printImage(wav4));
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
    useField = field;
    // useField.assign(8,1,1,1,
    //         10, 100, 100, 100,
    //         100, 100, 100, 10);
            // -10, -100, -100, -100,
            // 100, 100, 100, 10);



    MLOG(severity_type::debug, "field\n", printImage(useField));
    const int iWidth = useField.width();
    // image_fmt spline1 = cubic_spline1(iWidth + 100),
    //           spline2 = cubic_spline2(iWidth);
    // const data_fmt p1 = useField.dot(spline1),
    //                p2 = useField.dot(spline2);
    // image_fmt control_matrix = getControlMatrix(1, p1, p2);

    const image_fmt psi1 = scalingFunction(1, iWidth),
                    psi2 = scalingFunction(2, iWidth),
                    psi3 = scalingFunction(3, iWidth),
                    psi4 = scalingFunction(4, iWidth);
    const data_fmt  p1 = useField.dot(psi1),
                    p2 = useField.dot(psi2),
                    p3 = useField.dot(psi3),
                    p4 = useField.dot(psi4);

    MLOG(severity_type::debug, "psi1\n", printImage(psi1));
    MLOG(severity_type::debug, "psi2\n", printImage(psi2));
    MLOG(severity_type::debug, "psi3\n", printImage(psi3));
    MLOG(severity_type::debug, "psi4\n", printImage(psi4));
    test_splines(100);
    test_scales(100);
    test_wavelets(100);
    // test_motherWavelet(100);
    MLOG(severity_type::debug, "\np1: ", p1, "\t\tp2: ", p2, "\t\tp3: ", p3, "\t\tp4: ", p4);
    std::vector<data_fmt> Phi;
    Phi.push_back(p1);    Phi.push_back(p2);  Phi.push_back(p3);Phi.push_back(p4);

    imageList_fmt testBase(psi1,psi2,psi3,psi4);
    retImg.crop(2, 1, 0, 0, 
                useField.width() + 1, retImg.height() - 2, 0, 0);
    retImg.fill(0);
    int iImageIndex = 0;
    data_fmt sum = 0;
    cimg_for(retImg, ptr, data_fmt)
    {
        sum = 0;
        for(int iPos = 0; iPos < 4; iPos++)
        {
            sum += useField.dot(testBase[iPos]) * testBase[iPos][iImageIndex];
        }
        *ptr = sum;
        iImageIndex++;
    }
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
    const int SCALE = 2;
    const int iForwMaskWidth = forward_mask.width(),
              iBackMaskWidth = backward_mask.width();



    void wavelet_7x7(const image_fmt &field, image_fmt &retImg)
    {
        const int iMaxLevel = ceil(cimg::log2(cimg::max(field.width(), field.height())))
                            - 1;

        image_fmt initPyr = downSample(field.width() + iForwMaskWidth * 2,
                                    field.height() + iForwMaskWidth  * 2,
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
        forw_pyramid.push_back(downSample(imgCore.width() + iForwMaskWidth * 2,
                                    imgCore.height() + iForwMaskWidth * 2,
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
                    back_pyramid.back().width() - iBackMaskWidth - 1,
                    back_pyramid.back().height() - iBackMaskWidth - 1, 0, 0);
            imgCore = upSample(imgCore,
                    forw_pyramid.back().width(), forw_pyramid.back().height());
            back_pyramid.push_back(
                    imgCore.get_convolve(backward_mask)
                    + forw_pyramid.back().get_convolve(g));
            forw_pyramid.pop_back();
        }
        retImg = back_pyramid.back().get_crop(
                    iBackMaskWidth, backward_mask.height(), 0, 0,
                    back_pyramid.back().width() - iBackMaskWidth - 1,
                    back_pyramid.back().height() - iBackMaskWidth - 1, 0, 0);
    }

    } /* EndOfNamespace */
