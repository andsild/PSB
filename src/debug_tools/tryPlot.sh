#!/bin/bash

outwidth=640
outheight=480
xRange=3
yRange=20

margins="
    set lmargin at screen 0.1;
    set rmargin at screen 0.9;
    set bmargin at screen 0.15;
    set tmargin at screen 0.85;
"
setRange="
    set yrange [0:${yRange}] ;
    set xrange [0:${xRange}] ;
"
gnuplot -e "set terminal png size ${outwidth},${outheight};
    set output \"/tmp/out.png\" ;
    set datafile missing \"X\" ;
    unset key; unset tics; unset border;

    ${margins}
    ${setRange}
    plot \"<(sed -n 2~2p mytext1.dat)\" matrix with lines
            "

if [ -f "/tmp/out.png" ]
then
gnuplot -e "set terminal png size ${outwidth},${outheight};
    set output \"/tmp/joined.png\";
    set datafile missing \"X\" ;
    set key right inside title \"Lines:\";
    set multiplot;
   
    unset key; unset tics; unset border;
    set lmargin at screen 0.0;
    set rmargin at screen 1.0;
    set bmargin at screen 0.0;
    set tmargin at screen 1.0;
    plot \"/tmp/out.png\" binary filetype=png w rgbimage ;
   
    set key; set tics; set border; 
    ${margins}
    ${setRange}
    plot \"<(sed -n 2~2p mytext2.dat)\" matrix with lines ;
   
    unset multiplot ;
    "  \
            && setsid sxiv /tmp/joined.png &
else
    echo \"error!\"
    setsid sxiv /tmp/out.png &
fi

