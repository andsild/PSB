#include "imageedit.hpp"

#include <iostream>

#include <stdlib.h>
#include <string>
#include <vector>

#include <stdio.h>

#include "CImg.h"

#include <file.hpp>
#include "loginstance.hpp"
#include "image2.hpp"

using namespace cimg_library;
using namespace logging;
using namespace image_psb;


namespace image_display
{

int ImageContainer::iSolvedIndex = 0;
int ImageContainer::iResolvedIndex = 0;

ImageContainer::ImageContainer(std::string fileName)
               : fileName(fileName)
{
    this->iSolvedIndex = 0;
    this->iResolvedIndex = 0;
}

std::string ImageContainer::getFileName() const
{
    std::string sCopy = this->fileName;
    file_IO::trimLeadingFileName(sCopy);
    return sCopy;
}

void ImageContainer::addSolverImage(std::string fileName)
{
    std::cout << " pushed image " << fileName  << " back" << std::endl;
    this->vSolvedImages.push_back(fileName);
}

void ImageContainer::addResolvedImage(std::string fileName)
{
    this->vResolvedImages.push_back(fileName);
}

bool ImageContainer::hasResolved() const
{
    return (this->vResolvedImages.size() > 0);
}
std::string ImageContainer::getMain() const
{
    return this->fileName;
}
std::string ImageContainer::getSolved()
{
    if(this->iSolvedIndex >= this->vSolvedImages.size()
    || this->iSolvedIndex < 0)
    {
        std::string sErr= "lookup for solved image to " + this->fileName + " failed. ";
        throw image_psb::ImageException(sErr);
    }
    return this->vSolvedImages.at(iSolvedIndex);
}
std::string ImageContainer::getResolved() const
{
    return this->vResolvedImages[iResolvedIndex];
}
std::string ImageContainer::getNextSolver() 
{
    this->iSolvedIndex++;
    std::cout << this->vSolvedImages.size() << std::endl;
    std::cout << this->iSolvedIndex << std::endl;
    if(this->iSolvedIndex >= this->vSolvedImages.size())
        this->iSolvedIndex = 0;
    std::string sElem = this->vSolvedImages[iSolvedIndex];
    return sElem;
}

std::string ImageContainer::getNextResolver() 
{
    this->iResolvedIndex++;
    if(this->iResolvedIndex == this->vResolvedImages.size())
        iResolvedIndex = 0;
    return this->vResolvedImages[iResolvedIndex];
}

ImageDisplay::ImageDisplay()
{
    int iIndex = 0;
    visu.assign(500, 300, 1, 1, 0);
    graph_disp = visu;
    graph_disp.set_title("Color intensities");
}

void ImageDisplay::show()
{
    ImageContainer inst = this->vMainImages[this->iIndex];
    std::string mainFile, solver;
    try
    {
        mainFile = inst.getMain();
        solver = inst.getSolved();
    }
    catch(image_psb::ImageException ie)
    {
        LOG(severity_type::error)(ie.what());
        CLOG(severity_type::error)(ie.what());
        exit(EXIT_FAILURE);
    }

    this->main_image.assign(mainFile.c_str());
    this->solved_image.assign(solver.c_str());

    this->main_disp = this->main_image;
    this->main_disp.set_title(mainFile.c_str());
    this->solved_disp = this->solved_image;
    this->solved_disp.set_title(solver.c_str());
}

ImageContainer ImageDisplay::getCurrent()
{
    return this->vMainImages.at(this->iIndex);
}

void ImageDisplay::nextImage()
{
    this->iIndex++;
    if(this->iIndex == this->vMainImages.size())
        iIndex = 0;
    this->show();
}

void ImageDisplay::prevImage()
{
    this->iIndex--;
    if(this->iIndex == -1)
        iIndex = this->vMainImages.size() - 1;
    this->show();
}

void ImageDisplay::nextSolver()
{
    ImageContainer ic = this->getCurrent();
    std::string sNextFile = ic.getNextSolver();
    this->solved_image.assign(sNextFile.c_str());
    this->solved_disp = this->solved_image;
}

void ImageDisplay::loop()
{
    const double blackWhite[] = {255};

    while (!this->main_disp.is_closed() && 
           !this->solved_disp.is_closed() && 
           !this->graph_disp.is_closed())
    {
        ImageContainer cur = this->getCurrent();
        if (main_disp.button() && main_disp.mouse_y()>=0) 
        {
            const int yPos = this->main_disp.mouse_y();
            image_fmt main_cropped =  this->main_image.get_crop(
                    0, yPos, 0, 0, this->main_image.width()-1, yPos, 0, 0);
            image_fmt side_cropped = this->solved_image.get_crop(
                    0, yPos, 0, 0, solved_image.width()-1, yPos, 0, 0);
    
            // data, color, opacity, plot_type, verttex_type, ymin
            visu.fill(0).draw_graph(main_cropped, blackWhite, 1, 1, 0, 255, 0);
            visu.draw_graph(side_cropped, blackWhite, 1, 1, 0, 255, 0);
            visu.display(graph_disp);
        }
        switch (main_disp.key()) 
        {
            case cimg::keyARROWUP:
                this->nextImage();
                break;
            case cimg::keyARROWDOWN:
                this->prevImage();
                break;
            // case cimg::keyARROWLEFT:
            //     this->nextSolver();
            //     break;
        //         if(iIndex == 0) { iIndex = out.size(); }
        //         iIndex--;
        //         if(!loadImage(out[iIndex], solved_image)) {break; }
        //         mask_disp = solved_image;
        //         mask_disp.set_title(out[iIndex].c_str());
        //         // mask_disp.resize();
        //         break;
            case cimg::keyARROWRIGHT:
                this->nextSolver();
                break;
        //         if(iIndex == out.size() - 1) { iIndex = -1; }
        //         iIndex++;
        //         if(!loadImage(out[iIndex], solved_image)) {break; }
        //         mask_disp = solved_image;
        //         mask_disp.set_title(out[iIndex].c_str());
        //         // mask_disp.resize();
        //         break;
            case cimg::keyQ:
                return;
        }
        main_disp.wait();
    }
}

void ImageDisplay::addMainImage(std::string fileName)
{
    ImageContainer ic(fileName);
    this->vMainImages.push_back(ic);
}

// void ImageDisplay::addReSolvedImage(std::string fileName)
// {
//     for(std::vector<ImageContainer>::iterator it = this->vMainImages.begin();
//         it != this->vMainImages.end();
//         it++)
//     {
//         if( strcmp((*it).getFileName(), mainImage) == 0)
//             (*it).addResolvedImage(fileName);
//     }
// }
void ImageDisplay::addSolverImage(std::string fileName) 
{
    std::string suffix = fileName;
    file_IO::trimLeadingFileName(suffix);
    for(std::vector<ImageContainer>::iterator it = this->vMainImages.begin();
        it != this->vMainImages.end();
        it++)
    {
        std::string commonFileName = (*it).getMain();
        file_IO::trimLeadingFileName(commonFileName);
        if( commonFileName.compare(suffix) == 0)
        {
            (*it).addSolverImage(fileName);
            return;
        }
    }

    std::string sErr = std::string("no match for solver image: ") + fileName;
    throw image_psb::ImageException(sErr);
}

} /* EndOfNameSpace */
