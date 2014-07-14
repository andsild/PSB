/*
% a is the input..
     w = [0.06110 0.26177 0.53034 0.65934  0.51106 0.05407 0.24453 0.57410];

        h1 = w(1:4); % first four elements
        h1 = [h1 h1(end-1:-1:1)]; % reverse
        h1 = h1' * h1; % pow(2)

        h2 = h1*w(5); % h1 multiplied by center kernel value

        g = w(6:8); % last elements
        g = [g g(end-1:-1:1)]; % reverse
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

using namespace image_psb;
using namespace cimg_library;
using namespace logging;

namespace wavelet
{

void pyconv(image_fmt input)
{
    image_fmt forward_mask(4, 1, 1, 1,
            0.06110, 0.26177, 0.53034, 0.65934);
    forward_mask.dot(forward_mask);
    image_fmt backward_mask = (forward_mask * 5.0);
    image_fmt g(2, 1, 1, 1,
                0.05407, 0.24453, 0.57410);

    int iWidth = input.width(), iHeight = input.height();
    int iHalfWidth = (iWidth / 2),
        iHalfHeight = (iHeight / 2);
    int iMaxLevel = ceil(cimg::log2(cimg::max(iWidth, iHeight)));
    CLOG(severity_type::debug)("Max level set to: ", iMaxLevel);
    LOG(severity_type::debug)("Max level set to: ", iMaxLevel);

    image_fmt curPyr = input.get_resize(iWidth, iWidth, 1, 1, 0, 0);

    CLOG(severity_type::debug)("Before iterations\n", printImage(curPyr));
    LOG(severity_type::debug)("Before iterations:\n", printImage(curPyr));

    for(int iPos = 1; iPos < iMaxLevel; iPos++)
    {
        curPyr.convolve(forward_mask);
        image_fmt tmp(iHalfWidth, iHalfHeight, 1, 1);
        // Or I could just convolve using a 1,0 mask?
        cimg_forXY(curPyr, x, y)
        {
            if(x % 2 == 0 || y % 2 == 0) continue;
            int iRow = y / 2,
                iCol = x / 2;
            tmp(iCol, iRow) = curPyr(x,y);
            // tmp(x,y) = curPyr(x,y);
        }
        CLOG(severity_type::debug)("Forward analys: step ", iPos, "\n", printImage(tmp));
        image_fmt newPyr(iWidth, iHeight, 1, 1, 0);
        curPyr = newPyr.draw_image(iHalfWidth / 2 * iPos, iHalfWidth / 2 * iPos, 0, 0, tmp, 1);
        curPyr = newPyr;

        CLOG(severity_type::debug)("Forward analys: step ", iPos, "\n", printImage(curPyr));
    }

    // CLOG(severity_type::debug)("After forward analysis\n", printImage(curPyr));
    // LOG(severity_type::debug)("After forward analysis\n", printImage(curPyr));
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
    // CLOG(severity_type::debug)("Finished image\n", printImage(curPyr));
    // LOG(severity_type::debug)("Finished image\n", printImage(curPyr));
}

} /* EndOfNamespace */
