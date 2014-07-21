#include "solver.hpp"

#include <algorithm>
#include <iostream>
#include <vector>
#include <tuple>

#include "image2.hpp"
#include "file.hpp"

namespace solver
{

template<class... Ts>
std::vector<std::tuple<Ts...>> zip(std::vector<Ts> const&... vs){
    auto lo = std::min({vs.size()...});
    std::vector<std::tuple<Ts...>> v;
    v.reserve(lo);
    for(unsigned i = 0; i < lo; ++i)
        v.emplace_back(vs[i]...);
    return v;
}

image_fmt IterativeSolver::solve()
{
    int iIter = 0;
    double dIterationDiff = 9001;
    std::vector< std::vector<double> > vDivisions;
    std::vector< double > vEndRes;
    int iShortest = -1;

    // std::cerr << image_psb::printImage(this->origImage) << std::endl;
    // std::cerr << image_psb::printImage(this->subspaceFields[3]) << std::endl;

    for(int iPos = 0; iPos < this->DIVISION_SIZE; iPos++)
    {
        std::vector<double> vResults;
        vResults.reserve(2049);
        image_fmt ret(this->subspaces[iPos]);
        for(iIter = 0; this->dStopCriterion < dIterationDiff; iIter++)
        {
            this->func(this->subspaceFields[iPos], ret, dIterationDiff);
            vResults.push_back(this->subspaces[iPos].MSE(ret));
        }
        if(vResults.empty())
            continue;
        vDivisions.push_back(vResults);
        if(iShortest > vResults.size())
            iShortest = iPos;
    }

    for(int iPos = 0; iPos < vDivisions.at(iShortest).size(); iPos++)
    {
        double dVal = 0;
        for(auto it : vDivisions)
        {
            dVal += it.at(iPos);
        }
        vEndRes.push_back(dVal / vDivisions.size());
    }

    // for(auto&& it : zip(vDivisions[0], vDivisions[1]))
    // for(auto&& it : zip(vDivisions...vecs))
    // {
    //     std::cout << std::get<0>(it) << " " << std::get<1>(it) << " " << std::endl;
    // }
    file_IO::writeData(vEndRes, this->getLabel(), this->getFilename());

    // TODO: join picture


    // file_IO::writeData(vResults, this->getLabel, this->getFilename);
    return ret;
}

std::string Solver::getFilename() { return this->sFilename; }
std::string Solver::getLabel() { return this->sLabel; }

void IterativeSolver::postProsess()
{
    // this->writeData();
}
void IterativeSolver::divideImage()
{
    // Division is square: for 4 regions we split X in 2 (hence the / 2)
    const int WIDHT_REGION = (this->field.width() / (this->DIVISION_SIZE / 2)),
              HEIGHT_REGION = (this->field.height() / (this->DIVISION_SIZE / 2));

    for(int xSlice = 0; xSlice < this->DIVISION_SIZE / 2; xSlice++)
    {
        for(int ySlice = 0; ySlice < this->DIVISION_SIZE / 2; ySlice++)
        {
            int iLeftmostX = xSlice * WIDHT_REGION,
                iUpperY = ySlice * HEIGHT_REGION;
            int iRightmostX = iLeftmostX + WIDHT_REGION -1,
                iLowerY = iUpperY + HEIGHT_REGION - 1;

            if(WIDHT_REGION % 2 == 0 && xSlice == 0)
                iRightmostX++;
            if(HEIGHT_REGION % 2 == 0 && ySlice == 0)
                iLowerY++;

            if(iRightmostX > this->field.width())
                iRightmostX = this->field.width() - 1;
            if(iLowerY > this->field.height())
                iLowerY = this->field.height() - 1;


            this->subspaceFields.push_back(this->field.get_crop(iLeftmostX, iUpperY, 0,
                                                                iRightmostX,
                                                                iLowerY, 0));

            this->subspaces.push_back(this->origImage.get_crop(iLeftmostX, iUpperY, 0,
                                                                iRightmostX,
                                                                iLowerY, 0));
        }
    }
}


image_fmt DirectSolver::solve()
{
    image_fmt ret(this->origImage, "xyz", 0);
    this->func(this->field, ret);
    std::vector<double> vtmp(1, (image_psb::imageDiff(this->origImage, ret)));
    file_IO::writeData(vtmp, this->getLabel(), this->getFilename());
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
