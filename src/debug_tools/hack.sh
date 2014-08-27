#!/bin/bash
#: Title        : draw_graph
#: Date         : Jul 2014
#: Author       : Anders Sildnes
#: Version      : 1.0
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

numSolvers="$(grep --color=none -oE '.*__' ${DATA_FILE} | uniq | wc -l)"

declare -a arrayvar solvers
for sol in $(grep --color=none -oE '.*__' ${DATA_FILE} | uniq)
do
    solvers+=("${sol%%__*}")
done


declare -A colors
colors["gauss"]="FFB801"
colors["jacobi"]="15A600"
colors["sor"]="5141FF"

POINT_TYPE="1" #http://stelweb.asu.cas.cz/~nemeth/work/stuff/gnuplot/gnuplot-line-and-point-types-bw.png
POINT_SIZE="2"


TMP_OUT="/tmp/abcdefg"
TMP_FILTERED_OUT="/tmp/abcdefga"
awk '{ 
    if (NR % 2 != 0) 
        { keep=$0;
          gsub(/__.*+$/, " ", keep);
          next
        }
    if(NR%2 == 0) 
        {
            print keep, $0
        }
    } 
        ' ${DATA_FILE} > ${TMP_OUT}

awk '
{ 
    for (i=1; i<=NF; i++)  {
        a[NR,i] = $i
    }
}
NF>p { p = NF }
END {    
    for(j=1; j<=p; j++) {
        str=a[1,j]
        for(i=2; i<=NR; i++){
            if(a[i,j]=="") { a[i,j] = "X";}
            str=str"\t"a[i,j];
        }
        print str
    }
}' ${TMP_OUT} | sed 's/^\t/X\t/g' > ${TMP_FILTERED_OUT}

numCols="$(awk '{print NF}' ${TMP_FILTERED_OUT} | sort | tail -n1)"
solverCols=$(( $numSolvers / $numCols ))
iterInc=$(( $numCols / ${numSolvers}))
iterEnd=$(( ${numCols} ))
printf "%-30s: %s\n%-30s: %s\n%-30s: %s\n" \
       "Number of solvers" "${numSolvers}"  \
       "Number of columns (solutions)" "${numCols}"  \
       "Number of iterations" "$(( $iterEnd / $iterInc ))" 

solverIter=0
# seq: start inc end
for iPos in $(seq 1 ${iterInc} ${iterEnd})
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
cmdParse=$(echo ${cmdParse} | sed -e 's/,$//g ; s/,/&\n/g' )
cmdParse="plot ${cmdParse} ;"

cmd="set terminal png; set output 'test.png';
    set key outside;
    set ylabel \"Mean Square difference from input and solution\";
    set xlabel \"(logscale) Iterations\";
    set logscale x;
    ${cmdParse}
    "
#     # plot for [col=1:${numCols}] '${TMP_OUT}a' using 0:col with lines"
# echo ${cmd}
gnuplot -e "${cmd}" 2>&1 | sed '/.*arial.*/d'
# cat ${TMP_FILTERED_OUT}

# EOF
