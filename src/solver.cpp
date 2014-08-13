#include "solver.hpp"

#include <vector>

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


Solver::Solver(const image_fmt *origImg, const image_fmt* const f,
        std::string sFile, std::string sLab,
        bool bMulPart, bool bFin)
    : origImage(origImg), field(*f), logInst(),
        sFilename(sFile), sLabel(sLab), bMultipart(bMulPart), bFinal(bFin)
{
    // this->logInst.setName(
    //         file_IO::SAVE_PATTERN.getLogname(this->sFilename, this->sLabel, false));
    // this->logInst.setLevel(GETLEVEL);

    // DO_IF_LOGLEVEL(severity_type::extensive)
    // {
    //     this->logInst.print< severity_type::extensive>("Initial image:\n", image_psb::printImage(this->origImage));
    //     this->logInst.print< severity_type::extensive>("Initial field:\n", image_psb::printImage(this->field));
    // }
}

IterativeSolver::IterativeSolver(
        const image_fmt* const origImg, const image_fmt* const f,
        const image_fmt* const U, iterative_func func,
        double dStopCriterion, std::string sFile, std::string sLabel,
        bool bMultiPart, bool bFinal)
    : Solver(origImg, f, sFile, sLabel, bMultiPart, bFinal), func(func),
        dStopCriterion(dStopCriterion), guess(*U)
{
    // DO_IF_LOGLEVEL(severity_type::extensive)
    //     this->logInst.print< severity_type::extensive>("Initial guess:\n", image_psb::printImage(this->guess));
}


DirectSolver::DirectSolver(
    const image_fmt* const origImg, const image_fmt* const f, direct_func func,
    std::string sFilename, std::string sLabel,
    bool dirichet,
    bool bMultiPart, bool bFin)
    : Solver(origImg, f, sFilename, sLabel, bMultiPart, bFinal),
        func(func), isDirichet(dirichet)
{
}

image_fmt IterativeSolver::solve(rawdata_fmt &vResults)
{
    int iIter = 0;
    double dIterationDiff = 9001;
    image_fmt guess(this->guess);
    const int iWidth = this->guess.width();
    const int iHeight = this->guess.height();
    const double iNumPixels = guess.size();

    // this->logInst.print< severity_type::info>("Stop criteria set to :", dStopCriterion);

    for(iIter = 0; this->dStopCriterion < dIterationDiff; iIter++)
    {
        this->func(this->field, guess, dIterationDiff, iWidth, iHeight);
        double dDiff = getDiff(this->origImage->MSE(guess), iNumPixels);
                       
        vResults.push_back(dDiff);

        // if(iIter % 100 == 0)
        // {
        //     this->logInst.print< severity_type::info >("Iteration number :", iIter, " with iteration diff ", dIterationDiff, " and image difference ", dDiff );
        //     CLOG(severity_type::info)("Solver ", this->getLabel(), " for ", this->getFilename(), ":: iteration number :", iIter, " with iteration diff ", dIterationDiff, " and image difference ", dDiff );
        // }
        
        // DO_IF_LOGLEVEL(severity_type::debug)
        // {
        //     std::string sImage = image_psb::printImage(guess);
        //     this->logInst.print< severity_type::debug >("Process:\n", sImage);
        //     CLOG(severity_type::debug)("Process for ", this->getLabel(), " on image ", this->getFilename(), ":\n", sImage);
        // }
    }

    // DO_IF_LOGLEVEL(severity_type::debug)
    // {
    //     std::string sImage = image_psb::printImage(guess);
    //     this->logInst.print< severity_type::debug >("Finished image:\n", sImage );
    //     CLOG(severity_type::debug)("Final product with ", this->getLabel(), " on image ", this->getFilename(), ":\n", sImage);
    // }
    // this->logInst.print< severity_type::info>("Finished in ", iIter, " iterations\nFinal MSE: ", vResults.back());

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

image_fmt DirectSolver::solve(rawdata_fmt &vResults)
{
    image_fmt ret(*(this->origImage), "xyz", 0);
    this->func(this->field, ret);
    if(this->isDirichet == false)
        ret += (this->origImage->mean() - ret.mean());
    MLOG(severity_type::extensive, "Returned image:\n", image_psb::printImageAligned(ret));
    ret.crop(1,1,0,0, ret.width() - 2, ret.height() - 2 , 0, 0);
    const int iPixels = (this->origImage->width() - 2) * (this->origImage->height() - 2);
    vResults.push_back(getDiff(image_psb::imageDiff(
            this->origImage->get_crop(1,1,0,0, this->origImage->width() - 2,
                                              this->origImage->height() - 2, 0,0)
            ,ret), iPixels));
    return ret;
}

} /* EndOfNamespace */
