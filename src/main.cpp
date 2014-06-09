#include "../lib/CImg-1.5.8/CImg.h"
#include <iostream>

using namespace cimg_library;
using namespace std;


CImg<double> expand_matrix(CImg<double>image, int degree)
{
}

template <typename T>
CImg< T > convolute(CImg< T > image,  CImg<double> kernel, int iDegree)
{
    CImg<double> expand=expandmatrix(image, (iDegree - 1) /2);
}

void normalize(CImg<double> kernel, int iDim)
{
    for (int iPos = 0; iPos < kernel.width(); iPos++) 
    {
        double dRowSum = 0;
        for (int jPos = 0; jPos < kernel.height(); jPos++) 
        {
             dRowSum += kernel(iPos, jPos, 0, iDim);
        }
        for (int jPos = 0; jPos < kernel.height(); jPos++) 
        {
            kernel(iPos, jPos, 0, iDim) /= dRowSum;
        }
    }

}

int main() {
    CImg<unsigned char> image("./media/image.jpg"), visu(500,400,1,3,0);
    int iDim  =  5;
    double dEntries[9] = {0, 1, 0, 1, -4, 1, 0, 1, 0};
    CImg<double> kern(dEntries, 3);
    normalize(kern, iDim);
    CImgDisplay main_disp(image,  "Image",  0);
    CImgDisplay main_conv(convolute(image, kern, iDim),  "MeanFilter 5", 0);

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
