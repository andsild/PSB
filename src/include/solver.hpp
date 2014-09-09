#ifndef SOLVER_H
#define  SOLVER_H

#include <vector>
#include <string>

#include "image_types.hpp"
#include "imageUtil.hpp"
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
    const image_fmt* const origImage;
    const image_fmt* const noisedImage;
    const image_fmt* const field;
    logging::Logger< logging::FileLogPolicy > logInst;
    bool bMultipart, bFinal;

    public:
    virtual image_fmt solve(rawdata_fmt &arg1, rawdata_fmt &arg2) = 0;
    std::string getLabel();
    std::string getFilename();
    void log(int, std::string);
    bool isMultipart();
    bool isFinal();
    void clear();
    Solver(const image_fmt* const origImage, const image_fmt* const field,
            std::string sFile, std::string sLab,
            bool bMulPart, bool bFin = false);
    Solver(const image_fmt* const origImage, const image_fmt* const noise,
            const image_fmt* const field,
            std::string sFile, std::string sLab,
            bool bMulPart, bool bFin = false);
    Solver();
};

class IterativeSolver : public virtual Solver
{
    private:
    iterative_func func;
    const image_fmt guess;
    double dStopCriterion;

    public:
    image_fmt solve(rawdata_fmt &arg1, rawdata_fmt &arg2);
    IterativeSolver(const image_fmt* const origImage, const image_fmt* field,
            const image_fmt * const U, iterative_func func,
            double dStopCriterion, std::string sFile, std::string sLabel,
            bool bMultiPart, bool bFinal = false);
    IterativeSolver();
};

class DirectSolver : public virtual Solver
{
    private:
    direct_func func;
    const bool isDirichet;
    public:
    image_fmt solve(rawdata_fmt &arg1, rawdata_fmt &arg2);
    DirectSolver(const image_fmt* const origImage, const image_fmt* const field,
                direct_func func,
                std::string sFilename, std::string sLabel,
                bool dirichet,
                bool bMultiPart = false, bool bFin = false);
    DirectSolver(const image_fmt* const origImage, const image_fmt* const noiseImg,
                const image_fmt* const field,
                direct_func func,
                std::string sFilename, std::string sLabel,
                bool dirichet,
                bool bMultiPart = false, bool bFin = false);
    DirectSolver();
};

} /* EndOfNamespace */


#endif
