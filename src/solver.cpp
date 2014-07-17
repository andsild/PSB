#include "solver.hpp"

#include <fstream>
#include <iostream>
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
        this->func(this->field, ret, dIterationDiff);
        vResults.push_back(image_psb::imageDiff(this->origImage, this->guess));
    }

    this->writeData(vResults);
    return ret;
}

void IterativeSolver::postProsess()
{
    // this->writeData();
}

void Solver::writeData(const std::vector<double> &vData)
{
    const char *filename = "output.txt";
    std::ofstream fout(filename, std::ios_base::binary|std::ios_base::out|std::ios_base::app);
    for(auto const item : vData)
        fout << item << " ";
    fout << std::endl;
    fout.close();
}

image_fmt DirectSolver::solve()
{
    image_fmt ret(this->origImage, "xyz", 0);
    this->func(this->field, ret);
    std::vector<double> vtmp(1, (image_psb::imageDiff(this->origImage, ret)));
    this->writeData(vtmp);
    return ret;
}

void DirectSolver::postProsess()
{
}

void Solver::alterField(double dScalar)
{
    this->field *= dScalar;
}

} /* EndOfNamespace */
