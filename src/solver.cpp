#include "solver.hpp"

#include <vector>

#include "CImg.h"

#include "imageUtil.hpp"
#include "loginstance.hpp"
using namespace logging;

namespace solver
{

/** To avoid small numbers, results are mutliplied by a constant
*/
const double MULTIPLIER_CONSTANT = 100.0;

/** After obtaining the difference and another image, 
  modify the value to make more sense of it.
  Here, we divide by pixels and multiply by a constant - this means
  that results are somewhat indepedent of image geometry, and they look nicer.

  @param origVal is the difference in some norm between reconstruction and
  orignal image
  @param dPixels is the amount of pixels in the image
  @return is the new, modified result value
*/
inline double getDiff(const double origVal, const double dPixels)
{
    return ((origVal / dPixels) * MULTIPLIER_CONSTANT);
}

/** Wipe the memory, i.e. image, field (and noisedImage) from a solver
  class (for when after we have used a solver
*/
void Solver::clear()
{
    delete this->origImage;
    delete this->field;
    delete this->noisedImage;
}

/** Default constructor
*/
Solver::Solver(const image_fmt *origImg, const image_fmt* const f,
        std::string sFile, std::string sLab,
        bool bMulPart, bool bFin)
    : origImage(origImg), noisedImage(nullptr), field(f), logInst(),
        sFilename(sFile), sLabel(sLab), bMultipart(bMulPart), bFinal(bFin)
{
}

/** Default constructor
*/
Solver::Solver(const image_fmt* const origImg, const image_fmt* const noisedImg,
        const image_fmt* const f,
        std::string sFile, std::string sLab,
        bool bMulPart, bool bFin)
    : origImage(origImg), noisedImage(noisedImage), field(f), logInst(),
        sFilename(sFile), sLabel(sLab), bMultipart(bMulPart), bFinal(bFin)
{
}
Solver::Solver() : origImage(nullptr), noisedImage(nullptr), field(nullptr)
{}

/** Default constructor
*/
IterativeSolver::IterativeSolver(
        const image_fmt* const origImg, const image_fmt* const f,
        const image_fmt* const U, iterative_func func,
        double dStopCriterion, std::string sFile, std::string sLabel,
        bool bMultiPart, bool bFinal)
    : Solver(origImg, f, sFile, sLabel, bMultiPart, bFinal), func(func),
        dStopCriterion(dStopCriterion), guess(*U)
{
}
/** Default constructor
*/
IterativeSolver::IterativeSolver() : Solver() {}


/** Default constructor
*/
DirectSolver::DirectSolver(
    const image_fmt* const origImg, const image_fmt* const f, direct_func func,
    std::string sFilename, std::string sLabel,
    bool dirichet,
    bool bMultiPart, bool bFin)
    : Solver(origImg, f, sFilename, sLabel, bMultiPart, bFinal),
        func(func), isDirichet(dirichet)
{
}

/** Default constructor
*/
DirectSolver::DirectSolver(
    const image_fmt* const origImg, const image_fmt* const noisedImg,
    const image_fmt* const f, direct_func func,
    std::string sFilename, std::string sLabel,
    bool dirichet,
    bool bMultiPart, bool bFin)
    : Solver(origImg,noisedImg, f, sFilename, sLabel, bMultiPart, bFinal),
        func(func), isDirichet(dirichet)
{
}

/** Default constructor
*/
DirectSolver::DirectSolver() : Solver(), isDirichet(false) {}

/** Execute an iterative solver
  @param vresults is a vector of result numbers, to be returned
  @param vtimes holds the time in ms for each iteration
  @return is the reconstructed image
*/
image_fmt IterativeSolver::solve(rawdata_fmt &vResults, rawdata_fmt &vTimes)
{
    int iIter = 0;
    double dIterationDiff = 9001;
    image_fmt guess(this->guess);
    const int iWidth = this->guess.width();
    const int iHeight = this->guess.height();
    const double iNumPixels = guess.size();
    unsigned long lTime = 0;

    /* While stop criterion is not reached */
    for(iIter = 0; this->dStopCriterion < dIterationDiff; iIter++)
    {
        cimg_library::cimg::tic(); /* Time each iteration */
        this->func(*(this->field), guess, dIterationDiff, iWidth, iHeight);
        lTime = cimg_library::cimg::toc(); /* Stop timer */
        double dDiff = getDiff(this->origImage->MSE(guess), iNumPixels);
                       
        vResults.push_back(dDiff);
        vTimes.push_back((double)lTime);
    }

    return guess;
}

/** Log a message to a file made for the solver instance alone
  @param iLevel is the verbosity level
  @param sMsg is the message to log
*/
void Solver::log(int iLevel, std::string sMsg)
{
    this->logInst.print<severity_type::extensive>(sMsg);
}

/** Say whether or not the image was divided into subregions
*/
bool Solver::isMultipart() { return this->bMultipart; }
/** Say whether this image instance is the final part of a divided image
*/
bool Solver::isFinal() { return this->bFinal; }
/** Get the filename for a solver image
*/
std::string Solver::getFilename() { return this->sFilename; }
/** Get the label for a solver instance
*/
std::string Solver::getLabel() { return this->sLabel; }

/** Reconstruct an image using a direct method
  @param vresults is a vector of result numbers, to be returned
  @param vtimes holds the time in ms for each iteration
  @return is the reconstructed image
*/
image_fmt DirectSolver::solve(rawdata_fmt &vResults, rawdata_fmt &vTimes)
{
    // const int iPixels = (this->origImage->width() - 2) * (this->origImage->height() - 2);
    image_fmt ret(*(this->origImage), "xyz", 0);
    image_fmt useField;
    unsigned long time = 0;
    if(this->isDirichet == false) /* Need to trim away borders afterward */
    {
        MLOG(severity_type::debug, "\n", image_util::printImageAligned(*(field)));

        cimg_library::cimg::tic();
        this->func(*(field), ret);
        time = cimg_library::cimg::toc();

        ret.crop(1,1,0,0,
                field->width() - 2, field->height() - 2, 0, 0);
        ret += (this->origImage->mean() - ret.mean());
    }
    else
    {
        cimg_library::cimg::tic();
        this->func(*(field), ret);
        time = cimg_library::cimg::toc();
    }
    MLOG(severity_type::extensive, "Returned image:\n",
            image_util::printImageAligned(ret));
    vResults.push_back(getDiff(
                image_util::imageDiff(*(this->origImage),ret),
                                    this->origImage->size()));
    vTimes.push_back(time);
    if(this->noisedImage != nullptr)
    {
        vResults.push_back(getDiff(image_util::imageDiff(
                        *(this->noisedImage), ret), this->noisedImage->size()));
    }
    return ret;
}

} /* EndOfNamespace */
