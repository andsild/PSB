#!/bin/bash
monster=""
for line in $(find log/ -maxdepth 1 -type f | sort | awk 'ORS=NR%3?",":"\n"')
do
    echo ${line}
    meh=$(echo ${line} | sed 's/,/ /g ; s/log\/execution.log//g')
    monster="${monster} ${meh}"
    # vim -p ${meh}
done
vim -p ${monster}
