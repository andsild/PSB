#ifndef SOLVER_H
#define  SOLVER_H

#define BORDER_SIZE 1

#include <vector>
#include <string>

#include "image_types.hpp"
#include "image2.hpp"
#include "loginstance.hpp"
#include "file.hpp"

using namespace logging;

namespace solver
{

class Solver
{
    private:
    std::string sFilename,
                sLabel;

    protected:
    const image_fmt origImage,
                    field;
    logging::Logger< logging::FileLogPolicy > logInst;
    bool bMultipart, bFinal;

    public:
    virtual image_fmt solve(rawdata_fmt &arg) = 0;
    std::string getLabel();
    std::string getFilename();
    void log(int, std::string);
    bool isMultipart();
    bool isFinal();
    Solver(const image_fmt *origImage, const image_fmt *field,
            std::string sFile, std::string sLab,
            bool bMulPart, bool bFin = false);
};

class IterativeSolver : public virtual Solver
{
    private:
    iterative_func func;
    const image_fmt guess;
    double dStopCriterion;

    public:
    image_fmt solve(rawdata_fmt &arg1);
    IterativeSolver(const image_fmt* const origImage, const image_fmt *field,
            const image_fmt* const U, iterative_func func,
            double dStopCriterion, std::string sFile, std::string sLabel,
            bool bMultiPart, bool bFinal = false);
};

class DirectSolver : public virtual Solver
{
    private:
    direct_func func;
    const bool isDirichet;
    public:
    image_fmt solve(rawdata_fmt &arg1);
    DirectSolver(const image_fmt* const origImage, const image_fmt* const field,
                direct_func func,
                std::string sFilename, std::string sLabel,
                bool dirichet,
                bool bMultiPart = false, bool bFin = false);
};

} /* EndOfNamespace */


#endif
