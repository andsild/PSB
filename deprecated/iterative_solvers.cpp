double findError(const d1 origData, const d1 newData, int iLength)
{
    double dError = 0;
    for(std::vector<int>::size_type iPos = 0; iPos != origData.size(); iPos++)
    {
        dError += calculateError(origData[iPos],
                                 newData[iPos]);
    }
    return dError /= iLength;

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
