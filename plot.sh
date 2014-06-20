#!/bin/bash

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

declare -A solvers
declare -A colors
solvers[0]="gauss"
    colors["${solvers[0]}"]="FFB801"
solvers[1]="jacobi"
    colors["${solvers[1]}"]="15A600"
solvers[2]="sor"
    colors["${solvers[2]}"]="5141FF"

POINT_TYPE="1" #http://stelweb.asu.cas.cz/~nemeth/work/stuff/gnuplot/gnuplot-line-and-point-types-bw.png
POINT_SIZE="2"


cmd="plot"


for solver in "${solvers[@]}"
do
    for file in $(find "${DIR}/build/output/${solver}" -type f -name "*average.dat")
    do
        cmd="${cmd} \"${file}\" using 1 title \"${solver}\" lc rgb \"#${colors["${solver}"]}\"  with lines   ,"
    done
done

# cmd="${cmd%%,}"

for solver in "${solvers[@]}"
do
    for file in $(find "${DIR}/build/output/${solver}" -regextype sed -regex ".*/[0-9].*\.dat")
    do
        cmd="${cmd} \"${file}\" using 1 with points pointtype ${POINT_TYPE} pointsize ${POINT_SIZE} lc rgb \"#${colors["${solver}"]}\" notitle,"
    done
done

cmd="${cmd%%,}"

gnuplot -e \
    "set terminal png; set output \"out.png\" ;
     set xlabel \"iterations\" ; set ylabel \"error in central difference\" ;
     set logscale y ;
     ${cmd};"
#echo "set terminal png; set output \"out.png\" ; ${cmd};"
