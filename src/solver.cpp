#include "solver.hpp"

#include <vector>

#include "image2.hpp"
#include "loginstance.hpp"

using namespace logging;

namespace solver
{


image_fmt IterativeSolver::solve(rawdata_fmt &vResults)
{
    int iIter = 0;
    double dIterationDiff = 9001;
    image_fmt guess(this->guess);
    const int width = this->guess.width();
    const int height = this->guess.height();

    // this->logInst.print< severity_type::info>("Stop criteria set to :", dStopCriterion);

    for(iIter = 0; this->dStopCriterion < dIterationDiff; iIter++)
    {
        this->func(this->field, guess, dIterationDiff, width, height);
        double dDiff = this->origImage.MSE(guess);
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
    // this->logInst.print<severity_type::info>(sMsg);
}

bool Solver::isMultipart() { return this->bMultipart; }
bool Solver::isFinal() { return this->bFinal; }
std::string Solver::getFilename() { return this->sFilename; }
std::string Solver::getLabel() { return this->sLabel; }

image_fmt DirectSolver::solve(rawdata_fmt &vResults)
{
    image_fmt ret(this->origImage, "xyz", 0);
    this->func(this->field, ret);
    vResults.push_back(image_psb::imageDiff(this->origImage, ret));
    return ret;
}

/** Modify the field of a solver (multiply by scalar)
*/
void Solver::alterField(data_fmt dScalar)
{
    this->field *= dScalar;
}

} /* EndOfNamespace */
