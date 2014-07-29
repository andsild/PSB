/*
% a is the input..
     w = [0.06110 0.26177 0.53034 0.65934  0.51106 0.05407 0.24453 0.57410];

        h1 = w(1:4); % first four elements
        h1 = [h1 h1(end-1:-1:1)]; % reverse and beginning
        h1 = h1' * h1; % pow(2)

        h2 = h1*w(5); % h1 multiplied by center kernel value

        g = w(6:8); % last elements
        g = [g g(end-1:-1:1)]; % reverse and beginning
        g = g' * g;  % pow(@)
        result = pyconv( -div(:,:,i), h1, h2, g ); % convolution by L2 norm

 %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function ahat = pyconv( a, h1, h2, g )
%EVALF Evaluate convolution pyramid
%   Evaluate convolution pyramid on input a using
%   filter set specified by: h1,h2,g.
%   For more details see:
%   http://www.cs.huji.ac.il/labs/cglab/projects/convpyr

[h,w] = size(a); % h = rowSize, w = colSize, aka width/height
maxLevel = ceil(log2(max(h,w))); %
fs = size(h1,1); % size of first dim of h1

% Forward transform (analysis)
% pyr = pyrmaid
pyr{1} = padarray(a, [fs fs]); %  pad with "fs" zeroes in each direction
for i=2:maxLevel % for each element

    down = imfilter(pyr{i-1},h1,0); % filter previous pyramid with h1, put in down
    down = down(1:2:end,1:2:end); % extract element 1 + 2n, repeat them down
                                  % in 2d

    down = padarray(down, [fs fs]); % pad the array ( again )
    pyr{i} = down; % set current level in pyramid to down

end

% Backward transform (synthesis)
% similar to loop aboves, upsamling..
fpyr{maxLevel} = imfilter(pyr{maxLevel},g,0);
for i=maxLevel-1:-1:1

    rd = fpyr{i+1}; % fetch from prev pyramid
    rd = rd(1+fs:end-fs, 1+fs:end-fs); % fetch the kernel

    up = zeros(size(pyr{i})); % zero array
    up(1:2:end,1:2:end) = rd; % pad every 1 + 2n elements with 0, interpolation step

    fpyr{i} = imfilter(up,h2,0) + imfilter(pyr{i},g,0); % apply mask, boundary=0
end

ahat = fpyr{1};
ahat = ahat(1+fs:end-fs, 1+fs:end-fs);
*/

#include "wavelet.hpp"

#include <math.h>
#include <iostream>

#include "CImg.h"
#include "loginstance.hpp"
#include "image2.hpp"
#include "image_types.hpp"

using namespace cimg_library;
using namespace logging;

namespace wavelet
{

/* Boundary conditions:
    0 = pad with zero
    1 = repeat last value
    2 = repeat from start
*/

void pyconv(const image_fmt &field, image_fmt &retImg)
{
    const int SCALE = 2;
    const int WEIGHTS_LEN = 8;
   
    data_fmt vals[] = { 0.06110, 0.26177, 0.53034, 0.65934, 0.51106, 0.05407, 0.24453, 0.57410};
    const image_fmt weights(vals, WEIGHTS_LEN);
    MLOG(severity_type::debug, "Weights:\n", image_psb::printImage(weights));

    image_fmt forward_mask = weights.get_resize(4,1,1,1,0);
    forward_mask.resize(WEIGHTS_LEN - 1, 1, 1, 1, 0, 0);
    forward_mask.draw_image(4,0,0,0,forward_mask.get_resize(3,1,1,1,0).mirror('x'));
    forward_mask = forward_mask.get_transpose() * forward_mask;

    MLOG(severity_type::debug, "Forward mask:\n", image_psb::printImage(forward_mask));

    const image_fmt backward_mask = forward_mask * weights(4);
    MLOG(severity_type::debug, "Backward mask:\n", image_psb::printImage(backward_mask));

    image_fmt g = weights.get_crop(5,0,0,0, WEIGHTS_LEN - 1, 0, 0, 0);
    g.resize(WEIGHTS_LEN - 3, 1, 1, 1, 0, 0);
    g.draw_image(3,0,0,0,g.get_resize(2,1,1,1,0).mirror('x'));
    g = g.get_transpose() * g;
    MLOG(severity_type::debug, "Hacky G:\n", image_psb::printImage(g));

    // image_fmt g(5, 1, 1, 1,
    //             0.05407, 0.24453, 0.57410, 0.24453,0.05407);
    // g = g.get_transpose().dot(g);

    // MLOG(severity_type::debug, "Weights:\n", image_psb::printImage(weights));
    // weights.unroll('y');
    // MLOG(severity_type::debug, "Weights:\n", image_psb::printImage(weights));
    // MLOG(severity_type::debug, "g mask\n", image_psb::printImage(g));
    // MLOG(severity_type::debug, "backmask:\n", image_psb::printImage(backward_mask));

    //
    // int iPyrWidth = field.width(),
    //     iPyrHeight = field.height();
    // int iMaxLevel = ceil(cimg::log2(cimg::max(iPyrWidth, iPyrHeight)));
    //
    // image_fmt curPyr = field.get_resize(iPyrWidth, iPyrWidth, 1, 1, 0, 0);
    //
    // iPyrWidth = floor((double)iPyrWidth / (double)SCALE);
    // iPyrHeight = floor((double)iPyrHeight / (double)SCALE);
    // // iPyrWidth--;iPyrHeight--;
    //
    // const int iDrawXpos = (field.width()/ SCALE) - (iPyrWidth/ 2),
    //          iDrawYpos = (field.height()/ SCALE) - (iPyrHeight/ 2);
    //
    // MLOG(severity_type::debug, "Max level set to: ", iMaxLevel);
    // MLOG(severity_type::debug, "Before iterations\n",
    //                             image_psb::printImage(curPyr));
    //
    // /* Forward transform */
    // for(int iPos = 0; iPos < iMaxLevel; iPos++)
    // {
    //     curPyr.correlate(forward_mask);
    //     image_fmt tmp = curPyr.get_shift(-1,-1, 0, 0, 1)
    //                     .get_resize(iPyrWidth, iPyrHeight, 1, 1, 1);
    //     curPyr = curPyr.get_fill(0)
    //                    .draw_image( (field.width() / SCALE) - (iPyrWidth/ 2),
    //                                 (field.height()/ SCALE) - (iPyrHeight / 2),
    //                                 0, 0, tmp, 1);
    //     MLOG(severity_type::debug, "Forward analysis: step ", iPos, "\n", image_psb::printImage(tmp));
    //     MLOG(severity_type::debug, "Forward analys: step ", iPos, ", drawing at ", iDrawXpos, ",", iDrawYpos, "\n", image_psb::printImage(curPyr));
    //
    // }
    // /* Backward transform */
    // for(int iPos = 0; iPos < iMaxLevel; iPos++)
    // {
    //     image_fmt tmp = curPyr.get_crop(iDrawXpos, iDrawYpos, 0, 0,
    //                     iDrawXpos + iPyrWidth,
    //                     iDrawYpos + iPyrHeight, 0, 0);
    //     MLOG(severity_type::debug, "Backward analysis: step ", iPos, "\n",
    //                                 image_psb::printImage(tmp));
    //     tmp.resize(field.width(), field.height(), 1, 1,
    //                   4, 0);
    //     curPyr = tmp.get_correlate(backward_mask) + curPyr.get_convolve(g);
    //     MLOG(severity_type::debug, "Backward analysis: step ", iPos, "\n", image_psb::printImage(curPyr));
    // }
}

} /* EndOfNamespace */
