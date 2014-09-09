#include "solver.hpp"

#include <vector>

#include "CImg.h"

#include "image2.hpp"
#include "loginstance.hpp"
using namespace logging;

namespace solver
{

const double MULTIPLIER_CONSTANT = 100.0;

inline double getDiff(const double origVal, const double dPixels)
{
    return ((origVal / dPixels) * MULTIPLIER_CONSTANT);
}

void Solver::clear()
{
    delete this->origImage;
    delete this->field;
    delete this->noisedImage;
}

Solver::Solver(const image_fmt *origImg, const image_fmt* const f,
        std::string sFile, std::string sLab,
        bool bMulPart, bool bFin)
    : origImage(origImg), noisedImage(nullptr), field(f), logInst(),
        sFilename(sFile), sLabel(sLab), bMultipart(bMulPart), bFinal(bFin)
{
}

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

IterativeSolver::IterativeSolver(
        const image_fmt* const origImg, const image_fmt* const f,
        const image_fmt* const U, iterative_func func,
        double dStopCriterion, std::string sFile, std::string sLabel,
        bool bMultiPart, bool bFinal)
    : Solver(origImg, f, sFile, sLabel, bMultiPart, bFinal), func(func),
        dStopCriterion(dStopCriterion), guess(*U)
{
}
IterativeSolver::IterativeSolver() : Solver() {}


DirectSolver::DirectSolver(
    const image_fmt* const origImg, const image_fmt* const f, direct_func func,
    std::string sFilename, std::string sLabel,
    bool dirichet,
    bool bMultiPart, bool bFin)
    : Solver(origImg, f, sFilename, sLabel, bMultiPart, bFinal),
        func(func), isDirichet(dirichet)
{
}

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

DirectSolver::DirectSolver() : Solver(), isDirichet(false) {}

image_fmt IterativeSolver::solve(rawdata_fmt &vResults, rawdata_fmt &vTimes)
{
    int iIter = 0;
    double dIterationDiff = 9001;
    image_fmt guess(this->guess);
    const int iWidth = this->guess.width();
    const int iHeight = this->guess.height();
    const double iNumPixels = guess.size();
    unsigned long lTime = 0;

    for(iIter = 0; this->dStopCriterion < dIterationDiff; iIter++)
    {
        cimg_library::cimg::tic();
        this->func(*(this->field), guess, dIterationDiff, iWidth, iHeight);
        lTime = cimg_library::cimg::toc();
        double dDiff = getDiff(this->origImage->MSE(guess), iNumPixels);
                       
        vResults.push_back(dDiff);
        vTimes.push_back((double)lTime);
    }

    return guess;
}

void Solver::log(int iLevel, std::string sMsg)
{
    this->logInst.print<severity_type::extensive>(sMsg);
}

bool Solver::isMultipart() { return this->bMultipart; }
bool Solver::isFinal() { return this->bFinal; }
std::string Solver::getFilename() { return this->sFilename; }
std::string Solver::getLabel() { return this->sLabel; }

image_fmt DirectSolver::solve(rawdata_fmt &vResults, rawdata_fmt &vTimes)
{
    // const int iPixels = (this->origImage->width() - 2) * (this->origImage->height() - 2);
    image_fmt ret(*(this->origImage), "xyz", 0);
    image_fmt useField;
    unsigned long time = 0;
    if(this->isDirichet == false)
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
