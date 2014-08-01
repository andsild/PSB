                function retval = imfilter(im, f, varargin)
                ## Check number of input arguments
                if (nargin < 2)
                    print_usage();
                endif
                
                ## Check image
                if (!ismatrix(im))
                    error("imfilter: first input argument must be an image");
                endif
                [imrows, imcols, imchannels, tmp] = size(im);
                if (tmp != 1 || (imchannels != 1 && imchannels != 3))
                    error("imfilter: first input argument must be an image");
                endif
                C = class(im);
                
                ## Check filter (XXX: matlab support 3D filter, but I have no idea what they do with them)
                if (!ismatrix(f))
                    error("imfilter: second input argument must be a matrix");
                endif
                [frows, fcols, tmp] = size(f);
                if (tmp != 1)
                    error("imfilter: second argument must be a 2-dimensional matrix");
                endif
                
                ## Parse options
                res_size = "same";
                res_size_options = {"same", "full"};
                pad = 0;
                pad_options = {"symmetric", "replicate", "circular"};
                ftype = "corr";
                ftype_options = {"corr", "conv"};
                for i = 1:length(varargin)
                    v = varargin{i};
                    if (any(strcmpi(v, pad_options)) || isscalar(v))
                    pad = v;
                    elseif (any(strcmpi(v, res_size_options)))
                    res_size = v;
                    elseif (any(strcmpi(v, ftype_options)))
                    ftype = v;
                    else
                    warning("imfilter: cannot handle input argument number %d", i+2);
                    endif
                endfor
                
                ## Pad the image
                im = padarray(im, floor([frows/2, fcols/2]), pad);
                if (mod(frows,2) == 0)
                    im = im(1:end-1, :, :);
                endif
                if (mod(fcols,2) == 0)
                    im = im(:, 1:end-1, :);
                endif
                
                ## Do the filtering
                if (strcmpi(res_size, "same"))
                    res_size = "valid";
                else # res_size == "full"
                    res_size = "same";
                endif
                if (strcmpi(ftype, "corr"))
                    for i = imchannels:-1:1
                    retval(:,:,i) = filter2(f, im(:,:,i), res_size);
                    endfor
                else
                    for i = imchannels:-1:1
                    retval(:,:,i) = conv2(im(:,:,i), f, res_size);
                    endfor
                endif
                
                ## Change the class of the output to the class of the input
                ## (the filtering functions returns doubles)
                retval = cast(retval, C);
                
                endfunction























## Copyright (C) 2013 Carnë Draug <carandraug@octave.org>
##
## This program is free software; you can redistribute it and/or modify it under
## the terms of the GNU General Public License as published by the Free Software
## Foundation; either version 3 of the License, or (at your option) any later
## version.
##
## This program is distributed in the hope that it will be useful, but WITHOUT
## ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
## FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
## details.
##
## You should have received a copy of the GNU General Public License along with
## this program; if not, see <http://www.gnu.org/licenses/>.

## -*- texinfo -*-
## @deftypefn  {Function File} {} padarray (@var{A}, @var{padsize})
## @deftypefnx {Function File} {} padarray (@dots{}, @var{padval})
## @deftypefnx {Function File} {} padarray (@dots{}, @var{pattern})
## @deftypefnx {Function File} {} padarray (@dots{}, @var{direction})
## Pad array or matrix.
##
## Adds padding of length @var{padsize}, to a numeric matrix @var{A}.
## @var{padsize} must be a vector of non-negative values, each of them
## defining the length of padding to its corresponding dimension.  For
## example, if @var{padsize} is [4 5], it adds 4 rows (1st dimension)
## and 5 columns (2nd dimension), to both the start and end of @var{A}.
##
## If there's less values in @var{padsize} than number of dimensions in @var{A},
## they're assumed to be zero.  Singleton dimensions of @var{A} are also
## padded accordingly (except when @var{pattern} is @qcode{"reflect"}).
##
## The values used in the padding can either be a scalar value @var{padval}, or
## the name of a specific @var{pattern}.  Available patterns are:
##
## @table @samp
## @item "zeros" (default)
## Pads with the value 0 (same as passing a @var{padval} of 0).  This is the
## default.
## @item "circular"
## Pads with a circular repetition of elements in @var{A} (similar to
## tiling @var{A}).
## @item "replicate"
## Pads replicating the values at the border of @var{A}.
## @item "symmetric"
## Pads with a mirror reflection of @var{A}.
## @item "reflect"
## Same as "symmetric", but the borders are not used in the padding.  Because
## of this, it is not possible to pad singleton dimensions.
## @end table
##
## By default, padding is done in both directions.  To change this,
## @var{direction} can be one of the following values:
##
## @table @samp
## @item "both" (default)
## Pad each dimension before the first element of @var{A} the number
## of elements defined by @var{padsize}, and the same number again after
## the last element. This is the default.
## @item "pre"
## Pad each dimension before the first element of @var{A} the number of
## elements defined by @var{padsize}.
## @item "post"
## Pad each dimension after the last element of @var{A} the number of
## elements defined by @var{padsize}.
## @end table
##
## @seealso{cat, flipdim, resize, prepad, postpad}
## @end deftypefn

                function B = padarray(A, padsize, varargin)

                if (nargin < 2 || nargin > 4)
                    print_usage ();
                elseif (! isvector (padsize) || ! isnumeric (padsize) || any (padsize < 0) ||
                        any (padsize != fix (padsize)))
                    error ("padarray: PADSIZE must be a vector of non-negative integers");
                endif

                ## Assure padsize is a row vector
                padsize = padsize(:).';

                if (! any (padsize))
                    ## Nothing to do here
                    B = A;
                    return
                endif

                ## Default values
                padval    = 0;
                pattern   = "";
                direction = "both";

                ## There won't be more than 2 elements in varargin
                ## We have to support setting the padval (shape) and direction in any
                ## order. Both examples must work:
                ##  padarray (A, padsize, "circular", "pre")
                ##  padarray (A, padsize, "pre", "circular")
                for opt = 1:numel(varargin)
                    val = varargin{opt};
                    if (ischar (val))
                    if (any (strcmpi (val, {"pre", "post", "both"})))
                        direction = val;
                    elseif (any (strcmpi (val, {"circular", "replicate", "reflect", "symmetric"})))
                        pattern = val;
                    elseif (strcmpi (val, "zeros"))
                        padval = 0;
                    else
                        error ("padarray: unrecognized string option `%s'", val);
                    endif
                    elseif (isscalar (val))
                    padval = val;
                    else
                    error ("padarray: PADVAL and DIRECTION must be a string or a scalar");
                    endif
                endfor

                fancy_pad = false;
                if (! isempty (pattern))
                    fancy_pad = true;
                endif

                ## Check direction
                pre  = any (strcmpi (direction, {"pre", "both"}));
                post = any (strcmpi (direction, {"post", "both"}));

                ## Create output matrix
                B_ndims = max ([numel(padsize) ndims(A)]);
                A_size  = size (A);
                P_size  = padsize;
                A_size(end+1:B_ndims) = 1;  # add singleton dimensions
                P_size(end+1:B_ndims) = 0;  # assume zero for missing dimensions

                pre_pad_size = P_size * pre;
                B_size = A_size + pre_pad_size + (P_size * post);

                ## insert input matrix into output matrix
                A_idx = cell (B_ndims, 1);
                for dim = 1:B_ndims
                    A_idx{dim} = (pre_pad_size(dim) +1):(pre_pad_size(dim) + A_size(dim));
                endfor
                if (post && ! pre && (padval == 0 || fancy_pad))
                    ## optimization for post padding only with zeros
                    B = resize (A, B_size);
                else
                    B = repmat (cast (padval, class (A)), B_size);
                    B(A_idx{:}) = A;
                endif

                if (fancy_pad)
                    ## Init a template "index all" cell array
                    template_idx = repmat ({":"}, [B_ndims 1]);

                    circular = replicate = symmetric = reflect = false;
                    switch (tolower (pattern))
                    case "circular",  circular  = true;
                    case "replicate", replicate = true;
                    case "symmetric", symmetric = true;
                    case "reflect",   reflect   = true;
                    otherwise
                        error ("padarray: unknown PADVAL `%s'.", pattern);
                    endswitch

                    ## For a dimension of the input matrix of size 1, since reflect does
                    ## not includes the borders, it is not possible to pad singleton dimensions.
                    if (reflect && any ((! (A_size -1)) & P_size))
                    error ("padarray: can't add %s padding to sinleton dimensions", pattern);
                    endif

                    ## For symmetric and reflect:
                    ##
                    ## The idea is to split the padding into 3 different cases:
                    ##    bits
                    ##        Parts of the input matrix that are used for the padding.
                    ##        In most user cases, there will be only this padding,
                    ##        complete will be zero, and so bits will be equal to padsize.
                    ##    complete
                    ##        Number of full copies of the input matrix are used for
                    ##        the padding (for reflect, "full" size is actually minus 1).
                    ##        This is divided into pair and unpaired complete. In most
                    ##        cases, this will be zero.
                    ##    pair complete
                    ##        Number of pairs of complete copies.
                    ##    unpaired complete
                    ##        This is either 1 or 0. If 1, then the complete copy closer
                    ##        to the output borders has already been flipped so that if
                    ##        there's bits used to pad as well, they don't need to be flipped.
                    ##
                    ## Reasoning pair and unpaired complete: when the pad is much larger
                    ## than the input matrix, we must pay we must pay special attention to
                    ## symmetric and reflect. In a normal case (the padding is smaller than
                    ## the input), we just use the flipped matrix to pad and we're done.
                    ## In other cases, if the input matrix is used multiple times on the
                    ## pad, every other copy of it must NOT be flipped (the padding must be
                    ## symmetric itself) or the padding will be circular.

                    if (reflect)
                    A_cut_size          = A_size -1;
                    complete            = floor (P_size ./ A_cut_size);
                    bits                = rem (P_size, A_cut_size);
                    pair_size           = A_cut_size * 2;
                    pair_complete       = floor (complete / 2);
                    unpaired_complete   = mod (complete, 2);
                    else
                    complete            = floor (P_size ./ A_size);
                    bits                = rem (P_size, A_size);
                    if (circular)
                        complete_size     = complete .* A_size;
                    elseif (symmetric)
                        pair_complete     = floor (complete / 2);
                        pair_size         = A_size * 2;
                        unpaired_complete = mod (complete, 2);
                    endif
                    endif

                    dim = 0;
                    for s = padsize
                    dim++;
                    if (s == 0)
                        ## skip this dimension if no padding requested
                        continue
                    endif

                    if (circular)
                        dim_idx     = template_idx;
                        source_idx  = template_idx;
                        A_idx_end   = A_idx{dim}(end);
                        A_idx_ini   = A_idx{dim}(1);

                        if (complete(dim))
                        dim_pad_size(1:B_ndims) = 1;
                        dim_pad_size(dim)       = complete(dim)*pre + complete(dim)*post;
                        dim_idx{dim}            = [];
                        if (pre)
                            dim_idx{dim}  = [(bits(dim) +1):(complete_size(dim) + bits(dim))];
                        endif
                        if (post)
                            dim_idx{dim}  = [dim_idx{dim} (A_idx_end +1):(A_idx_end + complete_size(dim))];
                        endif
                        source_idx{dim} = A_idx{dim};
                        B(dim_idx{:})   = repmat (B(source_idx{:}), dim_pad_size);
                        endif

                        if (pre)
                        if (bits(dim))
                            dim_idx{dim}    = 1:bits(dim);
                            source_idx{dim} = (A_idx_end - bits(dim) +1):A_idx_end;
                            B(dim_idx{:})   = B(source_idx{:});
                        endif
                        endif
                        if (post)
                        if (bits(dim))
                            dim_idx{dim}    = (B_size(dim) -bits(dim) +1):B_size(dim);
                            source_idx{dim} = A_idx_ini:(A_idx_ini + bits(dim) -1);
                            B(dim_idx{:})   = B(source_idx{:});
                        endif
                        endif

                    elseif (replicate)
                        dim_pad_size(1:B_ndims) = 1;
                        dim_pad_size(dim)       = P_size(dim);
                        dim_idx                 = template_idx;
                        source_idx              = template_idx;
                        if (pre)
                        dim_idx{dim}          = 1:P_size(dim);
                        source_idx{dim}       = P_size(dim) +1;
                        B(dim_idx{:})         = repmat (B(source_idx{:}), dim_pad_size);
                        endif
                        if (post)
                        dim_idx{dim}          = (A_idx{dim}(end) +1):B_size(dim);
                        source_idx{dim}       = A_idx{dim}(end);
                        B(dim_idx{:})         = repmat (B(source_idx{:}), dim_pad_size);
                        endif

                    ## The idea behind symmetric and reflect passing is the same so the
                    ## following cases have similar looking code. However, there's small
                    ## adjustements everywhere that makes it really hard to merge as a
                    ## common case.
                    elseif (symmetric)
                        dim_idx     = template_idx;
                        source_idx  = template_idx;
                        A_idx_ini   = A_idx{dim}(1);
                        A_idx_end   = A_idx{dim}(end);

                        if (pre)
                        if (bits(dim))
                            dim_idx{dim}      = 1:bits(dim);
                            if (unpaired_complete(dim))
                            source_idx{dim} = (A_idx_end - bits(dim) +1):A_idx_end;
                            B(dim_idx{:})   = B(source_idx{:});
                            else
                            source_idx{dim} = A_idx_ini:(A_idx_ini + bits(dim) -1);
                            B(dim_idx{:})   = flipdim (B(source_idx{:}), dim);
                            endif
                        endif
                        endif
                        if (post)
                        if (bits(dim))
                            dim_idx{dim}      = (B_size(dim) - bits(dim) +1):B_size(dim);
                            if (unpaired_complete(dim))
                            source_idx{dim} = A_idx_ini:(A_idx_ini + bits(dim) -1);
                            B(dim_idx{:})   = B(source_idx{:});
                            else
                            source_idx{dim} = (A_idx_end - bits(dim) +1):A_idx_end;
                            B(dim_idx{:})   = flipdim (B(source_idx{:}), dim);
                            endif
                        endif
                        endif

                        if (complete(dim))
                        dim_pad_size(1:B_ndims) = 1;
                        source_idx{dim}         = A_idx{dim};
                        flipped_source          = flipdim (B(source_idx{:}), dim);
                        endif

                        if (pair_complete(dim))
                        dim_pad_size(dim) = pair_complete(dim);
                        dim_idx{dim}      = [];
                        if (pre)
                            dim_idx{dim}    = [(1 + bits(dim) + (A_size(dim)*unpaired_complete(dim))):(A_idx_ini -1)];
                            B(dim_idx{:})   = repmat (cat (dim, B(source_idx{:}), flipped_source), dim_pad_size);
                        endif
                        if (post)
                            dim_idx{dim}    = [(A_idx_end +1):(A_idx_end + (pair_size(dim) * pair_complete(dim)))];
                            B(dim_idx{:})   = repmat (cat (dim, flipped_source, B(source_idx{:})), dim_pad_size);
                        endif
                        endif

                        if (unpaired_complete(dim))
                        source_idx = template_idx;
                        if (pre)
                            dim_idx{dim}  = (1 + bits(dim)):(bits(dim) + A_size(dim));
                            B(dim_idx{:}) = flipped_source(source_idx{:});
                        endif
                        if (post)
                            dim_idx{dim}  = (B_size(dim) - bits(dim) - A_size(dim) +1):(B_size(dim) - bits(dim));
                            B(dim_idx{:}) = flipped_source(source_idx{:});
                        endif
                        endif

                    elseif (reflect)
                        dim_idx     = template_idx;
                        source_idx  = template_idx;
                        A_idx_ini   = A_idx{dim}(1);
                        A_idx_end   = A_idx{dim}(end);

                        if (pre)
                        if (bits(dim))
                            dim_idx{dim}      = 1:bits(dim);
                            if (unpaired_complete(dim))
                            source_idx{dim} = (A_idx_end - bits(dim)):(A_idx_end -1);
                            B(dim_idx{:})   = B(source_idx{:});
                            else
                            source_idx{dim} = (A_idx_ini +1):(A_idx_ini + bits(dim));
                            B(dim_idx{:})   = flipdim (B(source_idx{:}), dim);
                            endif
                        endif
                        endif
                        if (post)
                        if (bits(dim))
                            dim_idx{dim}      = (B_size(dim) - bits(dim) +1):B_size(dim);
                            if (unpaired_complete(dim))
                            source_idx{dim} = (A_idx_ini +1):(A_idx_ini + bits(dim));
                            B(dim_idx{:})   = B(source_idx{:});
                            else
                            source_idx{dim} = (A_idx_end - bits(dim)):(A_idx_end -1);
                            B(dim_idx{:})   = flipdim (B(source_idx{:}), dim);
                            endif
                        endif
                        endif

                        if (complete(dim))
                        dim_pad_size(1:B_ndims) = 1;
                        source_idx{dim}         = A_idx{dim};
                        flipped_source          = flipdim (B(source_idx{:}), dim);
                        endif

                        if (pair_complete(dim))
                        dim_pad_size(dim) = pair_complete(dim);
                        dim_idx{dim}      = [];
                        if (pre)
                            flipped_source_idx = source_idx;
                            flipped_source_idx{dim} = 1:A_cut_size(dim);
                            source_idx{dim} = A_idx_ini:(A_idx_end -1);
                            dim_idx{dim}    = [(1 + bits(dim) + (A_cut_size(dim)*unpaired_complete(dim))):(A_idx_ini -1)];
                            B(dim_idx{:})   = repmat (cat (dim, B(source_idx{:}), flipped_source(flipped_source_idx{:})), dim_pad_size);
                        endif
                        if (post)
                            flipped_source_idx = source_idx;
                            flipped_source_idx{dim} = 2:A_size(dim);
                            source_idx{dim} = (A_idx_ini +1):A_idx_end;
                            dim_idx{dim}    = [(A_idx_end +1):(A_idx_end + (pair_size(dim) * pair_complete(dim)))];
                            B(dim_idx{:})   = repmat (cat (dim, flipped_source(flipped_source_idx{:}), B(source_idx{:})), dim_pad_size);
                        endif
                        endif

                        if (unpaired_complete(dim))
                        source_idx = template_idx;
                        if (pre)
                            source_idx{dim} = 1:(A_size(dim)-1);
                            dim_idx{dim}    = (1 + bits(dim)):(bits(dim) + A_size(dim) -1);
                            B(dim_idx{:})   = flipped_source(source_idx{:});
                        endif
                        if (post)
                            source_idx{dim} = 2:A_size(dim);
                            dim_idx{dim}    = (B_size(dim) - bits(dim) - A_size(dim) +2):(B_size(dim) - bits(dim));
                            B(dim_idx{:})   = flipped_source(source_idx{:});
                        endif
                        endif

                    endif
                    endfor
                endif
                endfunction
img =imread("/home/andesil/PSB/nice_example/random.png");
img =imread("/home/andesil/PSB/nice_example/increasing.png");
% img =imread("/home/andesil/PSB/nice_example/square.png");
% img =imread("/home/andesil/PSB/nice_example/square1border.png");
% img =imread("/home/andesil/PSB/nice_example/circle.png");
% img =imread("/home/andesil/PSB/nice_example/circleNoborder.png");
img = imread("/home/andesil/PSB/large_media/100000G.jpg");
divImg = conv2(img, [0,1,0;1,-4,1;0,1,0], "full" );
% divImg = conv2(img, [0,1,0;1,-4,1;0,1,0], "same");
% divImg = conv2(img, [0,1,0;1,-4,1;0,1,0], "valid");
% img
% divImg

w = [0.06110 0.26177 0.53034 0.65934  0.51106 0.05407 0.24453 0.57410];

h1 = w(1:4); % first four elements
h1 = [h1 h1(end-1:-1:1)]; % reverse and beginning
h1 = h1' * h1; % pow(2)

h2 = h1*w(5); % h1 multiplied by center kernel value

g = w(6:8); % last elements
g = [g g(end-1:-1:1)]; % reverse and beginning
g = g' * g;  % pow(@)


function pyconv( a, h1, h2, g )
    [h,w] = size(a); % h = rowSize, w = colSize, aka width/height
    maxLevel = ceil(log2(max(h,w))); %
    maxLevel = maxLevel  - 5
    fs = size(h1,1); % size of first dim of h1

    % Forward transform (analysis)
    pyr{1} = padarray(a, [fs fs]); %  pad with "fs" zeroes in each direction
    for i=2:maxLevel % for each element
    
        down = imfilter(pyr{i-1},h1, 0); % filter previous pyramid with h1, put in down
        down = down(1:2:end,1:2:end); % extract element 1 + 2n, repeat them down
                                    % in 2d
    
        down = padarray(down, [fs fs]); % pad the array ( again )
        pyr{i} = down; % set current level in pyramid to down
    end
    
    
    % Backward transform (synthesis)
    % similar to loop aboves, upsamling..
    fpyr{maxLevel} = imfilter(pyr{maxLevel},g, 0);
    for i=maxLevel-1:-1:1
    
        rd = fpyr{i+1}; % fetch from prev pyramid
        rd = rd(1+fs:end-fs, 1+fs:end-fs); % fetch the kernel
    
        up = zeros(size(pyr{i})); % zero array
        up(1:2:end,1:2:end) = rd; % pad every 1 + 2n elements with 0, interpolation step
    
        fpyr{i} = imfilter(up,h2,0) + imfilter(pyr{i}, g, 0); % apply mask, boundary=0
    end
    
    ahat = fpyr{1};
    ahat = ahat(1+fs:end-fs, 1+fs:end-fs);

    % ahat = round(ahat);
    % ahat(ahat>255) = 255;
    % ahat(ahat<0) = 0;
    imwrite(ahat, "convy.out.png");

endfunction

pyconv( -divImg, h1, h2, g )
ctime(time())
