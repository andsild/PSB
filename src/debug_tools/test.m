 img =imread("/home/andesil/PSB/nice_example/all_increasing.png")
     
     w = [0.06110 0.26177 0.53034 0.65934  0.51106 0.05407 0.24453 0.57410];

        h1 = w(1:4); % first four elements
        h1 = [h1 h1(end-1:-1:1)]; % reverse and beginning
        h1 = h1' * h1; % pow(2)

        h2 = h1*w(5); % h1 multiplied by center kernel value

        g = w(6:8); % last elements
        g = [g g(end-1:-1:1)]; % reverse and beginning
        g = g' * g;  % pow(@)
        divImg = -divergence(img)
        result = pyconv( divImg, h1, h2, g ); % convolution by L2 norm


function ahat = pyconv( a, h1, h2, g )

[h,w] = size(a); % h = rowSize, w = colSize, aka width/height
maxLevel = ceil(log2(max(h,w))); %
fs = size(h1,1); % size of first dim of h1

% Forward transform (analysis)
% pyr = pyrmaid
pyr{1} = padarray(a, [fs fs]); %  pad with "fs" zeroes in each direction
for i=2:maxLevel % for each element

    down = conv2(pyr{i-1},h1,0); % filter previous pyramid with h1, put in down
    down = down(1:2:end,1:2:end); % extract element 1 + 2n, repeat them down
                                  % in 2d

    down = padarray(down, [fs fs]); % pad the array ( again )
    pyr{i} = down; % set current level in pyramid to down

end

% Backward transform (synthesis)
% similar to loop aboves, upsamling..
fpyr{maxLevel} = conv2(pyr{maxLevel},g,0);
for i=maxLevel-1:-1:1

    rd = fpyr{i+1}; % fetch from prev pyramid
    rd = rd(1+fs:end-fs, 1+fs:end-fs); % fetch the kernel

    up = zeros(size(pyr{i})); % zero array
    up(1:2:end,1:2:end) = rd; % pad every 1 + 2n elements with 0, interpolation step

    fpyr{i} = conv2(up,h2,0) + conv2(pyr{i},g,0); % apply mask, boundary=0
end

ahat = fpyr{1};
ahat = ahat(1+fs:end-fs, 1+fs:end-fs);