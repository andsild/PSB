#!/bin/bash

IFS=$'\n'
max=0
outfile="/tmp/outgraph.png"
missChar="X"
while read -r input
do
    x=$(printf "%s" ${input} | awk ' { print NF }')
    if [ ${x} -gt ${max} ]
    then
        max=${x}
    fi
done
cat /dev/stdin | awk -v max=${max} '
    {
        for (i = NF+1; i < max+1; i++)
        {
            $i = "X";
        }
        print
    }' \
    | gnuplot -e "set terminal png; set output \"${outfile}\"; \
            set datafile missing \"${missChar}\" ; \
            plot \"-\" matrix with line ;
           "
setsid sxiv ${outfile} &
