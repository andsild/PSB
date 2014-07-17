#include "solver.hpp"

#include <vector>

#include "image2.hpp"

namespace solver
{

image_fmt IterativeSolver::solve()
{
    int iIter = 0;
    double dIterationDiff = 9001;
    image_fmt ret(this->guess);
    std::vector<double> vResults;
    vResults.reserve(2049);
    

    for(iIter; this->dStopCriterion < dIterationDiff; iIter++)
    {
        this->func(this->field, this->guess, dIterationDiff);
        vResults.push_back(image_psb::imageDiff(this->origImage, this->guess));
    }

    return ret;
}

void IterativeSolver::postProsess()
{
    this->writeData();
}

void IterativeSolver::writeData()
{
}

image_fmt DirectSolver::solve()
{
    image_fmt ret(this->origImage, "xyz", 0);
    this->func(this->field, ret);
    return ret;
}

void DirectSolver::postProsess()
{
    this->writeData();
}

void DirectSolver::writeData()
{
}

void Solver::alterField(double dScalar)
{
    this->field *= dScalar;
}

} /* EndOfNamespace */
