#include "solver.hpp"

namespace solver
{

void IterativeSolver::solve()
{
    int iIter = 0;
    double dIterationDiff;
    image_fmt tmpGuess;

    for(iIter; this->dStopCriterion < dIterationDiff; iIter++)
    {
        this->func(this->field, this->guess, dIterationDiff);
        // this->func(this->field, this->guess
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
