#ifndef SOLVER_H
#define  SOLVER_H

#define BORDER_SIZE 1

#include <iostream>
#include <vector>
#include <string>

#include "image_types.hpp"
#include "loginstance.hpp"

namespace solver
{

class Solver
{
    private:
    std::string sFilename,
                sLabel;

    protected:
    virtual void postProsess() = 0;

    const image_fmt &origImage;
    image_fmt &field;
    logging::Logger< logging::FileLogPolicy > logInst;

    public:
    virtual image_fmt solve() = 0;
    void alterField(double);
    std::string getLabel();
    std::string getFilename();
    Solver(const image_fmt &origImage, image_fmt &field, std::string sFile, std::string sLab)
        : field(field), origImage(origImage), logInst(),
            sFilename(sFile), sLabel(sLab)
    {
        this->logInst.setName(std::string("poop"));
        std::cout << this->sFilename << std::endl;
        std::cout << this->sLabel << std::endl;
    }
};

class IterativeSolver : public virtual Solver
{
    private:
    iterative_func func;
    const image_fmt &guess;
    double dStopCriterion;
    void postProsess();
    void divideImage();
    image_fmt joinImage(imageList_fmt);
    imageList_fmt subspaces, subspaceFields;
    const int DIVISION_SIZE;

    public:
    image_fmt solve();
    IterativeSolver(const image_fmt &origImage, image_fmt &field,
            const image_fmt &guess, iterative_func func,
            double dStopCriterion, const int iDivSize, std::string sFile, std::string sLabel)
        : Solver(origImage, field, sFile, sLabel), func(func),
            dStopCriterion(dStopCriterion), guess(guess), DIVISION_SIZE(iDivSize)
    {
        divideImage();
    }
};

class DirectSolver : public virtual Solver
{
    private:
    direct_func func;
    void postProsess();
    public:
    image_fmt solve();
    DirectSolver(const image_fmt &origImage, image_fmt &field, direct_func func,
            std::string sFilename, std::string sLabel)
        : Solver(origImage, field, sFilename, sLabel), func(func)
    {
    }
};

} /* EndOfNamespace */


#endif
