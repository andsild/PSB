#!/bin/bash

DATA_FILE="output.txt"
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
}' ${TMP_OUT} | sed 's/^\t/X\t/g' > ${TMP_OUT}a

numcols="$(awk '{print NF}' ${TMP_OUT}a | sort | tail -n1)"
solverCols=$(( $numSolvers / $numcols ))
iterInc=$(( $numcols / ${numSolvers}))
iterend=$(( ${numcols} ))
printf "Number of solvers:\t\t\t\t%s\nNumber of columns (solutions):\t%s \
       \nNumber of iterations:\t\t\t%s\n" \
       "${numSolvers}" "${numcols}" "$(( $iterend / $iterInc ))"

solverIter=0
# seq: start inc end
for iPos in $(seq 1 ${iterInc} ${iterend})
do
    range=$(( $iPos + ${iterInc} ))
    curSolver="${solvers[$solverIter]}"
    solverIter=$(( $solverIter + 1 ))
    cmdParse+=" for [col=${iPos}:${range}] '${TMP_OUT}a' using 0:col  \
                title \"${curSolver}\" \
                with points pointtype ${POINT_TYPE} pointsize ${POINT_SIZE} \
                lc rgb \"#${colors[${curSolver}]}\"
                 ,"
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
#     # plot for [col=1:${numcols}] '${TMP_OUT}a' using 0:col with lines"
echo ${cmd}
gnuplot -e "${cmd}"
cat ${TMP_OUT}a

# EOF