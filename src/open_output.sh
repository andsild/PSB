#!/bin/bash
for line in $(find output/ -maxdepth 1 -type f | sort | awk 'ORS=NR%3?",":"\n"')
do
    meh=$(echo ${line} | sed 's/,/ /g')
    vim -o ${meh}
done
