#!/bin/bash

rm /tmp/test.png
gnuplot -e " \
    set terminal png; set output \"/tmp/test.png\" ; \
    set datafile missing \"X\" ; \
    plot \"<(sed -n 2~2p mytext.dat)\" matrix with line,  \
        \"<(sed -n 2~2p mytext2.dat)\" matrix with line; \
    "

[ -f "/tmp/test.png" ] && setsid sxiv /tmp/test.png &
