#include "../lib/CImg-1.5.8/CImg.h"
#include "solvers/GS/gauss_image.cpp"
#include <iostream>

using namespace cimg_library;
using namespace pe_solver;
using namespace std;

//returns the forward 2nd derivative w.r.t x and y (d2u/dxdy)
CImg<double> inputkernel(double entries[], int iNewDim)
{
    CImg<double> image(entries, iNewDim, iNewDim, 1, 1, false);
    return image;
}

CImg<double> inputkernel(double entries[], int iNewWidth, int iNewHeight)
{
    CImg<double> image(entries, iNewWidth, iNewHeight, 1, 1, false);
    return image;
}

int main() {
    //visu is just the background for the preview
    // CImg<unsigned char> image("./media/icon_img.png"),  visu(500, 400, 1, 3, 0);
    CImg<unsigned char> image("./media/109201.jpg"),  visu(499, 400, 1, 3, 0);

    double max_error = 1;
    double D[9] = {0,1,0, 0,-4,1, 0,1,0};
    CImg<double> mask = inputkernel(D, 3);
    //FIXME: specified wrong order
    mask(0, 1) = 1;
    CImg<double> masked = image.get_convolve(mask);
    CImg<double> F = masked.get_vector();
    //Initial guess shall be the zero-vector
    CImg<double> UU(1, F.height(), 1, 1, 0); 

#include <unistd.h>
    matrix_type x2 = test(F, UU, max_error, image.width());


	// CImgDisplay main_disp(image,"Image",0);	
	// CImgDisplay mask_disp(x2,"Image",0);	
    //
    // while (!main_disp.is_closed() && 
    //        !main_disp.is_keyESC() &&
    //        !main_disp.is_keyQ()) {
    //     main_disp.wait();
    // }
    //
    return 0;
}
