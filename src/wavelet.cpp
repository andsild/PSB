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

    fpyr{i} = imfilter(up,h2,0) + imfilter(pyr{i},g,0); % apply mask
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

void pyconv(image_fmt input)
{
    const int SUBSAMPLING_FACTOR = 2;

    image_fmt forward_mask(7, 1, 1, 1,
            0.06110, 0.26177, 0.53034, 0.65934, 0.53034, 0.26177, 0.6110);
    forward_mask = forward_mask.get_transpose().dot(forward_mask);
    image_fmt backward_mask = (forward_mask * 0.51106);
    image_fmt g(5, 1, 1, 1,
                0.05407, 0.24453, 0.57410, 0.24453,0.05407);
    g = g.get_transpose().dot(g);

    int iWidth = input.width(), iHeight = input.height();
    int iSampleWidth = iWidth / SUBSAMPLING_FACTOR,
        iSampleHeight = iHeight / SUBSAMPLING_FACTOR;
    int iMaxLevel = ceil(cimg::log2(cimg::max(iWidth, iHeight)));
    CLOG(severity_type::debug)("Max level set to: ", iMaxLevel);
    LOG(severity_type::debug)("Max level set to: ", iMaxLevel);

    image_fmt curPyr = input.get_resize(iWidth, iWidth, 1, 1, 0, 0);

    CLOG(severity_type::debug)("Before iterations\n", image_psb::printImage(curPyr));
    LOG(severity_type::debug)("Before iterations:\n", image_psb::printImage(curPyr));

    for(int iPos = 1; iPos < iMaxLevel; iPos++)
    {
        curPyr.convolve(forward_mask);
        image_fmt tmp(iSampleWidth, iSampleHeight, 1, 1);
        // Or I could just convolve using a 1,0 mask?
        cimg_forXY(curPyr, x, y)
        {
            if(x % SUBSAMPLING_FACTOR == 0 || y % SUBSAMPLING_FACTOR == 0) continue;
            int iRow = y / 2,
                iCol = x / 2;
            tmp(iCol, iRow) = curPyr(x,y);
            // tmp(x,y) = curPyr(x,y);
        }
        CLOG(severity_type::debug)("Forward analys: step ", iPos, "\n", image_psb::printImage(tmp));
        image_fmt newPyr(iWidth, iHeight, 1, 1, 0);
        curPyr = newPyr.draw_image(iSampleWidth / 2 * iPos, iSampleHeight / 2 * iPos, 0, 0, tmp, 1);
        curPyr = newPyr;

        CLOG(severity_type::debug)("Forward analys: step ", iPos, "\n", image_psb::printImage(curPyr));
    }

    // CLOG(severity_type::debug)("After forward analysis\n", image_psb::printImage(curPyr));
    // LOG(severity_type::debug)("After forward analysis\n", image_psb::printImage(curPyr));
    //
    // for(int iPos = 1; iPos < iMaxLevel; iPos++)
    // {
    //     image_fmt tmp(iWidth, iWidth, 1, 1);
    //     cimg_forXY(tmp, x, y)
    //     {
    //         if(x % 2 == 0 || y % 2 == 0) continue;
    //         int iRow = y / 2,
    //             iCol = x / 2;
    //         tmp(x, y) = curPyr(iRow, iCol);
    //     }
    //     curPyr = tmp;
    //     break;
    // }
    //
    // CLOG(severity_type::debug)("Finished image\n", image_psb::printImage(curPyr));
    // LOG(severity_type::debug)("Finished image\n", image_psb::printImage(curPyr));
}

} /* EndOfNamespace */
