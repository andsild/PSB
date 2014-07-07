#include "iterative_solvers.hpp"

#include <string>
#include <vector>

#include "CImg.h"

#include "loginstance.hpp"
#include "image2.hpp"

using namespace logging;
using namespace cimg_library;

namespace pe_solver //[p]oison-[e]quation
{

/** jacobi iteration
 *
 */
void iterate_jacobi(const CImg<double> &F, CImg<double> &U,
                          double iWidthLength, int iLength, double &dDiff,
                          double H)
{
    dDiff = 0;
    int BORDER_SIZE = 1;
    CImg<double> copyU = U;
    CImg_3x3(I,double);

    cimg_for_in3x3(copyU, BORDER_SIZE, BORDER_SIZE,
                   U.width() - BORDER_SIZE - 1, U.height() - BORDER_SIZE - 1,
                   x,y,0,0,I,double) // uses Neumann borders
    {
        double dOldVal = Icc;
        double dNewVal = .25 * (Icn + Icp + Ipc + Inc - F(x,y));
        U(x,y) = dNewVal;
        double dCurDiff = fabs(dOldVal - dNewVal);
        if( dCurDiff > dDiff)
            dDiff = dCurDiff;
    }
}

/** Gauss-seidel iteration
 *
 */
void iterate_gauss(const CImg<double> &F, CImg<double> &U,
                          double iWidthLength, int iLength, double &dDiff,
                          double H)
{
    dDiff = 0;
    int BORDER_SIZE = 1;
    CImg_3x3(I,double);

    cimg_for_in3x3(U, BORDER_SIZE, BORDER_SIZE,
                   U.width() - BORDER_SIZE - 1, U.height() - BORDER_SIZE - 1,
                   x,y,0,0,I,double) // uses Neumann borders
    {
        double dOldVal = U(x,y);
        double dNewVal = .25 * ( Icn + Icp + Ipc + Inc - F(x,y) * H * H);
        U(x,y) = dNewVal;
        double dCurDiff = fabs(dOldVal - dNewVal);
        if( dCurDiff > dDiff)
            dDiff = dCurDiff;
    }
}

void iterate_sor(const CImg<double> &F, CImg<double> &U,
                 double iWidthLength, int iLength, double &dDiff, double H)
{
    const double omega = 2 / (1 + (3.14 / iWidthLength ));
    const double dOmegaConstant = omega / 4;
    const double dNotOmega = (1 - omega);

    dDiff = 0;
    int BORDER_SIZE = 1;
    CImg_3x3(I,double);

    cimg_for_in3x3(U, BORDER_SIZE, BORDER_SIZE,
                   U.width() - BORDER_SIZE - 1, U.height() - BORDER_SIZE - 1,
                   x,y,0,0,I,double) // uses Neumann borders
    {
        double dOldVal = U(x,y);
        double dNewVal = (dNotOmega * Icc)
                          + (dOmegaConstant)
                          * (Icn + Icp + Ipc + Inc - F(x,y) * H * H);
        U(x,y) = dNewVal;
        double dCurDiff = (fabs(dOldVal - dNewVal));
        if( dCurDiff > dDiff)
            dDiff = dCurDiff;
    }
}

void two_grid(double h, CImg<double> &U, CImg<double> &F, int iWidthLength, int iSmoothFactor)
{
    // double H = 2 * h;
    // CImg<double> r(iWidthLength*2);
    // CImg<double> R(iWidthLength);
    // CImg<double> V(iWidthLength);
    // int L2 = iWidthLength / 2;
    //
    // // solve exactly if there is only one interior point
    // if (iWidthLength == 1) 
    // {
    //     U[0] = 0.25;// * (U[0][1] + U[2][1] + U[1][0] + U[1][2] + h * h * F[1][1]);
    //     return;
    // }
    //
    // // pre-smoothing
    // for (int iPos = 0; iPos < iSmoothFactor; iPos++)
    // {
    //     iterate_gauss(F, U, iWidthLength, iWidthLength * 2, h);
    // }
    //
    // for (int yPos = iWidthLength;
    //      yPos <= iWidthLength;
    //      yPos += iWidthLength)
    // {
    //     for (int xPos = 0; xPos <= iWidthLength; xPos++)
    //     {
    //         int iCurrent = xPos + yPos;
    //         r[iCurrent] = F[iCurrent] - (4 * U[iCurrent] / (h * h))
    //                         + ( U[iCurrent + 1] + U[iCurrent - 1]
    //                           + U[iCurrent - iWidthLength] 
    //                           + U[iCurrent + iWidthLength]);
    //     }
    // }
    //
    // for (int yPos = iWidthLength; yPos <= L2; yPos += iWidthLength)
    // {
    //     int iPos = 2 * yPos - 1;
    //     for (int xPos = 0; xPos <= L2; xPos++)
    //     {
    //         int jPos = 2 * jPos - 1;
    //         int iCurrent = (iPos+jPos);
    //         R[(yPos+xPos)] = 0.25 * 
    //                         ( r[iCurrent]
    //                           + r[iCurrent+iWidthLength]
    //                           + r[iCurrent + 1]
    //                           + r[iCurrent+1+iWidthLength]
    //                         );
    //     }
    // }
    //
    // two_grid(H, V, R, L2, iSmoothFactor);
    // CImg<double> v(iWidthLength*2);
    //
    // for (int yPos = iWidthLength; yPos <= L2; yPos += iWidthLength)
    // {
    //     int iPos = 2 * yPos - 1;
    //     for (int xPos = 0; xPos <= L2; xPos++)
    //     {
    //         int jPos = 2 * jPos - 1;
    //         int iCurrent = (iPos+jPos);
    //         v[(xPos + yPos)] = v[iCurrent+iWidthLength] 
    //                          = v[iCurrent+1]
    //                          = v[iCurrent+1+iWidthLength]
    //                          = V[(xPos+yPos)];
    //     }
    // }
    //
    // // correct U
    // for(std::vector<int>::size_type iPos = iWidthLength;
    //         iPos < iWidthLength * 2;
    //         iPos++) 
    // {
    //         U[iPos] += v[iPos];
    // }
    //
    // // post-smoothing Gauss-Seidel
    // for (int iPos = 0; iPos < iSmoothFactor; iPos++)
    // {
    //     iterate_gauss(F, U, iWidthLength, iWidthLength * 2, h);
    // }
}

std::vector<std::string> iterative_solve(iterative_function function,
                    const CImg<double> solution, CImg<double> &guess, CImg<double> rho,
                    double dMaxErr, int iWidth, logging::Logger< logging::FileLogPolicy > &logInst) 
{
    CImg<double> old_guess = guess, newGuess = guess;
    double dRelativeError = 0;
    std::vector<std::string> vOutput;
    int iLength = solution.width() * solution.height();

    int iIter = 0;
    do
    {
        double dOld = dRelativeError;
        function(rho, newGuess, iWidth, iLength, dRelativeError, 1.0);

       if(logInst.getLevel() >= severity_type::extensive)
           (logInst.print<severity_type::debug>)("New guess\n", image_psb::printImage(newGuess));
       if(CGETLEVEL >= severity_type::extensive)
           CLOG(severity_type::debug)("New guess\n", image_psb::printImage(newGuess));

        if(iIter % 50 == 0) 
        {
            logInst.print<severity_type::info>("Iteration: # ",iIter, "\titeration diff: ", dRelativeError);
            CLOG(severity_type::info)("Iteration: # ",iIter, "\titeration diff: ", dRelativeError);
        }

       double dMSE = newGuess.MSE(solution);
       vOutput.push_back(std::to_string(dMSE));

        (logInst.print<severity_type::extensive>)("Image residual(mean): ", dMSE);
        (logInst.print<severity_type::extensive>)("Iteration diff(max): ", dRelativeError);
        CLOG(severity_type::extensive)("Iteration diff(max): ", dRelativeError);
        CLOG(severity_type::extensive)("Image residual(mean): ", dMSE);

        old_guess = newGuess;
        iIter++;

        /* dRelative error reached a ``weird'' value */
       if(dRelativeError == 0)
       {
           LOG(severity_type::warning)("Solver iteration diff reached 0 - something might have gone wrong");
           CLOG(severity_type::warning)("Solver iteration diff reached 0 - something might have gone wrong");
           break;
        }

        /* No change in iteration */
        // if (dOld == dRelativeError)
        // {
        //     CLOG(severity_type::warning)("Method converged (no change in iteration) with value: ", dOld);
        //     LOG(severity_type::warning)("Method converged (no change in iteration) with value: ", dOld);
        // }

    } while(dRelativeError > dMaxErr);

    CLOG(severity_type::info)("Finished in ", iIter, " iterations");
    (logInst.print<severity_type::info>)("Finished in ", iIter, " iterations");
    guess = newGuess;

    return vOutput;
}

} // EndOfNamespace

/* EOF */
