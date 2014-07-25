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
    const image_fmt &origImage;
    image_fmt &field;
    logging::Logger< logging::FileLogPolicy > logInst;
    bool bMultipart, bFinal;

    public:
    virtual image_fmt solve(std::vector<double> &arg) = 0;
    void alterField(double);
    std::string getLabel();
    std::string getFilename();
    void log(int, std::string);
    bool isMultipart();
    bool isFinal();
    Solver(const image_fmt &origImage, image_fmt &field, std::string sFile, std::string sLab, bool bMulPart, bool bFin = false)
        : field(field), origImage(origImage), logInst(),
            sFilename(sFile), sLabel(sLab), bMultipart(bMulPart), bFinal(bFin)
    {
        this->logInst.setName(
                file_IO::SAVE_PATTERN.getLogname(this->sFilename, this->sLabel, false));
        this->logInst.setLevel(GETLEVEL);

        DO_IF_LOGLEVEL(severity_type::extensive)
        {
            this->logInst.print< severity_type::extensive>("Initial image:\n", image_psb::printImage(this->origImage));
            this->logInst.print< severity_type::extensive>("Initial field:\n", image_psb::printImage(this->field));
        }

    }
};

class IterativeSolver : public virtual Solver
{
    private:
    iterative_func func;
    const image_fmt &guess;
    double dStopCriterion;

    public:
    image_fmt solve(std::vector<double> &arg1);
    IterativeSolver(const image_fmt &origImage, image_fmt &field,
            const image_fmt &U, iterative_func func,
            double dStopCriterion, std::string sFile, std::string sLabel,
            bool bMultiPart, bool bFinal = false)
        : Solver(origImage, field, sFile, sLabel, bMultiPart, bFinal), func(func),
            dStopCriterion(dStopCriterion), guess(U)
    {
        DO_IF_LOGLEVEL(severity_type::extensive)
            this->logInst.print< severity_type::extensive>("Initial guess:\n", image_psb::printImage(this->guess));
    }
};

class DirectSolver : public virtual Solver
{
    private:
    direct_func func;
    public:
    image_fmt solve(std::vector<double> &arg1);
    DirectSolver(const image_fmt &origImage, image_fmt &field, direct_func func,
            std::string sFilename, std::string sLabel, bool bMultiPart, bool bFinal = false)
        : Solver(origImage, field, sFilename, sLabel, bMultiPart, bFinal), func(func)
    {
    }
};

} /* EndOfNamespace */


#endif
