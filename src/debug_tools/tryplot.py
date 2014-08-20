#!/usr/bin/python

import numpy as np
import matplotlib
matplotlib.use('agg')
import matplotlib.pyplot as plt
import pylab
from matplotlib import _png
from itertools import izip

from sys import argv, exit
from os import listdir
from os.path import isfile, join

from random import randint

def main(inData, color):
    fig = plt.figure()
    ax = fig.add_subplot(111)
    ax.plot(inData, scalex=False, scaley=False)
    fig.savefig("out" + str(randint(0,10000)) + ".png")
    pylab.close()
    # plt.plot(inData, color, rasterized=False, clip_on=True)



def doMe(inData, color, ax):
    # This won't actually ever be drawn. We just need an artist to update.
    # ax.extend(inData)
    # col = ax.plot(inData, alpha=0.3)
    # ax.draw_artist(col)
    pass

    # for xy, color in datastream(int(1e6), chunksize=int(1e4)):
    #     ax.draw_artist(col)

def datastream(n, chunksize=1):
    """Returns a generator over "n" random xy positions and rgb colors."""
    for _ in xrange(n//chunksize):
        xy = 10 * np.random.random((chunksize, 2))
        color = np.random.random((chunksize, 3))
        yield xy, color

def save(fig, filename):
    """We have to work around `fig.canvas.print_png`, etc calling `draw`."""
    renderer = fig.canvas.renderer
    with open(filename, 'w') as outfile:
        _png.write_png(renderer._renderer.buffer_rgba(),
                       renderer.width, renderer.height,
                       outfile, fig.dpi)




if __name__ == "__main__":
    if(len(argv) < 2):
        print "Usage: bin < folder>"
        exit(1)

    colors=[]
    files=[]
    for index in range(1, len(argv), 2):
        colors.append(argv[index+1])
        files.append((argv[index], sorted([ f for f in listdir(argv[index]) if isfile(join(argv[index], f))])))

    colors.reverse()

    plt.xlabel("Iterations")
    plt.ylabel("Error")
    plt.xscale('log')
    plt.yscale('log')
    # plt.ylim(0,1000)
    # plt.xlim(0,10)
    plt.ioff()
    # plt.autoscale(False, axis='both')
    # for readfile in files:
    #     index=0;
    #     with open(join(dirFiles,readfile), 'r') as f:
    #         print readfile
    #         for line in f:
    #             index += 1
    #             if(index % 2 == 1):
    #                 continue
    #             main([float(x) for x in line.split()], color)
    #

    # facecolor = transparancy
    fig, ax = plt.subplots(facecolor='none') # note that ax and fig are now related
    size=100
    ax.set_xscale("log") # base 10 is default
    ax.set_yscale("log")
    ax.axis([1, 450000 + 1, 0.0, size + 0]) # xmin xmax ymin ymax
    ax.set_ybound(9.999999e-11, size)
    fig.canvas.draw() # draw canvas first
    # import ipdb; ipdb.set_trace();

    import gc
    import objgraph
    from mem_top import mem_top
    iterIndex = 0
    for (path, folder) in files:
        useColor=colors.pop()
        for readfile in folder:
            index=0;
            with open(join(path,readfile), 'r') as f:
                print readfile
                for line in f:
                    index += 1
                    if(index % 2 == 1):
                        continue
                    ax = fig.axes[0]
                    dataY = [float(x) for x in line.split()]
                    dataX = range(1, len(dataY) + 1)

                    line = matplotlib.lines.Line2D(dataX, dataY,
                                                transform=ax.transData, color=useColor)
                    ax.draw_artist(line)
                    # import ipdb; ipdb.set_trace()
                    del dataX; del dataY;
            iterIndex += 1
            if iterIndex % 300 == 0:
                gc.collect()
                import ipdb; ipdb.set_trace();




    print "Done with plot!"
    # import ipdb; ipdb.set_trace();
    save(fig, "out.png")
    # plt.savefig("out.png");
    # plt.show();


# EOF


    # datatype = [("index", np.float32), ("floati", np.float32), ('floatq', np.float32)]
    # # data = np.memmap("./testdata/monster/monster.tmp", datatype, 'r');
    # data = np.genfromtxt("./testdata/monster/monster.tmp", delimiter="  ", missing_values="", dtype=datatype)
    # data = np.genfromtxt("../testdata/testbabe.tmp", missing_values="X", filling_values=0, dtype=datatype)
    # data = np.genfromtxt("../testdata/testbabe.tmp", missing_values="X", filling_values=0)
    # print data
    # plt.plot(data['floati'], data["floatq"], 'r')
    # plt.plot(data);
    # plt.show()
    # save(plt);
    # plt.savefig("out.png")
