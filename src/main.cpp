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


//TODO: find a better datatype and sparse representation.
CImg<double> poisson2d(const CImg<double> inImage, CImg<double> identity, 
                         int iSize)
{
    iSize = 9;
    CImg<double> retMatrix(iSize, iSize);
    iSize = 3;
    // CImg<double> retMatrix(9, 9);

    for(int iCount = 0; iCount < iSize; iCount++)
    {
        int mult = (iCount * iSize);

    for(int iPos = 0; iPos < iSize; iPos++)
    {
        for(int jPos = 0; jPos < iSize; jPos++)
        {
            retMatrix(iPos+mult, jPos+iSize+mult) = identity(iPos, jPos, 0);
            retMatrix(iPos+mult, jPos+mult) = inImage(iPos, jPos, 0);

            if(iCount == 0) { continue; } // avoid filling first row
            retMatrix(iPos+mult, jPos-iSize+mult) = identity(iPos, jPos, 0);
        }
    }
    }

    // for(int iCount = 0; iCount < iSize; iCount++)
    // {
    //     int mult = (iCount * iSize) - 1;
    //     if(mult < 0) { mult = 0; };
    //
    // for(int iPos = 0; iPos < iSize; iPos++)
    // {
    //     for(int jPos = 0; jPos < iSize; jPos++)
    //     {
    //         // retMatrix(iPos+mult, jPos+iSize+mult) = identity(iPos, jPos, 0);
    //         if(iCount == 0) { continue; } // avoid filling first row
    //         // retMatrix(iPos+iSize, jPos+iSize+mult) = identity(iPos, jPos, 0);
    //     }
    // }
    // }
    return retMatrix;
}


int main() {
    CImg<unsigned char> image("./media/image.jpg"),  visu(500, 400, 1, 3, 0);

    double max_error = .9;
    double D[9] = {0,1,0, 0,-4,1, 0,1,0},
           I[9] = {-1,0,0, 0,-1,0, 0,0,0};
    CImg<double> mask = inputkernel(D, 3);
    //FIXME
    mask(0, 1) = -1;
    CImg<double> identity = inputkernel(I,3);
    identity(2, 2) = 1;
    CImg<double> masked = image.get_convolve(mask);
    CImg<double> F = masked.get_vector();
    //FIXME: mask is wrong
    CImg<double> A = poisson2d(mask, identity, masked.width()*masked.height());

    print_image(A);
    
    //Initial guess shall be zero
    CImg<double> UU(1, masked.width()*masked.height(), 1, 1, 0);

    // matrix_type x2 = test(A, F, UU, max_error);


	// CImgDisplay main_disp(image,"Image",0);	
	// CImgDisplay mask_disp(x2,"Image",0);	

    // while (!main_disp.is_closed() && 
    //        !main_disp.is_keyESC() &&
    //        !main_disp.is_keyQ()) {
    //     main_disp.wait();
    // }
    return 0;
}
