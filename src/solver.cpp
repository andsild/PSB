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
        double dDiff = getDiff(this->origImage.MSE(guess), iNumPixels);
                       
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
    image_fmt ret(this->origImage, "xyz", 0);
    MLOG(severity_type::debug, "Returned image:\n", image_psb::printImageAligned(ret));
    this->func(this->field, ret);
    ret.crop(1,1,0,0, ret.width() - 2, ret.height() - 2 , 0, 0);
    const int iPixels = (this->origImage.width() - 2) * (this->origImage.height() - 2);
    vResults.push_back(getDiff(image_psb::imageDiff(this->origImage.get_crop(1,1,0,0,
                                                    this->origImage.width() - 2,
                                                    this->origImage.height() - 2,
                                                    0, 0), ret), iPixels));
    return ret;
}

/** Modify the field of a solver (multiply by scalar)
*/
void Solver::alterField(data_fmt dScalar)
{
    this->field *= dScalar;
}

} /* EndOfNamespace */
