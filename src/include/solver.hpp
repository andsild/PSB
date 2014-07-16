#ifndef SOLVER_H
#define  SOLVER_H

#define BORDER_SIZE 1

#include <string>

#include "image_types.hpp"
#include <loginstance.hpp>

namespace solver
{

typedef void (*iterative_function)(const image_fmt &arg1,
        image_fmt &arg2, double, int, double &arg3, double) ;

class Solver
{
    private:
    std::string sFilename;

    protected:
    virtual void postProsess() = 0;
    virtual void solve() = 0;
    virtual void writeData() = 0;

    image_fmt field;
    logging::Logger< logging::FileLogPolicy > logInst;

    public:
    Solver(image_fmt image, std::string sFileName)
        : field(image), sFilename(sFilename), logInst()
    {
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
    IterativeSolver(image_fmt field, image_fmt guess, iterative_func func,
            double dStopCriterion, std::string sFilename = "")
        : Solver(field, sFilename) , func(func), dStopCriterion(dStopCriterion) {}
};

// class DirectSolver : Solver
// {
//     DirectSolver(image_fmt image, std::string sFilename)
//         : Solver(image, sFilename) {}
// };

} /* EndOfNamespace */

#endif
