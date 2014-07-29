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

void pyconv(const image_fmt &field, image_fmt &retImg)
{
    const int SUBSAMPLING_FACTOR = 2;

    image_fmt forward_mask(7, 1, 1, 1,
            0.06110, 0.26177, 0.53034, 0.65934, 0.53034, 0.26177, 0.6110);
    forward_mask = forward_mask.get_transpose().dot(forward_mask);
    image_fmt backward_mask = (forward_mask * 0.51106);
    image_fmt g(5, 1, 1, 1,
                0.05407, 0.24453, 0.57410, 0.24453,0.05407);
    g = g.get_transpose().dot(g);
    //
    int iPyrWidth = field.width(),
        iPyrHeight = field.height();
    int iMaxLevel = ceil(cimg::log2(cimg::max(iPyrWidth, iPyrHeight)));
    MLOG(severity_type::debug, "Max level set to: ", iMaxLevel);

    image_fmt curPyr = field.get_resize(iPyrWidth, iPyrWidth, 1, 1, 0, 0);

    MLOG(severity_type::debug, "Before iterations\n", image_psb::printImage(curPyr));
    iPyrWidth = ceil(iPyrWidth / SUBSAMPLING_FACTOR);
    iPyrHeight = ceil(iPyrHeight / SUBSAMPLING_FACTOR);


    const int iDrawXpos = (field.width()/ SUBSAMPLING_FACTOR) - (iPyrWidth/ 2),
                iDrawYpos = (field.height()/ SUBSAMPLING_FACTOR) - (iPyrHeight/ 2);

    /* Forward transform */
    for(int iPos = 0; iPos < iMaxLevel; iPos++)
    {
        curPyr.correlate(forward_mask);
        image_fmt tmp = curPyr.get_shift(-1,-1, 0, 0, 1).get_resize(iPyrWidth, iPyrHeight, 1, 1, 1);
        curPyr = curPyr.get_fill(0).draw_image( (field.width() / SUBSAMPLING_FACTOR) - (iPyrWidth/ 2),
                                    (field.height()/ SUBSAMPLING_FACTOR) - (iPyrHeight / 2),
                                    0, 0, tmp, 1);
        MLOG(severity_type::debug, "Forward analysis: step ", iPos, "\n", image_psb::printImage(tmp));
        MLOG(severity_type::debug, "Forward analys: step ", iPos, ", drawing at ", iDrawXpos, ",", iDrawYpos, "\n", image_psb::printImage(curPyr));

    }
    /* Backward transform */
    for(int iPos = 0; iPos < iMaxLevel; iPos++)
    {
        image_fmt tmp = curPyr.get_crop(iDrawXpos, iDrawYpos, 0, 0,
                        iDrawXpos + iPyrWidth,
                        iDrawYpos + iPyrHeight, 0, 0);
        tmp.resize(field.width(), field.height(), 1, 1,
                      4, 0);
        curPyr = tmp.get_correlate(backward_mask) + curPyr.get_convolve(g);
        MLOG(severity_type::debug, "Backward analysis: step ", iPos, "\n", image_psb::printImage(tmp));
        MLOG(severity_type::debug, "Backward analysis: step ", iPos, "\n", image_psb::printImage(curPyr));
    }
}

} /* EndOfNamespace */
