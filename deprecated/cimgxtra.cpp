
/********************************************
** cimgxtra.cpp                            **
** Version 0.51a                           **
** Last update: July 9 2007                **
** Created by Vicknesh Selvam              **
** Please update this section and detail   **
** modifications if this file is modified  **
** Requires: Cimg.h and an image file      **
********************************************/
#include "CImg.h"
#include <iostream>
#include <iomanip>
using namespace cimg_library;

//returns the forward 2nd derivative w.r.t x and y (d2u/dxdy)
CImg<double> Dxyplus(const CImg<double> &image){
	CImg<double> newimage(image.dimx(),image.dimy(),image.dimz(),image.dimv(),0);
	for (int i=0;i<image.dimx()-1;i++){
		for (int k=0; k<image.dimz();k++){
			for (int l=0;l<image.dimv();l++){		
				newimage(i,image.dimy()-1,k,l)=image(i+1,image.dimy()-1,k,l)-image(i,image.dimy()-1,k,l);
				for (int j=0;j<image.dimy()-1;j++)
					newimage(i,j,k,l)=image(i+1,j,k,l)+image(i,j+1,k,l)-2*image(i,j,k,l);
			}
		}
	}	
	for (int k=0; k<image.dimz();k++){
		for (int l=0;l<image.dimv();l++){
			newimage(image.dimx()-1,image.dimy()-1,k,l)= image(image.dimx()-2,image.dimy()-2,k,l)-image(image.dimx()-1,image.dimy()-1,k,l);
			for (int j=0;j<image.dimy()-1;j++){
				newimage(image.dimx()-1,j,k,l)=image(image.dimx()-1,j+1,k,l)-image(image.dimx()-1,j,k,l);
			}
		}
	}

	return newimage;
}
//returns the 2nd derivative w.r.t x
CImg<double> DoubleDx(const CImg<double> &image){
	CImg<double> newimage(image.dimx(),image.dimy(),image.dimz(),image.dimv(),0);
	for (int j=0;j<image.dimy();j++)
		for (int k=0; k<image.dimz();k++)
			for (int l=0;l<image.dimv();l++){
				for (int i=1;i<image.dimx()-1;i++)
					newimage(i,j,k,l)=image(i+1,j,k,l)+image(i-1,j,k,l)-2*image(i,j,k,l);
				newimage(0,j,k,l)=image(1,j,k,l)-image(0,j,k,l);
				newimage(newimage.dimx()-1,j,k,l)=image(newimage.dimx()-2,j,k,l)-image(newimage.dimx()-1,j,k,l);
			}
	return newimage;
}
//returns the 2nd derivative w.r.t y
CImg<double> DoubleDy(const CImg<double> &image){
	CImg<double> newimage(image.dimx(),image.dimy(),image.dimz(),image.dimv(),0);
	for (int i=0;i<image.dimx();i++)
		for (int k=0; k<image.dimz();k++)
			for (int l=0;l<image.dimv();l++){
				for (int j=1;j<image.dimy()-1;j++)
					newimage(i,j,k,l)=image(i,j+1,k,l)+image(i,j-1,k,l)-2*image(i,j,k,l);
                newimage(i,0,k,l)=image(i,1,k,l)-image(i,0,k,l);
				newimage(i,newimage.dimy()-1,k,l)=image(i,newimage.dimy()-2,k,l)-image(i,newimage.dimy()-1,k,l);

			}
	return newimage;

}


//returns the forward x derivative
CImg<double> Dxplus(const CImg<double> &image){
	CImg<double> newimage(image.dimx(),image.dimy(),image.dimz(),image.dimv(),0);
	for (int j=0;j<image.dimy();j++)
		for (int k=0; k<image.dimz();k++)
			for (int l=0;l<image.dimv();l++){
				newimage(image.dimx()-1,j,k,l)=(image.dimx()-2,j,k,l)-(image.dimx()-1,j,k,l);
				for (int i=0;i<image.dimx()-1;i++)
					newimage(i,j,k,l)=image(i+1,j,k,l)-image(i,j,k,l);
			}
	return newimage;

}
//returns the backward x derivative
CImg<double> Dxminus(const CImg<double> &image){
	CImg<double> newimage(image.dimx(),image.dimy(),image.dimz(),image.dimv(),0);
	for (int j=0;j<image.dimy();j++)
		for (int k=0; k<image.dimz();k++)
			for (int l=0;l<image.dimv();l++){
                newimage(0,j,k,l)=(1,j,k,l)-(0,j,k,l);
				for (int i=1;i<image.dimx();i++)
					newimage(i,j,k,l)=image(i,j,k,l)-image(i-1,j,k,l);
			}
	return newimage;

}
//returns the forward y derivative
CImg<double> Dyplus(const CImg<double> &image){
	CImg<double> newimage(image.dimx(),image.dimy(),image.dimz(),image.dimv(),0);
	for (int k=0; k<image.dimz();k++)
		for (int l=0;l<image.dimv();l++)
			for (int i=0;i<image.dimx();i++){
                newimage(i,image.dimy()-1,k,l)=image(i,image.dimy()-2,k,l)-image(i,image.dimy()-1,k,l);
				for (int j=0;j<image.dimy()-1;j++)
					newimage(i,j,k,l)=image(i,j+1,k,l)-image(i,j,k,l);
			}

	return newimage;

}
//returns the backward y derivative
CImg<double> Dyminus(const CImg<double> &image){
	CImg<double> newimage(image.dimx(),image.dimy(),image.dimz(),image.dimv(),0);
	for (int i=0;i<image.dimx();i++)
		for (int k=0; k<image.dimz();k++)
			for (int l=0;l<image.dimv();l++){
				newimage(i,0,k,l)=image(i,1,k,l)-image(i,0,k,l);
				for (int j=1;j<image.dimy();j++)
					newimage(i,j,k,l)=image(i,j,k,l)-image(i,j-1,k,l);
			}
	return newimage;

}

//returns the square of the gradient of the image
CImg<double> gradsq(const CImg<double> &image){
	CImg<double> newimage(image.dimx(),image.dimy(),image.dimz(),image.dimv(),0);
	CImg<double> ux=Dxplus(image);
	CImg<double> uy=Dyplus(image);
    for (int i=0;i<image.dimx();i++)
		for (int j=0;j<image.dimy();j++)
			for (int k=0; k<image.dimz();k++)
				for (int l=0;l<image.dimv();l++)
					newimage(i,j,k,l)=(ux(i,j,k,l)*ux(i,j,k,l)+uy(i,j,k,l)*uy(i,j,k,l));

	return newimage;

}


//extracts the specific frame of an image, eg for an RGB image, frame 1 is green
CImg<double> getframe(const CImg<double> &image,int frame){
	CImg<double> newframe(image.dimx(),image.dimy(),image.dimz(),1);
	for (int i=0;i<image.dimx();i++)
		for (int j=0;j<image.dimy();j++)
			for (int k=0; k<image.dimz();k++)
				newframe(i,j,k,0)=image(i,j,k,frame);
	return newframe;

}

//randomizes a specific percentage of the image
void addnoise(CImg<double> &image,int percent){
	for (int i=0;i<image.dimx();i++)
		for (int j=0;j<image.dimy();j++)
			for (int k=0; k<image.dimz();k++)
				for (int l=0;l<image.dimv();l++){	
					if (rand()%(100/percent)==0)
						image(i,j,k,l)=(double)(rand()%255);
				}
	return;
}

//normalizes each colour, i.e. sum of all values in each colour's matrix=1
void normalize (CImg<double> &input){
	double total[3]={0,0,0};
	for (int i=0;i<input.dimx();i++){
		for (int j=0;j<input.dimy();j++){
			for (int k=0;k<3;k++){
				total[k]+=input(i,j,0,k);
			}
		}
	}

	for (int i=0;i<input.dimx();i++){
		for (int j=0;j<input.dimy();j++){
			for (int k=0;k<3;k++){
				input(i,j,0,k)/=total[k];
			}
		}
	}
	return;
}

//output's matrix in raster order, first red, then green, then blue
void outputvalues (const CImg<double> &matrix){
	
	if (matrix.dimz()==3)
	for (int k=0;k<matrix.dimz();k++){
		if (matrix.dimz()==3)
		std:: cout << "\n";
		for (int i=0;i< matrix.dimx() ;i++){
			std::cout << "\n";
			for (int j=0;j< matrix.dimy() ;j++){
				std::cout << std::setw(10) << matrix(i,j,0,k) << " ";
			}
		}
	}
    return;
}

//takes in an array, and the square root of its sizes, and creates a matrix out of it, with all 3 colours of the same value
CImg<double> inputmatrix(double input[], int entsizes){
	CImg<double> newmatrix(entsizes,entsizes,1,3,0.0);//creates new image
	for (int i=0;i<entsizes;i++)
		for (int j=0;j<entsizes;j++)
			for (int k=0;k<3;k++)
				newmatrix(i,j,0,k)=input[j+i*entsizes];
    return newmatrix;
}
//creates borders of  length degree around an image 
CImg<double> borders(const CImg<double> &image,int degree){
    //copy image to temp newim with black border
	CImg<double> newim(image.dimx()+2*degree,image.dimy()+2*degree,image.dimz(),image.dimv());
    for (int i=0;i<image.dimx();i++)
        for (int j=0;j<image.dimy();j++)
            for (int k=0;k<3;k++)
                newim(i+degree,j+degree,0,k)=image(i,j,0,k);
   
	for (int i=0;i<newim.dimx();i++){//copy 1st and last rows of image onto remaining rows of newim repectively(creates top/bottom borders)
        for (int j=0;j<3;j++){
            for (int k=0;k<=degree;k++){
                newim(i,k,0,j)=newim(i,degree,0,j);
                newim(i,newim.dimy()-k-1,0,j)=newim(i,image.dimy()-1+degree,0,j);   
            }
        }
    }
    for (int i=0;i<newim.dimy();i++){//copy 1st and last cols onto 1st and last repectively(creates left/right borders
        for (int j=0;j<3;j++){
            for (int k=0;k<=degree;k++){
                    newim(k,i,0,j)=newim(degree,i,0,j);
                    newim(newim.dimx()-k-1,i,0,j)=newim(image.dimx()-1+degree,i,0,j);
            }
        }
    }
    return newim;
}

//linear kernel filter, returns a matrix
CImg<double> convolve(const CImg<double> &image,const CImg<double> &kernel){
    CImg<double> expand=borders(image,(kernel.dimx()-1)/2);
    CImg<double> filtered(image.dimx(),image.dimy(),image.dimz(),image.dimv(),0); //creates new image, all values zero.
 
	for (int i=0;i<filtered.dimx();i++)
        for (int j=0;j<filtered.dimy();j++)
            for (int k=0;k<3;k++)
                for(int l=0;l<kernel.dimx();l++)
                    for (int m=0;m<kernel.dimy();m++)
                        filtered(i,j,0,k)+=(expand(i+l,j+m,0,k))*kernel(l,m,0,k);//convolution function

    return filtered;
}

int main() {	
	CImg<double> image("starwars1.bmp");  
	CImgDisplay main_disp(image,"Image",0);	
	CImg<double> darkimage(image.dimx(),image.dimy(),1,3,0);//Darkened image, Chapter 2
	for (int i=0;i<image.dimx();i++)		
		for (int j=0;j<image.dimy();j++)	
			for (int k=0;k<3;k++)			
				darkimage(i,j,0,k)=image(i,j,0,k)/2;    
	CImgDisplay dark_disp (darkimage,"Dark Image",0);
	int dim=5;	
	CImg<double> kern(dim,dim,1,3,2);  
	//The last value can be any  
	//positive number , since we�re normalizing
	normalize(kern);    
	CImgDisplay main_conv(convolve(image,kern),"Meanfilter 5",0);//Convolution: Chapter 3

	CImg<double> greyimage=getframe(image,1);
	addnoise(greyimage,2);	

	//Chapter 4: Denoising
	CImgDisplay grey_disp (greyimage,"Image with noise, Green frame",0);

	CImg<double> newu=greyimage;
	CImg<double> new2u=greyimage;//for isotropic denoising
	CImg<double> temp2x=greyimage;
	CImg<double> temp2y=greyimage;
	CImg<double> tempx=greyimage;
	CImg<double> tempy=greyimage;
	CImg<double> ones(greyimage.dimx(),greyimage.dimy(),greyimage.dimz(),greyimage.dimv(),1.0);
	CImg<double> denom=greyimage;
	double dt=.25;
	for (double t=0;t<10;t+=dt){
		tempx=Dxplus(newu);
		tempy=Dyplus(newu);
		denom=ones + gradsq(newu);
		denom.sqrt();
		tempx.div(denom);
		tempy.div(denom);
		temp2x=Dxminus(tempx);
		temp2y=Dyminus(tempy);
		newu = newu + dt*(temp2x + temp2y);  
		new2u = new2u + dt*(DoubleDx(new2u) + DoubleDy(new2u) );//isotropic
	}
	CImgDisplay iso_disp (new2u,"Image denoised isotropically",0);
	CImgDisplay anis_disp (newu,"Image denoised anisotropically",0);

	while (!main_disp.is_closed) {	
		main_disp.wait();	
	}
	return 0;
}