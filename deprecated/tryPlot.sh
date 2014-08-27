#!/bin/bash

#!/bin/bash
#: Title        : draw_graph
#: Date         : Jul 2014
#: Author       : Anders Sildnes
#: Version      : 2.0 (1.0 used column data)
#: Desctiption  : Convert output from PSB to a format for gnuplot
#: Options      : $1 is the folder with data

if [ ! -d "${1}" ]
then
    printf "Usage: %s <folder to render >\n" "${0##*/}"
    exit 1
fi

filetype="png"
newPlotFile="${2:-"/tmp/out.${filetype}"}"
joinedOutFile="${3:-"/tmp/joined.${filetype}"}"

outwidth=700
outheight=800
xRange=100000
yRange=10

margins="
        set lmargin at screen 0.15;
        set rmargin at screen 1;
        set tmargin at screen 1;
        set bmargin at screen 0.15;
"
setRange="
    set link x; set link y;
    set xrange [1:${xRange}] ;
    set yrange [0.1:${yRange}] ;
    set logscale x;
"
setTerm="
    reset ;
    set terminal ${filetype} \
        size ${outwidth},${outheight} ;
    "
unsetRatios="
    unset border;
    unset tics;
    unset xlabel; unset ylabel;
    "
       
function doOverlay()
{
    # set key right inside title \"Lines:\";
    gnuplot -e "${setTerm}
        set output \"${3}\";
        set datafile missing \"X\" ;
        set multiplot;

        ${unsetRatios}
        set lmargin at screen 0.0;
        set rmargin at screen 1.0;
        set tmargin at screen 1.0;
        set bmargin at screen 0.0;
        plot \"${2}\" binary filetype=${filetype} w rgbimage notitle ;

        set key; set tics; set border;
        ${margins}
        ${setRange}
        plot \"${1}\" matrix with lines notitle ;

        unset multiplot ;
        " 2>&1 | sed '/.*arial.*/d ; /.*undefined.*/d'|| (echo ${1} ; exit 1)
}
function doSinglePlot()
{
    gnuplot -e "${setTerm}
        set output \"${2}\" ;
        set datafile missing \"X\" ;
        set origin 0.0, 0.0;
        ${unsetRatios}

        ${margins}
        ${setRange}
        plot \"${1}\" matrix with lines
        " 2>&1 | sed '/.*arial.*/d ; /.*undefined.*/d'|| (echo ${1} ; exit 1)
        # line inc : block inc : start line (0 index)
}


function doMidPlot()
{
    gnuplot -e "${setTerm}
        set output \"${tmpDraw}\";
        set datafile missing \"X\" ;
        set multiplot;
        set origin 0.00,0.0 ;

        unset tics;
        

        ${unsetRatios}
        set lmargin at screen 0.0;
        set rmargin at screen 1.0;
        set tmargin at screen 1.0;
        set bmargin at screen 0.0;
        plot \"${2}\" binary filetype=${filetype} with rgbimage ;

        ${margins}
        ${setRange}
        plot \"${1}\" matrix with lines ;

        unset multiplot ;
        " 2>&1 | sed '/.*arial.*/d ; /.*undefined.*/d'
    cp ${tmpDraw} ${2}
}
tmpDraw="/tmp/test.png"
[ -f ${joinedOutFile} ] && rm ${joinedOutFile}
[ -f ${newPlotFile} ]   && rm ${newPlotFile}
[ -f ${tmpDraw} ]       && rm ${tmpDraw}
for file in $(find ${1} -type f | head --lines=1)
do
    # doSinglePlot ${file} ${newPlotFile}
    doSinglePlot "./gauss__projectsNaturalnessdata100400jpg_8446_51.234816.dat" ${joinedOutFile} ${tmpDraw}
    break;
done
cp -vi ${newPlotFile} ${joinedOutFile}
# setsid sxiv ${newPlotFile} & 2>/dev/null

for file in $(find ${1} -type f | tail --lines=+2 | head --lines=-1)
# for file in $(seq 1 1000 1)
do
    doMidPlot ${file} ${joinedOutFile} ${tmpDraw}
    doMidPlot "./gauss__projectsNaturalnessdata100400jpg_8446_51.234816.dat" ${joinedOutFile} ${tmpDraw}
    # setsid sxiv ${joinedOutFile} &
done

for file in $(find ${1} -type f | tail --lines=1)
do
    # doOverlay ${file} ${joinedOutFile} ${tmpDraw}
    doOverlay "./gauss__projectsNaturalnessdata100400jpg_8446_51.234816.dat"  ${joinedOutFile} ${tmpDraw}
   cp -v ${tmpDraw} ${joinedOutFile}
done

setsid sxiv ${tmpDraw} & 2>/dev/null



# EOF
