#include "../lib/CImg-1.5.8/CImg.h"
#include <iostream>

using namespace cimg_library;
using namespace std;


CImg<double> expand_matrix(CImg<double>image, int iDegree)
{

    return image;
}

CImg<double> borders(const CImg<double> &image,int degree){
    //copy image to temp newim with black border
    CImg<double> newim(image.width()  + (2*degree),
                       image.height() + (2*degree),
                       image.depth(), image.spectrum());
    for (int iPos=0; iPos<image.width(); iPos++)
        for (int jPos=0; jPos<image.height(); jPos++)
            for (int kPos=0; kPos<3; kPos++)
                newim(iPos+degree, jPos+degree, 0, kPos) =
                    image(iPos,jPos,0,kPos);

    //copy 1st and last rows of image onto remaining rows of newim repectively(creates top/bottom borders)
    for (int iPos=0; iPos<newim.width(); iPos++)
    {
        for (int jPos=0; jPos<3; jPos++)
        {
            for (int kPos=0; kPos<=degree; kPos++)
            {
                newim(iPos,kPos,0,jPos) = newim(iPos,degree,0,jPos);
                newim(iPos, newim.height()-kPos-1, 0, jPos) = 
                    newim(iPos, image.height() - 1 + degree, 0, jPos);   
            }
        }
    }
    //copy 1st and last cols onto 1st and last repectively(creates left/right borders
    for (int iPos=0; iPos < newim.height(); iPos++)
    {
        for (int jPos=0; jPos<3; jPos++)
        {
            for (int kPos=0; kPos<=degree; kPos++)
            {
                newim(kPos,iPos,0,jPos) = newim(degree, iPos, 0, jPos);
                newim(newim.width() - kPos - 1, iPos, 0, jPos) =
                    newim(image.width() - 1 + degree, iPos, 0, jPos);
            }
        }
    }
    return newim;
}

CImg<double> convolve(const CImg<double> &image,const CImg<double> &kernel)
{
    CImg<double> expand=borders(image,(kernel.width()-1)/2);
    // all values to last parameter (0)
    CImg<double> filtered(image.width(),image.height(),image.depth(),
                          image.spectrum(),0); 
 
    for (int iPos=0;iPos<filtered.width();iPos++)
    {
        for (int jPos=0;jPos<filtered.height();jPos++)
        {
            for (int kPos=0;kPos<3;kPos++)
            {
                for(int lPos=0;lPos<kernel.width();lPos++)
                {
                    for (int mPos=0;mPos<kernel.height();mPos++)
                    {
                        filtered(iPos,jPos,0,kPos)+=
                            (expand(iPos + lPos, jPos + mPos, 0, kPos))
                            *kernel(lPos, mPos, 0, kPos);
                    }
                }
            }
        }
    }

    return filtered;
}

template <typename T>
CImg< T > convolute(CImg< T > image,  CImg<double> kernel, int iDegree)
{
    CImg<double> expand = expand_matrix(image, (iDegree - 1) /2);

    return expand;
}

void normalize(CImg<double> &input)
{
    double dTotal[3] = {0, 0, 0};

    for(int iPos = 0; iPos < input.width(); iPos++)
    {
        for(int jPos = 0; jPos < input.height(); jPos++)
        {
            for(int kPos = 0; kPos < 3; kPos++)
            {
                dTotal[kPos] += input(iPos, jPos, 0, kPos);
            }
        }
    }

    for(int iPos = 0; iPos < input.width(); iPos++)
    {
        for(int jPos = 0; jPos < input.height(); jPos++)
        {
            for(int kPos = 0; kPos < 3; kPos++)
            {
                input(iPos, jPos, 0, kPos) /= dTotal[kPos];
            }
        }
    }
}

// CImg<double> normalize(CImg<double> kernel, int iDim)
// {
//     for (int iPos = 0; iPos < kernel.width(); iPos++) 
//     {
//         double dRowSum = 0;
//         for (int jPos = 0; jPos < kernel.height(); jPos++) 
//         {
//              dRowSum += kernel(iPos, jPos, 0, iDim);
//         }
//         for (int jPos = 0; jPos < kernel.height(); jPos++) 
//         {
//             kernel(iPos, jPos, 0, iDim) /= dRowSum;
//         }
//
//     }
//     return kernel;
//
// }

int main() {
    CImg<unsigned char> image("./media/image.jpg"), visu(500,400,1,3,0);
    int iDim  =  3;
    CImg<double> kern(iDim, iDim, 1, 3, 2);
    normalize(kern);
    CImgDisplay main_disp(image,  "Image",  0);
    // CImgDisplay main_conv(convolute(image, kern, iDim),  "MeanFilter 5", 0);
    CImgDisplay main_conv(convolve(image, kern),  "MeanFilter 5", 0);

    while(!main_disp.is_closed())
    {
        main_disp.wait();
    }

    // const unsigned char red[] = { 255,0,0 }, green[] = { 0,255,0 }, blue[] =
    // { 0,0,255 }; image.blur(2.5);
    // CImgDisplay main_disp(image,"Click a point"),
    //                      draw_disp(visu,"Intensity profile");
    // while (!main_disp.is_closed() && !draw_disp.is_closed()) {
    //     main_disp.wait();
    //     if (main_disp.button() && main_disp.mouse_y()>=0) {
    //         const int y = main_disp.mouse_y();
    //         visu.fill(0).draw_graph(image.get_crop(0,y,0,0,image.width()-1,y,0,0),red,1,1,0,255,0);
    //         visu.draw_graph(image.get_crop(0,y,0,1,image.width()-1,y,0,1),green,1,1,0,255,0);
    //         visu.draw_graph(image.get_crop(0,y,0,2,image.width()-1,y,0,2),blue,1,1,0,255,0).display(draw_disp);
    //     }
    // }
    return 0;
}
