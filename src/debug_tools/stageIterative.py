#!/usr/bin/python

import sys
from itertools import product
from subprocess import call

def main(path):
    cmdArgs = [
        ("-f", [path + s for s in \
                        ["105400.jpg", "121701.jpg", "121901.jpg", "146001.jpg",
                        "136006.jpg", "123400.jpg", "110000.jpg", "117102.jpg",
                        "100000.jpg", "1113000.jpg"]]),
        ("--", ["gauss", "jacobi", "sor"]),
        ("-t", ["0.01"]) \
    ]

    #TODO: put your command prefix here, e.g. "bjob -p main.out"
    exeLine = "./main.out"
    exeArgs = [""]


    # Generate cartesian product of possible combinations of cmdArgs
    for (param, arg) in cmdArgs:
        sep = " "
        if param == "--":
            sep = ""
        tmpHist = [' '.join(item) for item in \
                   product(exeArgs, [param + sep + val for val in arg ])]
        exeArgs = tmpHist

    for cmd in exeArgs:
        try:
            print "Executing:\t" + str([exeLine] + [s for s in cmd.split()]),
            retVal = call([exeLine] + [s for s in cmd.split()])
            print "with value " + str(retVal)
        except OSError as oe:
            print oe
            print "Failed to execute command line:\n\t " + cmd
            continue;

if __name__ == '__main__':
    if len(sys.argv) < 2:
        print "error, no argv.."
        exit(1)
    path = sys.argv[1]
    main(path)


# EOF
