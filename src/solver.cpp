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
    for(iIter = 0; this->dStopCriterion < dIterationDiff; iIter++)
    {
        this->func(this->field, guess, dIterationDiff);
        MLOG(severity_type::debug, "Process: ", image_psb::printImage(guess));
        vResults.push_back(this->origImage.MSE(guess));
    }

    file_IO::writeData(vResults, this->getLabel(), this->getFilename());
    return guess;
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
