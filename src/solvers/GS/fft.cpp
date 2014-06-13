#ifndef _SOLVER_FFT
#define _SOLVER_FFT 1

#include <iostream>
#include <cmath>
#include <algorithm>
#include <vector>
#include <stdio.h>
#include <cmath>
#include <string>

#include "../../main.cpp"
#include "gauss_image.cpp"
#include "../../../lib/CImg-1.5.8/CImg.h"

using namespace cimg_library;
using namespace std;

namespace pe_solver
{
double q = 10;                       // point charge
Matrix<complex<double>,2> rho(N, N);
for (int j = 0; j < N; j++) {
    for (int k = 0; k < N; k++) {
        if (j == N/2 && k == N/2)    // at center of lattice
            rho[j][k] = q / (h * h);
        else
            rho[j][k] = 0.0;
    }
}

// FFT rows of rho
Matrix<complex<double>,1> f(N);      // to store rows and columns
for (int j = 0; j < N; j++) {
    for (int k = 0; k < N; k++)
        f[k] = rho[j][k];
    fft(f);
    for (int k = 0; k < N; k++)
        rho[j][k] = f[k];
}
// FFT columns of rho
for (int k = 0; k < N; k++) {
    for (int j = 0; j < N; j++)
        f[j] = rho[j][k];
    fft(f);
    for (int j = 0; j < N; j++)
        rho[j][k] = f[j];
}

// solve equation in Fourier space
Matrix<complex<double>,2> V(N, N);
complex<double> i(0.0, 1.0);
double pi = 4 * atan(1.0);
complex<double> W = exp(2.0 * pi * i / double(N));
complex<double> W_m = 1.0, W_n = 1.0;
for (int m = 0; m < N; m++) {
    for (int n = 0; n < N; n++) {
        complex<double> denom = 4.0;
        denom -= W_m + 1.0 / W_m + W_n + 1.0 / W_n;
        if (denom != 0.0)
            V[m][n] = rho[m][n] * h * h / denom;
        W_n *= W;
    }
    W_m *= W;
}

// inverse FFT rows of V
for (int j = 0; j < N; j++) {
    for (int k = 0; k < N; k++)
        f[k] = V[j][k];
    fft_inv(f);
    for (int k = 0; k < N; k++)
        V[j][k] = f[k];
}
// inverse FFT columns of V
for (int k = 0; k < N; k++) {
    for (int j = 0; j < N; j++)
        f[j] = V[j][k];
    fft_inv(f);
    for (int j = 0; j < N; j++)
        V[j][k] = f[j];
}

}
#endif
