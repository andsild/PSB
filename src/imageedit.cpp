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
std::string ImageContainer::getSolved() const
{
    if(this->vSolvedImages.empty())
    {
        std::string sErr= "lookup for solved image to " + this->fileName + " failed: no resolved images added, but access was attempted ";
        throw image_psb::ImageException(sErr);
    }
    if(this->iSolvedIndex < 0) iSolvedIndex++;
    if(this->iSolvedIndex >= this->vSolvedImages.size()) iSolvedIndex--;
    if(this->iSolvedIndex >= this->vSolvedImages.size()
    || this->iSolvedIndex < 0)
    {
        std::string sErr= "lookup for solved image to " + this->fileName +
                          " failed. (index was out of bounds: "
                          + std::to_string(this->iSolvedIndex)
                          + " attempted on list of size "
                          + std::to_string(this->vSolvedImages.size());
        throw image_psb::ImageException(sErr);
    }
    return this->vSolvedImages.at(iSolvedIndex);
}
std::string ImageContainer::getResolved() const
{
    if(this->vResolvedImages.empty())
    {
        std::string sErr= "lookup for solved image to " + this->fileName + " failed: no resolved images added, but access was attempted ";
        throw image_psb::ImageException(sErr);
    }
    if(this->iResolvedIndex < 0) iResolvedIndex++;
    if(this->iResolvedIndex >= this->vResolvedImages.size()) iResolvedIndex--;

    if(this->iResolvedIndex >= this->vResolvedImages.size()
    || this->iResolvedIndex < 0)
    {
        std::string sErr= "lookup for solved image to " + this->fileName +
                          " failed. (index was out of bounds: "
                          + std::to_string(this->iResolvedIndex)
                          + " attempted on list of size "
                          + std::to_string(this->vResolvedImages.size());
        throw image_psb::ImageException(sErr);
    }
    return this->vResolvedImages.at(iResolvedIndex);
}
std::string ImageContainer::getNextSolver()
{
    if(this->iSolvedIndex >= this->vSolvedImages.size())
        this->iSolvedIndex = 0;
    if(this->iSolvedIndex < 0)
        this->iSolvedIndex++;
    std::string sElem = this->vSolvedImages.at(iSolvedIndex);
    this->iSolvedIndex++;
    return sElem;
}


std::string ImageContainer::getPrevSolver()
{
    if(this->iSolvedIndex < 0)
        this->iSolvedIndex = this->vSolvedImages.size() - 1;
    if(this->iSolvedIndex >= this->vSolvedImages.size())
        this->iSolvedIndex--;
    std::string sElem = this->vSolvedImages.at(iSolvedIndex);
    this->iSolvedIndex--;
    return sElem;
}

std::string ImageContainer::getNextResolver() 
{
    if(this->iResolvedIndex >= this->vResolvedImages.size())
        iResolvedIndex = 0;
    if(this->iResolvedIndex < 0)
        iResolvedIndex++;
    std::string sElem = this->vResolvedImages.at(iResolvedIndex);
    this->iResolvedIndex++;
    return sElem;
}

std::string ImageContainer::getPrevResolver()
{
    if(this->iResolvedIndex < 0)
        this->iResolvedIndex = this->vResolvedImages.size() - 1;
    if(this->iResolvedIndex >= vResolvedImages.size()) iResolvedIndex--;
    std::string sElem = this->vResolvedImages.at(iResolvedIndex);
    this->iResolvedIndex--;
    return sElem;
}

ImageDisplay::ImageDisplay()
{
    this->iIndex = 0;
    visu.assign(500, 256, 1, 3, 0);
    graph_disp = visu;
    graph_disp.set_title("Color intensities");
}

bool ImageContainer::hasResolvedImages()
{
    return this->vResolvedImages.empty() == false;
}

void ImageDisplay::show()
{
    ImageContainer inst = this->vMainImages.at(this->iIndex);
    std::string mainFile, solver, resolved;
    try
    {
        mainFile = inst.getMain();
        solver = inst.getSolved();
        if(inst.hasResolvedImages())
            resolved = inst.getResolved();
    }
    catch(image_psb::ImageException ie)
    {
        LOG(severity_type::error)(ie.what());
        CLOG(severity_type::error)(ie.what());
        exit(EXIT_FAILURE);
    }

    cimg::exception_mode(0);
    try
    {
        this->main_image.assign(mainFile.c_str());
        toGrayScale(this->main_image);
        // this->main_image.load_ascii(mainFile.c_str());
        // this->solved_image.assign(solver.c_str());
        this->solved_image.load_ascii(solver.c_str());
    }
    catch(CImgIOException ciie)
    {
        LOG(severity_type::error)(ciie.what());
        CLOG(severity_type::error)(ciie.what());
    }


    this->main_disp = this->main_image;
    this->main_disp.set_title(mainFile.c_str());
    this->solved_disp = this->solved_image;
    this->solved_disp.set_title(solver.c_str());

    if(inst.hasResolvedImages())
    {
        // this->resolved_image.assign(resolved.c_str());
        this->resolved_image.load_ascii(resolved.c_str());
        this->resolved_disp = this->resolved_image;
        this->resolved_disp.set_title(resolved.c_str());
    }

}

ImageContainer ImageDisplay::getCurrent()
{
    return this->vMainImages.at(this->iIndex);
}

void ImageDisplay::nextImage()
{
    this->iIndex++;
    if(this->iIndex >= this->vMainImages.size())
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
    this->solved_image.load_ascii(sNextFile.c_str());
    this->solved_disp = this->solved_image;
    this->solved_disp.set_title(sNextFile.c_str());

    if(ic.hasResolvedImages())
    {
        std::string sNextFiler = ic.getNextResolver();
        this->resolved_image.load_ascii(sNextFiler.c_str());
        this->resolved_disp = this->resolved_image;
        this->resolved_disp.set_title(sNextFiler.c_str());
    }
}

void ImageDisplay::prevSolver()
{
    ImageContainer ic = this->getCurrent();

    std::string sPrevFile = ic.getPrevSolver();
    this->solved_image.load_ascii(sPrevFile.c_str());
    this->solved_disp = this->solved_image;
    this->solved_disp.set_title(sPrevFile.c_str());


    if(ic.hasResolvedImages())
    {
        std::string sPrevFiler = ic.getPrevResolver();
        this->resolved_image.load_ascii(sPrevFiler.c_str());
        this->resolved_disp = this->resolved_image;
        this->resolved_disp.set_title(sPrevFiler.c_str());
    }
}

void ImageDisplay::loop()
{
    const double blackWhite[] = {255, 255, 255},
                 red[] = {255, 0, 0},
                 green[] = {0, 255, 0};
    int yMin = 0, yMax = 0;
    if(this->resolved_disp.is_empty() == false)
    {
        yMax = this->resolved_image.max();
        this->visu.resize(500, yMax + 20, 1, 3);
    }

    // if(this->resolved_disp.is_empty() == false)
    // {
    //     yMin = this->resolved_image.min();
    //     yMax = this->resolved_image.max() + 100;
    // }

    while (!this->main_disp.is_closed() && 
           !this->solved_disp.is_closed() && 
           !this->graph_disp.is_closed())
    {
        ImageContainer cur = this->getCurrent();
        if (main_disp.button() && main_disp.mouse_y()>=0) 
        {
            const int yPos = this->main_disp.mouse_y();
            visu.fill(0);
            if(this->resolved_disp.is_empty() == false)
            {
                image_fmt resolved_cropped = this->resolved_image.get_crop(
                    0, yPos, 0, 0, resolved_image.width()-1, yPos, 0, 0);
                // yMax = resolved_cropped.max();
                visu.draw_graph(resolved_cropped, green, 1, 1, yMin, yMax, 0);
            }
            image_fmt main_cropped =  this->main_image.get_crop(
                    0, yPos, 0, 0, this->main_image.width()-1, yPos, 0, 0);
            image_fmt side_cropped = this->solved_image.get_crop(
                    0, yPos, 0, 0, this->solved_image.width()-1, yPos, 0, 0);

            // data, color, opacity, plot_type, verttex_type, ymin
            visu.draw_graph(main_cropped, blackWhite, 1, 1, yMin, yMax, 0);
            visu.draw_graph(side_cropped, red, 1, 1, yMin, yMax, 0);
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
            case cimg::keyARROWLEFT:
                this->nextSolver();
                break;
            case cimg::keyARROWRIGHT:
                this->prevSolver();
                break;
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

void ImageDisplay::addResolvedImage(std::string fileName)
{
    std::string suffix = fileName;
    file_IO::trimLeadingFileName(suffix);
    for(std::vector<ImageContainer>::iterator it = this->vMainImages.begin();
        it != this->vMainImages.end();
        it++)
    {
        std::string commonFileName = (*it).getMain();
        file_IO::trimLeadingFileName(commonFileName);
        std::cout << "Trying to find " << fileName << " matching " << commonFileName << std::endl;
        if( commonFileName.compare(suffix) == 0)
        {
            (*it).addResolvedImage(fileName);
            return;
        }
    }

    std::string sErr = std::string("no match for solver image: ") + fileName;
    throw image_psb::ImageException(sErr);
}

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
