#ifndef SOLVER_H
#define  SOLVER_H

#define BORDER_SIZE 1

#include <string>

#include "image_types.hpp"
#include "loginstance.hpp"

namespace solver
{

class Solver
{
    private:
    std::string sFilename;

    protected:
    virtual void postProsess() = 0;
    virtual void writeData() = 0;

    const image_fmt &origImage;
    image_fmt &field;
    logging::Logger< logging::FileLogPolicy > logInst;

    public:
    virtual image_fmt solve() = 0;
    void alterField(double);
    Solver(const image_fmt &origImage, image_fmt &field, std::string sFileName)
        : field(field), origImage(origImage), logInst()
    {
        this->logInst.setName(std::string("poop"));
        this->sFilename = sFilename;
    }
};

class IterativeSolver : public virtual Solver
{
    private:
    iterative_func func;
    image_fmt &guess;
    double dStopCriterion;
    void betweenIter();
    void postProsess();
    void writeData();

    public:
    image_fmt solve();
    IterativeSolver(const image_fmt &origImage, image_fmt &field, image_fmt &guess, iterative_func func,
            double dStopCriterion, std::string sFilename = "")
        : Solver(origImage, field, std::string()) , func(func), dStopCriterion(dStopCriterion), guess(guess)
        {
        }
};

// class DirectSolver : Solver
// {
//     DirectSolver(image_fmt image, std::string sFilename)
//         : Solver(image, sFilename) {}
// };

} /* EndOfNamespace */


#endif
