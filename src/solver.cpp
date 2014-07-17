#include "solver.hpp"

#include "image2.hpp"

namespace solver
{

image_fmt IterativeSolver::solve()
{
    int iIter = 0;
    double dIterationDiff = 9001;
    image_fmt ret(this->guess);

    for(iIter; this->dStopCriterion < dIterationDiff; iIter++)
    {
        this->func(this->field, this->guess, dIterationDiff);
        double dMSE = image_psb::imageDiff(this->origImage, this->guess);
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

void Solver::alterField(double dScalar)
{
    this->field *= dScalar;
}

} /* EndOfNamespace */
