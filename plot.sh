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

NAME_FILE_AVERAGE="average.dat"


for solver in "${solvers[@]}"
do
    for file in $(find "${DIR}/build/output/${solver}" -type f -name "*${NAME_FILE_AVERAGE}")
    do
        cmd="${cmd} \"${file}\" using 1 title \"${solver}\" lc rgb \"#${colors["${solver}"]}\"  with lines   ,"
    done
done

# cmd="${cmd%%,}"

for solver in "${solvers[@]}"
do
    for file in $(find "${DIR}/build/output/${solver}" -type f ! -name "*${NAME_FILE_AVERAGE}" -and ! -name "*.png")
    do
        echo ${file}
        cmd="${cmd} \"${file}\" using 1 with points pointtype ${POINT_TYPE} pointsize ${POINT_SIZE} linecolor rgb \"#${colors["${solver}"]}\" notitle,"
    done
done

cmd="${cmd%%,}"

LOGSCALE_AXIS="set xlabel \"iterations\" ; set ylabel \"(logscale) error in central difference\" ;
     set logscale y ;"
LABEL_AXIS="set xlabel \"iterations\" ; set ylabel \"error in central difference\" ;"

gnuplot -e \
    "set terminal png; set output \"out.png\" ; ${LOGSCALE_AXIS} ;
     ${cmd};"
#echo "set terminal png; set output \"out.png\" ; ${cmd};"
