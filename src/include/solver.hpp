#ifndef SOLVER_H
#define  SOLVER_H

#define BORDER_SIZE 1

#include <iostream>
#include <vector>
#include <string>

#include "image_types.hpp"
#include "loginstance.hpp"
#include "file.hpp"

namespace solver
{

class Solver
{
    private:
    std::string sFilename,
                sLabel;

    protected:
    const image_fmt &origImage;
    image_fmt &field;
    logging::Logger< logging::FileLogPolicy > logInst;
    bool bMultipart, bFinal;

    public:
    virtual image_fmt solve() = 0;
    void alterField(double);
    std::string getLabel();
    std::string getFilename();
    bool isMultipart();
    bool isFinal();
    Solver(const image_fmt &origImage, image_fmt &field, std::string sFile, std::string sLab, bool bMulPart, bool bFin = false)
        : field(field), origImage(origImage), logInst(),
            sFilename(sFile), sLabel(sLab), bMultipart(bMulPart), bFinal(bFin)
    {
        this->logInst.setName(
                file_IO::SAVE_PATTERN.getLogname(this->sFilename, this->sLabel, false));
    }
};

class IterativeSolver : public virtual Solver
{
    private:
    iterative_func func;
    const image_fmt &guess;
    double dStopCriterion;

    public:
    image_fmt solve();
    IterativeSolver(const image_fmt &origImage, image_fmt &field,
            const image_fmt &U, iterative_func func,
            double dStopCriterion, std::string sFile, std::string sLabel,
            bool bMultiPart, bool bFinal = false)
        : Solver(origImage, field, sFile, sLabel, bMultiPart, bFinal), func(func),
            dStopCriterion(dStopCriterion), guess(U)
    {
    }
};

class DirectSolver : public virtual Solver
{
    private:
    direct_func func;
    public:
    image_fmt solve();
    DirectSolver(const image_fmt &origImage, image_fmt &field, direct_func func,
            std::string sFilename, std::string sLabel, bool bMultiPart, bool bFinal = false)
        : Solver(origImage, field, sFilename, sLabel, bMultiPart, bFinal), func(func)
    {
    }
};

} /* EndOfNamespace */


#endif
