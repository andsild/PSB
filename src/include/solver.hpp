#ifndef SOLVER_H
#define  SOLVER_H

#define BORDER_SIZE 1

#include <string>

#include "image_types.hpp"
#include <loginstance.hpp>

namespace solver
{

class Solver
{
    private:
    std::string sFilename;

    protected:
    virtual void postProsess() = 0;
    virtual void solve() = 0;
    virtual void writeData() = 0;

    image_fmt origImage;
    image_fmt field;
    logging::Logger< logging::FileLogPolicy > logInst;

    public:
    Solver(image_fmt origImage, image_fmt image, std::string sFileName)
        : field(image), sFilename(sFilename), logInst(), origImage(origImage)
    {
        // this->field = 
        this->logInst.setName(std::string("poop"));
    }
};

class IterativeSolver : public virtual Solver
{
    private:
    iterative_func func;
    image_fmt guess;
    double dStopCriterion;
    void betweenIter();
    void solve();
    void postProsess();
    void writeData();

    public:
    IterativeSolver(image_fmt origImage, image_fmt field, image_fmt guess, iterative_func func,
            double dStopCriterion, std::string sFilename = "")
        : Solver(origImage, field, sFilename) , func(func), dStopCriterion(dStopCriterion) {}
};

// class DirectSolver : Solver
// {
//     DirectSolver(image_fmt image, std::string sFilename)
//         : Solver(image, sFilename) {}
// };

} /* EndOfNamespace */

#endif
