#include "solver.hpp"

#include <vector>

#include "image2.hpp"
#include "file.hpp"
#include "loginstance.hpp"

using namespace logging;

namespace solver
{

image_fmt IterativeSolver::solve()
{
    int iIter = 0;
    double dIterationDiff = 9001;
    image_fmt guess(this->guess);

    std::vector<double> vResults;
    vResults.reserve(2049);
    this->logInst.print< severity_type::info>("Stop criteria set to :", dStopCriterion);


    for(iIter = 0; this->dStopCriterion < dIterationDiff; iIter++)
    {
        this->func(this->field, guess, dIterationDiff);
        double dDiff = this->origImage.MSE(guess);
        vResults.push_back(dDiff);

        if(iIter % 100 == 0)
        {
            this->logInst.print< severity_type::info >("Iteration number :", iIter, " with iteration diff ", dIterationDiff, " and image difference ", dDiff );
            CLOG(severity_type::info)("Solver ", this->getLabel(), " for ", this->getFilename(), ":: iteration number :", iIter, " with iteration diff ", dIterationDiff, " and image difference ", dDiff );
        }
        
        // DO_IF_LOGLEVEL(severity_type::debug)
        // {
        //     std::string sImage = image_psb::printImage(guess);
        //     this->logInst.print< severity_type::debug >("Process:\n", sImage);
        //     CLOG(severity_type::debug)("Process for ", this->getLabel(), " on image ", this->getFilename(), ":\n", sImage);
        // }
    }

    file_IO::writeData(vResults, this->getLabel(), this->getFilename());

    DO_IF_LOGLEVEL(severity_type::debug)
    {
        std::string sImage = image_psb::printImage(guess);
        this->logInst.print< severity_type::debug >("Finished image:\n", sImage );
        CLOG(severity_type::debug)("Final product with ", this->getLabel(), " on image ", this->getFilename(), ":\n", sImage);
    }
    this->logInst.print< severity_type::info>("Finished in ", iIter, " iterations\nFinal MSE: ", vResults.back());

    return guess;
}

void Solver::log(int iLevel, std::string sMsg)
{
    this->logInst.print<severity_type::info>(sMsg);
}

bool Solver::isMultipart() { return this->bMultipart; }
bool Solver::isFinal() { return this->bFinal; }
std::string Solver::getFilename() { return this->sFilename; }
std::string Solver::getLabel() { return this->sLabel; }

image_fmt DirectSolver::solve()
{
    image_fmt ret(this->origImage, "xyz", 0);
    this->func(this->field, ret);
    std::vector<double> vtmp(1, (image_psb::imageDiff(this->origImage, ret)));
    file_IO::writeData(vtmp, this->getLabel(), this->getFilename());
    return ret;
}

void Solver::alterField(double dScalar)
{
    this->field *= dScalar;
}

} /* EndOfNamespace */
