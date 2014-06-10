#include "../lib/CImg-1.5.8/CImg.h"
#include "solvers/GS/gauss_image.cpp"
#include <iostream>

using namespace cimg_library;
using namespace pe_solver;
using namespace std;

//returns the forward 2nd derivative w.r.t x and y (d2u/dxdy)
CImg<double> Dxyplus(const CImg<double> &image)
{
    CImg<double> newimage(image.width(), image.height(), image.depth(),
            image.spectrum(), 0);

    for (int iPos=0; iPos<image.width()-1; iPos++)
    {
        for (int kPos=0; kPos<image.depth(); kPos++)
        {
            for (int lPos=0; lPos<image.spectrum(); lPos++)
            {
                // Get the difference between raster x-wise neighbour
                // and CUR
                newimage(iPos, image.height() - 1, kPos, lPos)
                    = image(iPos + 1, image.height() - 1, kPos, lPos)
                      - image(iPos, image.height() - 1, kPos, lPos);
                for (int jPos=0; jPos<image.height() - 1; jPos++)
                    // Set the pixel as diff between raster x-wise neighbour
                    // and y-wise neighbour and the combined in CUR
                    newimage(iPos, jPos, kPos, lPos) = 
                        image(iPos + 1, jPos, kPos, lPos)
                        + image(iPos, jPos + 1, kPos, lPos)
                        - 2*image(iPos, jPos, kPos, lPos);
            }
        }
    }
    for (int kPos=0; kPos<image.depth(); kPos++)
    {
        for (int lPos=0; lPos<image.spectrum(); lPos++)
        {
            //
            newimage(image.width() - 1, image.height() - 1, kPos, lPos)
                = image(image.width() - 2, image.height() - 2, kPos, lPos)
                  - image(image.width() - 1, image.height() - 1, kPos, lPos);
            for (int jPos=0; jPos<image.height()-1; jPos++)
            {
                newimage(image.width()-1, jPos, kPos, lPos)
                    = image(image.width() - 1, jPos + 1, kPos, lPos)
                      - image(image.width() - 1, jPos, kPos, lPos);
            }
        }
    }

    return newimage;
}
//returns the 2nd derivative w.r.t x
CImg<double> DoubleDx(const CImg<double> &image)
{
    CImg<double> newimage(image.width(), image.height(), image.depth(),
                          image.spectrum(), 0);
    for (int jPos=0; jPos < image.height(); jPos++)
        for (int kPos=0; kPos < image.depth(); kPos++)
            for (int lPos=0; lPos < image.spectrum(); lPos++)
            {
                for (int iPos=1; iPos < image.width()-1; iPos++)
                    newimage(iPos, jPos, kPos, lPos)
                        = image(iPos + 1, jPos, kPos, lPos)
                          + image(iPos - 1, jPos, kPos, lPos)
                          - 2*image(iPos, jPos, kPos, lPos);
                newimage(0, jPos, kPos, lPos)
                    = image(1, jPos, kPos, lPos)-image(0, jPos, kPos, lPos);
                newimage(newimage.width() - 1, jPos, kPos, lPos)
                    = image(newimage.width() - 2, jPos, kPos, lPos)
                      - image(newimage.width() - 1, jPos, kPos, lPos);
            }
    return newimage;
}
//returns the 2nd derivative w.r.t y
CImg<double> DoubleDy(const CImg<double> &image)
{
    CImg<double> newimage(image.width(), image.height(), image.depth(),
                          image.spectrum(), 0);
    for (int iPos=0; iPos<image.width(); iPos++)
        for (int kPos=0; kPos<image.depth(); kPos++)
            for (int lPos=0; lPos<image.spectrum(); lPos++)
            {
                for (int jPos=1; jPos<image.height()-1; jPos++)
                    newimage(iPos, jPos, kPos, lPos) =
                        image(iPos, jPos+1, kPos, lPos) 
                        + image(iPos, jPos-1, kPos, lPos)-2
                        * image(iPos, jPos, kPos, lPos);

                newimage(iPos, 0, kPos, lPos) = 
                    image(iPos, 1, kPos, lPos) - image(iPos, 0, kPos, lPos);

                newimage(iPos, newimage.height()-1, kPos, lPos) = 
                    image(iPos, newimage.height()-2, kPos, lPos)
                    - image(iPos, newimage.height()-1, kPos, lPos);

            }
    return newimage;

}


//returns the forward x derivative
CImg<double> Dxplus(const CImg<double> &image)
{
	CImg<double> newimage(image.width(), image.height(), image.depth(),
                          image.spectrum(), 0);
	for (int jPos=0; jPos<image.height(); jPos++)
		for (int kPos=0; kPos<image.depth(); kPos++)
			for (int lPos=0; lPos<image.spectrum(); lPos++)
            {
                newimage(image.width()-1, jPos, kPos, lPos) = 
                    (image.width()-2, jPos, kPos, lPos)
                    - (image.width()-1, jPos, kPos, lPos);
                for (int iPos=0; iPos<image.width()-1; iPos++)
                    newimage(iPos, jPos, kPos, lPos) = 
                        image(iPos+1, jPos, kPos, lPos)
                        - image(iPos, jPos, kPos, lPos);
            }
    return newimage;

}
//returns the backward x derivative
CImg<double> Dxminus(const CImg<double> &image)
{
	CImg<double> newimage(image.width(), image.height(), image.depth(), image.spectrum(), 0);
	for (int jPos=0; jPos<image.height(); jPos++)
		for (int kPos=0; kPos<image.depth(); kPos++)
			for (int lPos=0; lPos<image.spectrum(); lPos++)
            {
                newimage(0, jPos, kPos, lPos) = 
                    (1, jPos, kPos, lPos) - (0, jPos, kPos, lPos);
                for (int iPos=1; iPos<image.width(); iPos++)
                    newimage(iPos, jPos, kPos, lPos) = 
                        image(iPos, jPos, kPos, lPos)
                        - image(iPos-1, jPos, kPos, lPos);
            }
    return newimage;

}
//returns the forward y derivative
CImg<double> Dyplus(const CImg<double> &image)
{
	CImg<double> newimage(image.width(), image.height(), image.depth(),
                          image.spectrum(), 0);
	for (int kPos=0; kPos<image.depth(); kPos++)
		for (int lPos=0; lPos<image.spectrum(); lPos++)
			for (int iPos=0; iPos<image.width(); iPos++)
            {
                newimage(iPos, image.height()-1, kPos, lPos) = 
                    image(iPos, image.height()-2, kPos, lPos)
                    - image(iPos, image.height()-1, kPos, lPos);
                for (int jPos=0; jPos<image.height()-1; jPos++)
                    newimage(iPos, jPos, kPos, lPos) = 
                        image(iPos, jPos+1, kPos, lPos)
                        - image(iPos, jPos, kPos, lPos);
            }

    return newimage;

}
//returns the backward y derivative
CImg<double> Dyminus(const CImg<double> &image)
{
    CImg<double> newimage(image.width(), image.height(), image.depth(), image.spectrum(), 0);
    for (int iPos=0; iPos<image.width(); iPos++)
        for (int kPos=0; kPos<image.depth(); kPos++)
            for (int lPos=0; lPos<image.spectrum(); lPos++)
            {
                newimage(iPos, 0, kPos, lPos) = 
                    image(iPos, 1, kPos, lPos)
                    - image(iPos, 0, kPos, lPos);
                for (int jPos=1; jPos<image.height(); jPos++)
                    newimage(iPos, jPos, kPos, lPos) = 
                        image(iPos, jPos, kPos, lPos)
                        - image(iPos, jPos-1, kPos, lPos);
            }
    return newimage;

}

//returns the square of the gradient of the image
CImg<double> gradsq(const CImg<double> &image)
{
	CImg<double> newimage(image.width(), image.height(), image.depth(),
                          image.spectrum(), 0);
	CImg<double> ux=Dxplus(image);
	CImg<double> uy=Dyplus(image);
    for (int iPos=0; iPos<image.width(); iPos++)
		for (int jPos=0; jPos<image.height(); jPos++)
			for (int kPos=0; kPos<image.depth(); kPos++)
				for (int lPos=0; lPos<image.spectrum(); lPos++)
					newimage(iPos, jPos, kPos, lPos) = 
                        (ux(iPos, jPos, kPos, lPos)*ux(iPos, jPos, kPos, lPos)
                         + uy(iPos, jPos, kPos, lPos)*uy(iPos, jPos, kPos, lPos));
	return newimage;
}

//extracts the specific frame of an image, eg for an RGB image, frame 1 is green
CImg<double> getframe(const CImg<double> &image, int frame)
{
	CImg<double> newframe(image.width(), image.height(), image.depth(), 1);
	for (int iPos=0; iPos<image.width(); iPos++)
		for (int jPos=0; jPos<image.height(); jPos++)
			for (int kPos=0; kPos<image.depth(); kPos++)
				newframe(iPos, jPos, kPos, 0)
                    = image(iPos, jPos, kPos, frame);
	return newframe;
}

//randomizes a specific percentage of the image
void addnoise(CImg<double> &image, int percent)
{
	for (int iPos=0; iPos<image.width(); iPos++)
		for (int jPos=0; jPos<image.height(); jPos++)
			for (int kPos=0; kPos<image.depth(); kPos++)
				for (int lPos=0; lPos<image.spectrum(); lPos++)
                {
					if (rand()%(100/percent)==0)
						image(iPos, jPos, kPos, lPos) = (double)(rand() % 255);
				}
	return;
}

CImg<double> inputkernel(double entries[], int iNewDim)
{
    CImg<double> image(entries, iNewDim, iNewDim, 1, 1, false);
    return image;
}


vector<double> poisson2d(int iSize)
{
    int iDim = iSize * iSize;


    for(int iPos = 0; iPos < iDim; iPos++)
    {
        for(int jPos = 0; jPos < iDim; jPos++)
        {
            k = iPos + iDim * jPos;
            

}


int main() {
    CImg<unsigned char> image("./media/image.jpg"),  visu(500, 400, 1, 3, 0);


    //TODO: histogram cimg
    //    : normaliz in cimg..
    //TODO: get column
    double max_error = .9;
    double entries[9] = {0,1,0, 0,-4,1, 0,1,0};
    CImg<double> mask = inputkernel(entries, 3);
    CImg<double> masked = image.get_convolve(mask);
    CImg<double> reshaped = masked.get_vector();

     // Estimate gradient (uses forward finite difference scheme).
    const CImgList<> gradient = image.get_gradient("xy",1);

    // Remember average value for each channel 
    // (needed for normalizing the reconstruction).
    const CImg<> average = image.get_resize(1,1,1,3,2);

    // vector<double> sol(3, 3);
    // vector<double> sol {1, 28, 76};
    // vector<double> initGuess(image.width(), 0);

    // matrix_type x2 = test(image, sol, initGuess, max_error);
	CImgDisplay main_disp(image,"Image",0);	
	CImgDisplay mask_disp(masked,"Image",0);	

    while(!main_disp.is_closed())
    {
        main_disp.wait();
    }
    return 0;
}
