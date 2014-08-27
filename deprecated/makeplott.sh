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
    printf  "usage: %s < inputfile directory >\n" "${0##*/}"
    exit 1;
fi


function findMax()
{
    max=0
    awk 'BEGIN {maxCol = 0 }
    {
        maxCol = (NF > maxCol) ? NF : maxCol;
    }
    END { print maxCol; }' ${1}
}

function MSE()
{
    awk 'BEGIN { totalAvg=0; }
    {
        if(NR%2==0)
        {
            sum = 0;
            for(i = 0; i < NF; i++)
            {
                sum += ($i * $i);
            }
            totalAvg += sqrt(sum);
        }
    }
    END { print totalAvg / NR; } ' ${1}
}

function fillRows()
{
    awk -v max=${2} '
    {
        if(NR%2==0)
        {
            for (i = NF+1; i < max+1; i++)
            {
                $i = "X";
            }
            printf ("%s\n", $0);
        }
    } ' ${1}
}

outdir="./out/" ; [ -d "${outdir}" ] && rm -rv ${outdir}; mkdir -v ${outdir}

function readFile()
{
    local  solverName=$(head -n1 ${1})
           solverName=${solverName%%_*}
    local  imageName=${1##*/}
           imageName=${imageName##*_}
           imageName="${imageName%%.*}"
    local  max=$(findMax ${1})
    local  lastAvg=$(MSE ${1})
    fillRows ${1} ${max} \
        >> "${outdir}${solverName}_${imageName}_${lastAvg}_${max}.tmp"
}


for file in $(find ${1} -type f)
do
    readFile ${file}
done

# EOF
