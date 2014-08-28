#!/bin/bash

python tryplot.py plot2d ../../useData/sor/ dawdwa ../../useData/jacobi aw  \
../../useData/gauss daw ../../useData/dct/ daw ../../useData/dst/ \
../../useData/wavelet5/ ../../useData/wavelet7/ composite \
plot2DIterativeALL.png  plot2ddirectALL.png res.png

composite res.png canvas.png newres.png

# EOF
