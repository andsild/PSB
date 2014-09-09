/** Render the original and solved images for the user.
*/
// To refactor this code, I'd recommend starting from scratch
// I started out with something small in mind, and the code does not at all scale.
#include "imageedit.hpp"

#include <iostream>
#include <string>
#include <vector>

#include "CImg.h"

#include "file.hpp"
#include "image_types.hpp"
#include "loginstance.hpp"
#include "image2.hpp"

using namespace cimg_library;
using namespace logging;

namespace image_display
{



class ImageContainer
{
    private:
    std::vector<std::string> vSolvedImages,
                             vResolvedImages,
                             vNoisedImages;
                             
    std::string fileName, noisedFileName;

    circularIterator<std::vector<std::string>::const_iterator > itSolved,
                                                                itResolved;
    public:
    static int iSolvedIndex, iResolvedIndex, iNoisedIndex, isNoisedOrigImages;
    ImageContainer(std::string);
    
    void addSolverImage(std::string);
    void addResolvedImage(std::string);
    void addNoisedImage(std::string);
    void addNoisedImageOrig(std::string);

    bool hasResolvedImages();

    std::string getFileName() const;
    std::string getMain() const;
    std::string getSolved() const;
    std::string getResolved() const;
    std::string getNoised() const;
    std::string getOrigNoised() const;
    std::string getNextSolver();
    std::string getPrevSolver();
    std::string getNextResolver();
    std::string getPrevResolver();
    bool hasResolved() const;
    bool hasNoised() const;
    void initializeIterators();
};

int ImageContainer::iSolvedIndex       = 0;
int ImageContainer::iResolvedIndex     = 0;
int ImageContainer::iNoisedIndex       = 0;
int ImageContainer::isNoisedOrigImages = 0;

ImageContainer::ImageContainer(std::string fileName)
               : fileName(fileName)
{
    this->iSolvedIndex       = 0;
    this->iResolvedIndex     = 0;
    this->iNoisedIndex       = 0;
    this->isNoisedOrigImages = 0;
}

std::string ImageContainer::getFileName() const
{
    std::string sCopy = this->fileName;
    file_IO::trimLeadingFileName(sCopy);
    return sCopy;
}

void ImageContainer::addSolverImage(std::string fileName)
{
    this->vSolvedImages.push_back(fileName);
}

void ImageContainer::addResolvedImage(std::string fileName)
{
    this->vResolvedImages.push_back(fileName);
}

void ImageContainer::addNoisedImage(std::string fileName)
{
    this->vNoisedImages.push_back(fileName);
}

void ImageContainer::addNoisedImageOrig(std::string fileName)
{
    this->noisedFileName = fileName;
}

bool ImageContainer::hasResolved() const
{
    return (this->vResolvedImages.size() > 0);
}
bool ImageContainer::hasNoised() const
{
    return (this->vNoisedImages.size() > 0);
}

std::string ImageContainer::getMain() const
{
    if(this->fileName.length() < 1)
    {
        throw ImageException(std::string("no filename for image container!"));
    }
    return this->fileName;
}

std::string ImageContainer::getSolved() const
{
    if(this->vSolvedImages.empty())
    {
        std::string sErr= "lookup for solved image to " + this->fileName + " failed: no (non-re)solved images added, but access was attempted ";
        throw ImageException(sErr);
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
        throw ImageException(sErr);
    }
    return this->vSolvedImages.at(iSolvedIndex);
}
std::string ImageContainer::getResolved() const
{
    if(this->vResolvedImages.empty())
    {
        std::string sErr= "lookup for solved image to " + this->fileName + " failed: no resolved images added, but access was attempted ";
        throw ImageException(sErr);
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
        throw ImageException(sErr);
    }
    return this->vResolvedImages.at(iResolvedIndex);
}
std::string ImageContainer::getNoised() const
{
    return this->vNoisedImages.at(0);
}
std::string ImageContainer::getOrigNoised() const
{
    return this->noisedFileName;
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
    graph_disp.set_title("Color intensities: orig image=white, solved=red");
}

bool ImageContainer::hasResolvedImages()
{
    return this->vResolvedImages.empty() == false;
}

void ImageDisplay::loadImmy(std::string &sMainfile, std::string &sSolverfile,
                            std::string &sResolvedfile, std::string &sNoisedfile,
                            std::string &sOrigNoisedfile)
{
    ImageContainer inst = this->vMainImages.at(this->iIndex);
    try
    {
        sMainfile = inst.getMain();
        sSolverfile = inst.getSolved();
        if(inst.hasResolvedImages())
            sResolvedfile = inst.getResolved();
        if(inst.hasNoised())
        {
            sNoisedfile = inst.getNoised();
            sOrigNoisedfile = inst.getOrigNoised();
        }
    }
    catch(ImageException ie)
    {
        // std::cerr << ie.what() << std::endl;
        this->vMainImages.erase(this->vMainImages.begin() + this->iIndex);
        if(iIndex != 0)
            this->iIndex--;
        if(this->vMainImages.size() <= 0) {
            std::cerr << "Lookup for solved images exhausted, exiting" << std::endl;
            exit(EXIT_FAILURE);
        }
        this->loadImmy(sMainfile, sSolverfile, sResolvedfile, sNoisedfile, sOrigNoisedfile);
    }
}

void ImageDisplay::show()
{
    if(this->vMainImages.empty())
        return;
    std::string sMainfile, sSolverfile, sResolvedfile, sNoisedfile, sOrigNoisedfile;
    loadImmy(sMainfile, sSolverfile, sResolvedfile, sNoisedfile, sOrigNoisedfile);

    cimg::exception_mode(0);
    try
    {
        this->main_image.assign(sMainfile.c_str());
        image_util::toGrayScale(this->main_image);
        this->solved_image.load(sSolverfile.c_str());
    }
    catch(CImgIOException ciie)
    {
        std::cerr << ciie.what() << std::endl;
        return;
    }

    this->main_disp = this->main_image;
    this->main_disp.set_title(sMainfile.c_str());
    this->solved_disp = this->solved_image;
    this->solved_disp.set_title(sSolverfile.c_str());

    if(this->vMainImages.at(this->iIndex).hasResolvedImages())
    {
        this->resolved_image.load_ascii(sResolvedfile.c_str());
        this->resolved_disp = this->resolved_image;
        this->resolved_disp.set_title(sResolvedfile.c_str());
    }
    if(this->vMainImages.at(this->iIndex).hasNoised())
    {
        this->noised_image.assign(sNoisedfile.c_str());
        this->noised_disp = this->noised_image;
        this->noised_disp.set_title(sNoisedfile.c_str());

        this->orig_noised_image.assign(sOrigNoisedfile.c_str());
        this->orignoised_disp = this->orig_noised_image;
        this->orignoised_disp.set_title(sOrigNoisedfile.c_str());

        const char *graphTitle = this->graph_disp.title();
        std::string sNewTitle = graphTitle + std::string(", orignoise = yellow, solvednoise = green");
        this->graph_disp.set_title(sNewTitle.c_str());
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
    // this->solved_image.load_ascii(sNextFile.c_str());
    this->solved_image.load(sNextFile.c_str());
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
    this->solved_image.load(sPrevFile.c_str());
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
                 green[] = {0, 150, 0},
                 blue[] = {0, 0, 255},
                 yellow[] = {200,200,0};
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
                visu.draw_graph(resolved_cropped, green, 1, 1, yMin, yMax, 0);
            }
            if(this->noised_disp.is_empty() == false)
            {
                image_fmt noised_crop = this->noised_image.get_crop(
                    0, yPos, 0, 0, this->noised_image.width()-1, yPos, 0, 0);
                image_fmt orig_noised_crop = this->orig_noised_image.get_crop(
                    0, yPos, 0, 0, this->noised_image.width()-1, yPos, 0, 0);

                visu.draw_graph(noised_crop,      green,  1, 1, yMin, yMax, 0);
                visu.draw_graph(orig_noised_crop, yellow, 1, 1, yMin, yMax, 0);
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
    main_disp.close();
    solved_disp.close();
    graph_disp.close();
}

void ImageDisplay::addMainImage(std::string fileName)
{
    ImageContainer ic(fileName);
    this->vMainImages.push_back(ic);
}

void ImageDisplay::addResolvedImage2(std::string sFilename, std::string sCommon,
                                     bool isResolved, bool isNoised, bool isNoisedOrig)
{
    for(auto &it : this->vMainImages)
    {
        std::string sMainname = file_IO::getFilename(it.getMain());
        if(sMainname.compare(sCommon) == 0)
        {
            if(isResolved)
                it.addResolvedImage(sFilename);
            else if(isNoised)
            {
                it.addNoisedImage(sFilename);
            }
            else if(isNoisedOrig)
            {
                it.addNoisedImageOrig(sFilename);
            }
            else
                it.addSolverImage(sFilename);
            return;
        }
    }
    std::string sErr = std::string("no match for solver image: ") + sFilename;
    throw ImageException(sErr);
}

void ImageContainer::initializeIterators()
{
    itSolved.setIterator(this->vSolvedImages.begin(),
                                this->vSolvedImages.end(),
                                this->vSolvedImages.begin());
    itResolved.setIterator(this->vResolvedImages.begin(),
                                this->vResolvedImages.end(),
                                this->vResolvedImages.begin());
   // MLOG(severity_type::debug, itSolved()); 
}


void scanAndAddImage(std::string sRootdir, std::string sSolverdir)
{
    std::vector<std::string> vFilenames,vSolvedNames;
    try
    {
        vFilenames = file_IO::getFilesInFolder(sRootdir),
        vSolvedNames = file_IO::getFilesInFolder(sSolverdir);
    }
    catch(file_IO::DirNotFound) {
        std::cerr << "No directory " << sRootdir
                  << " found. Did you remember to run a solver first?"
                  << std::endl;
        return;
    }
    ImageDisplay id;

    for(auto const it : vFilenames)
    {
        id.addMainImage(it);
    }
    for(auto const it : vSolvedNames)
    {
        bool isResolved   =  false,
             isNoised     =  false,
             isNoisedOrig =  false;
        std::string _, sLabel, sFilename;

        file_IO::SAVE_PATTERN.getNames(it, _, sLabel, sFilename, isResolved,
                                        isNoised, isNoisedOrig);
        try
        {
            id.addResolvedImage2(it, sFilename, isResolved, isNoised, isNoisedOrig);
        }
        catch(ImageException ie)
        {
            std::cerr << ie.what() << "\n continuing..." << std::endl;
        }
    }
    for(auto it : id.vMainImages)
    {
        it.initializeIterators();
    }
    id.show();
    id.loop();
}


} /* EndOfNameSpace */
