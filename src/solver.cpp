#include "solver.hpp"

#include "image2.hpp"

namespace solver
{

void IterativeSolver::solve()
{
    int iIter = 0;
    double dIterationDiff;

    for(iIter; this->dStopCriterion < dIterationDiff; iIter++)
    {
        this->func(this->field, this->guess, dIterationDiff);
        double dMSE = image_psb::imageDiff(this->origImage, this->guess);
    }
}

void IterativeSolver::postProsess()
{
    this->writeData();
}

void IterativeSolver::writeData()
{
}

} /* EndOfNamespace */
