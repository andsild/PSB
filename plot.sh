#!/bin/bash

cmd="plot"
for file in $(find "./data/" -type f -name "*average.dat")
do
    cmd="${cmd} \"${file}\" using 1 title \"test${file}\" ,"
done

echo ${cmd%%,}
gnuplot -e "set terminal png; set output \"out.png\" ; ${cmd%%,};"
