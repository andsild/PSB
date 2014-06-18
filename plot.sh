#!/bin/bash

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

cmd="plot"
for file in $(find "${DIR}/data/" -type f -name "*average.dat")
do
    cmd="${cmd} \"${file}\" using 1 title \"test${file}\" with linespoints ,"
done

# cmd="${cmd%%,};"

for file in $(find "${DIR}/data/" -regextype sed -regex ".*/[0-9].*\.dat")
do
    cmd="${cmd} \"${file}\" using 1 with points pt 7 lc rgb \"\#FFB801\" notitle,"
    # break;
done

cmd="${cmd%%,}; set nomultiplot"

gnuplot -e "set terminal png; set output \"out.png\" ; ${cmd};"
#echo "set terminal png; set output \"out.png\" ; ${cmd};"
