#!/bin/bash
#: Title        : draw_graph
#: Date         : Jul 2014
#: Author       : Anders Sildnes
#: Version      : 2.0 (1.0 used column data)
#: Desctiption  : Convert output from PSB to a format for gnuplot
#: Options      : $1 should be a file that the info is dumped into 
#:                (default to terminal)

# if [ -z "${1}" ]
# then
#     printf  "usage: %s < inputfile directory >\n" "${0}"
#     exit 1;
# fi

declare -a arrayvar solvers
declare -A colors
colors["gauss"]="FFB801"
colors["jacobi"]="15A600"
colors["sor"]="5141FF"

POINT_TYPE="1" #http://stelweb.asu.cas.cz/~nemeth/work/stuff/gnuplot/gnuplot-line-and-point-types-bw.png
POINT_SIZE="2"
IFS=$'\n'

function fillRows()
{
    max=0
    for line in $(sed -n 2~2p ${1})
    do
        x=$(printf "%s" ${line} | awk ' { print NF } ')
        if [[ ${x} -gt ${max} ]]
        then
            max=${x}
        fi
    done
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
    }' ${1}
}

cmdParse=""
function readFile()
{
    solverName=$(head -n1 ${1})
    solverName=${solverName%%_*}
    
    cmdParse+="\"<( printf \"%s\" \"$(fillRows ${1})\" | sed -n 2~2p)\" matrix with lines \
                \"#${colors[${solverName}]}\", \
                "
}

for file in $(find . -name "*.dat")
do
    readFile ${file}
done


cmdParse=$(echo ${cmdParse} | sed -e 's/,$//g ; s/,/&\n/g' )
cmdParse="plot ${cmdParse} ;"
echo ${cmdParse}

cmd="set terminal png; set output 'test.png';
    set key outside;
    set ylabel \"Mean Square difference from input and solution\";
    set xlabel \"(logscale) Iterations\";
    set logscale x;
    ${cmdParse}
     "
gnuplot -e "${cmd}" 2>&1 | sed '/.*arial.*/d'

# EOF
