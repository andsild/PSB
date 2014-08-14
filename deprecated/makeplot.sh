#!/bin/bash
#: Title        : draw_graph
#: Date         : Jul 2014
#: Author       : Anders Sildnes
#: Version      : 2.0 (1.0 used column data)
#: Desctiption  : Convert output from PSB to a format for gnuplot
#: Options      : $1 should be a file that the info is dumped into 
#:                (default to terminal)

if [ -z "${1}" ]
then
    printf  "usage: %s < inputfile >\n" "${0}"
    exit 1;
fi

DATA_FILE="${1}"
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

solverListFile="/tmp/solvers"
grep --color=none -oE '.*__' ${DATA_FILE} | uniq | sed 's/__//g' > ${solverListFile}
numSolvers=$(wc -l ${solverListFile})

declare -a arrayvar solvers
declare -a arrayvar solverNums
for sol in $(cat ${solverListFile})
do
    solvers+=("${sol}")
    linePos=$(grep --line-number --max-count=1 ${sol} ${DATA_FILE})
    solverNums+=(${linePos%%:*})
done
solverNums+=$(wc -l ${DATA_FILE})

declare -A colors
colors["gauss"]="FFB801"
colors["jacobi"]="15A600"
colors["sor"]="5141FF"

POINT_TYPE="1" #http://stelweb.asu.cas.cz/~nemeth/work/stuff/gnuplot/gnuplot-line-and-point-types-bw.png
POINT_SIZE="2"

function readFile()
{
    solverName=$(head -n1 ${1})
    solverName=${solverName%%_*}

}

IFS=$'\n'
max=0
for line in $(sed -n 2~2p ${DATA_FILE})
do
    x=$(printf "%s" ${line} | awk ' { print NF } ')
    if [[ ${x} -gt ${max} ]]
    then
        max=${x}
    fi
done
echo ${max}

awk -v max=${max} '
{
    if(NR%2==0)
    {
        for (i = NF+1; i < max+1; i++)
        {
            $i = "X";
        }
        print
    }
    if(NR%2==1)
    {
        print;
    }
}' ${DATA_FILE}

printf "%-30s: %s\n%-30s: %s\n%-30s: %s\n" \
       "Number of solvers" "${numSolvers}"  \

for iPos in ${solverNums[@]}
do
    range=$(( $iPos + ${iterInc} ))
    curSolver="${solvers[$solverIter]}"
    solverIter=$(( $solverIter + 1 ))
    cmdParse+=" for [col=${iPos}:${range}] '${TMP_OUT}a' using 0:col  \
                notitle  \
                with lines \
                lc rgb \"#${colors[${curSolver}]}\"
                 ,"

    # cmdParse+=" for [col=${iPos}:${range}] '${TMP_OUT}a' using 0:col  \
    #             title \"${curSolver}\" \
    #             with points pointtype ${POINT_TYPE} pointsize ${POINT_SIZE} \
    #             lc rgb \"#${colors[${curSolver}]}\"
    #              ,"
done
# cmdParse=$(echo ${cmdParse} | sed -e 's/,$//g ; s/,/&\n/g' )
# cmdParse="plot ${cmdParse} ;"
#
# cmd="set terminal png; set output 'test.png';
#     set key outside;
#     set ylabel \"Mean Square difference from input and solution\";
#     set xlabel \"(logscale) Iterations\";
#     set logscale x;
#     ${cmdParse}
#     "
# #     # plot for [col=1:${numCols}] '${TMP_OUT}a' using 0:col with lines"
# # echo ${cmd}
# gnuplot -e "${cmd}" 2>&1 | sed '/.*arial.*/d'
# # cat ${TMP_FILTERED_OUT}
#
# # EOF
