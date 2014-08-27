#!/usr/bin/python

import sys
from os.path import join
from itertools import product
from subprocess import call

def main(path):
    cmdArgs = []
    cmdArgs.append(("-f", [ path + s for s in \
                    ["105400.jpg", "121701.jpg", "121901.jpg", "146001.jpg",
                     "136006.jpg", "123400.jpg", "110000.jpg", "117102.jpg",
                     "100000.jpg", "1113000.jpg"]]))
    cmdArgs.append(("--", ["gauss", "jacobi", "sor"]))
    cmdArgs.append(("-t", ["0.01"]))

    #TODO: put your command prefix here, e.g. "bjob -p main.out"
    exeLine = ["main.out"]

    for (param,arg) in cmdArgs:
        sep = " "
        if param == "--": sep = ""
        tmpHist = [ ' '.join(item) for item in \
                   (product(exeLine, [ param + sep + val for val in arg ]))]
        exeLine = tmpHist

    for cmd in exeLine:
        try:
            call(cmd)
        except OSError:
            print "Failed to execute command line:\n\t " + cmd
            continue;

if __name__ == '__main__':
    if len(sys.argv) < 2:
        print "error, no argv.."
        exit(1)
    path = sys.argv[1]
    main(path)


# EOF
