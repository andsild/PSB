#include "iterative_solvers.hpp"

#include <string>
#include <vector>

#include "CImg.h"

#include "logger.hpp"
#include "image2.hpp"

using namespace logging;
using namespace cimg_library;

namespace pe_solver //[p]oison-[e]quation
{

// #include <cstdarg>
// std::std::string format2(const char* fmt, ...){
//     int size = 512;
//     char* buffer = 0;
//     buffer = new char[size];
//     va_list vl;
//     va_start(vl, fmt);
//     int nsize = vsnprintf(buffer, size, fmt, vl);
//     if(size<=nsize){ //fail delete buffer and try again
//         delete[] buffer;
//         buffer = 0;
//         buffer = new char[nsize+1]; //+1 for /0
//         nsize = vsnprintf(buffer, size, fmt, vl);
//     }
//     std::std::string ret(buffer);
//     va_end(vl);
//     delete[] buffer;
//     return ret;
// }
//
// std::string printer(CImg<double> image)
// {
//     std::stringstream ss;
//     for(int iPos = 0; iPos < image.height(); iPos++)
//     {
//         for(int jPos = 0; jPos < image.width(); jPos++)
//         {
//             ss << format2("%5.1f ", image(jPos, iPos));
//             // printf("%5.1f ",image(jPos,iPos));
//         }
//         ss << "\n";
//     }
//
//     return ss.str();
// }
//

/** jacobi iteration
 *
 */
inline void iterate_jacobi(const CImg<double> &F, CImg<double> &U, double iWidthLength,
                    int iLength, double &dDiff, double H = 1)
{
    dDiff = 0;
    int BORDER_SIZE = 1;
    CImg<double> copyU = U;
    CImg_3x3(I,double);

    cimg_for_in3x3(copyU, BORDER_SIZE, BORDER_SIZE,
                   U.width() - BORDER_SIZE - 1, U.height() - BORDER_SIZE - 1,
                   x,y,0,0,I,double) // uses Neumann borders
    // cimg_for3x3(copyU,x,y,0,0,I,double) // uses Neumann borders
    {
        double dOldVal = U(x,y);
        double dNewVal = .25 * (Icn + Icp + Ipc + Inc - F(x,y) * H * H);
        U(x,y) = dNewVal;
        double dCurDiff = (abs(dOldVal - dNewVal));
        if( dCurDiff > dDiff)
            dDiff = dCurDiff;
    }
}



/** Gauss-seidel iteration
 *
 */
inline void iterate_gauss(const CImg<double> &F, CImg<double> &U, double iWidthLength,
                           int iLength, double &dDiff, double H = 1)
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
        double dCurDiff = (abs(dOldVal - dNewVal));
        if( dCurDiff > dDiff)
            dDiff = dCurDiff;
    }
}

inline void iterate_sor(const CImg<double> &F, CImg<double> &U,
                 double iWidthLength, int iLength, double &dDiff, double H = 1)
{
    static double omega = 2 / (1 + (3.14 / iWidthLength ));
    static double dOmegaConstant = omega / 4;
    static double dNotOmega = (1 - omega);

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
        double dCurDiff = (abs(dOldVal - dNewVal));
        if( dCurDiff > dDiff)
            dDiff = dCurDiff;
    }
}



void two_grid(double h, CImg<double> &U, CImg<double> &F, int iWidthLength, int iSmoothFactor)
{
    double H = 2 * h;
    CImg<double> r(iWidthLength*2);
    CImg<double> R(iWidthLength);
    CImg<double> V(iWidthLength);
    int L2 = iWidthLength / 2;

    // solve exactly if there is only one interior point
    if (iWidthLength == 1) 
    {
        U[0] = 0.25;// * (U[0][1] + U[2][1] + U[1][0] + U[1][2] + h * h * F[1][1]);
        return;
    }

    // pre-smoothing
    for (int iPos = 0; iPos < iSmoothFactor; iPos++)
    {
        iterate_gauss(F, U, iWidthLength, iWidthLength * 2, h);
    }

    for (int yPos = iWidthLength;
         yPos <= iWidthLength;
         yPos += iWidthLength)
    {
        for (int xPos = 0; xPos <= iWidthLength; xPos++)
        {
            int iCurrent = xPos + yPos;
            r[iCurrent] = F[iCurrent] - (4 * U[iCurrent] / (h * h))
                            + ( U[iCurrent + 1] + U[iCurrent - 1]
                              + U[iCurrent - iWidthLength] 
                              + U[iCurrent + iWidthLength]);
        }
    }

    for (int yPos = iWidthLength; yPos <= L2; yPos += iWidthLength)
    {
        int iPos = 2 * yPos - 1;
        for (int xPos = 0; xPos <= L2; xPos++)
        {
            int jPos = 2 * jPos - 1;
            int iCurrent = (iPos+jPos);
            R[(yPos+xPos)] = 0.25 * 
                            ( r[iCurrent]
                              + r[iCurrent+iWidthLength]
                              + r[iCurrent + 1]
                              + r[iCurrent+1+iWidthLength]
                            );
        }
    }

    two_grid(H, V, R, L2, iSmoothFactor);
    CImg<double> v(iWidthLength*2);

    for (int yPos = iWidthLength; yPos <= L2; yPos += iWidthLength)
    {
        int iPos = 2 * yPos - 1;
        for (int xPos = 0; xPos <= L2; xPos++)
        {
            int jPos = 2 * jPos - 1;
            int iCurrent = (iPos+jPos);
            v[(xPos + yPos)] = v[iCurrent+iWidthLength] 
                             = v[iCurrent+1]
                             = v[iCurrent+1+iWidthLength]
                             = V[(xPos+yPos)];
        }
    }

    // correct U
    for(std::vector<int>::size_type iPos = iWidthLength;
            iPos < iWidthLength * 2;
            iPos++) 
    {
            U[iPos] += v[iPos];
    }

    // post-smoothing Gauss-Seidel
    for (int iPos = 0; iPos < iSmoothFactor; iPos++)
    {
        iterate_gauss(F, U, iWidthLength, iWidthLength * 2, h);
    }
}

std::vector<std::string> iterative_solve(iterative_function function,
                    const CImg<double> solution, CImg<double> &guess, CImg<double> rho,
                    double dMaxErr, int iWidth, logging::logger< logging::file_log_policy > &logInst) 
{
    CImg<double> old_guess = guess, newGuess = guess;
    double dRelativeError = 0;
    std::vector<std::string> vOutput;
    int iLength = solution.width() * solution.height();

    int iIter = 0;
    do
    {
        double dOld = dRelativeError;
        function(rho, newGuess, iWidth, iLength, dRelativeError, 1);
        // if (dOld == dRelativeError)
        // {
        //     cout << "Method converged (no change in iteration) with value: "
        //          << dOld << endl;
        //     break;
        // }

        // cout << "New guess:" << endl; printer(newGuess);
        (logInst.print<severity_type::debug>)("New guess\n", image_psb::printImage(newGuess));
        CLOG(severity_type::debug)("New guess\n", image_psb::printImage(newGuess));

        if(iIter % 50 == 0) 
        {
            logInst.print<severity_type::info>("solving: ", dRelativeError);
            CLOG(severity_type::info)("solving: ", dRelativeError);
        }

       double dMSE = newGuess.MSE(solution);
       vOutput.push_back(std::to_string(dMSE));

        (logInst.print<severity_type::extensive>)("Image residual(mean): ", dMSE);
        CLOG(severity_type::extensive)("Image residual(mean): ", dMSE);
        (logInst.print<severity_type::extensive>)("Iteration diff(max): ", dRelativeError);
        CLOG(severity_type::extensive)("Iteration diff(max): ", dRelativeError);

        old_guess = newGuess;
        iIter++;
    } while(dRelativeError > dMaxErr);

    CLOG(severity_type::info)("Finished in ", iIter, " iterations");
    (logInst.print<severity_type::info>)("Finished in ", iIter, " iterations");
    guess = newGuess;

    return vOutput;
}

} // EndOfNamespace

/* EOF */
