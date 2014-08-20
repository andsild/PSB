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

    color="black"
    if(len(argv) > 2):
        color=argv[2]

    dirFiles=argv[1]
    files = [ f for f in listdir(dirFiles) if isfile(join(dirFiles, f))]
    files.sort();

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


    # facecolor = transparancy
    fig, ax = plt.subplots(facecolor='none') # note that ax and fig are now related
    fig.set_frameon(True)
    size=10
    ax.axis([0, 10000, 0, size]) # xmin xmax ymin ymax
    fig.canvas.draw() # draw canvas first

    import gc
    import objgraph
    from mem_top import mem_top
    # for i in range(0, 100):
    #     ax = fig.axes[0]
    #     dataX = range(size)
    #     dataY = range(100+i, size+100+i)
    #     line = matplotlib.lines.Line2D(dataX, dataY,
    #                                 transform=ax.transData)
    #     ax.draw_artist(line)
    #     del ax
    #     del dataX; del dataY;
    #     gc.collect()
    #     pass
    # import ipdb; ipdb.set_trace()
    #
    # line2 = matplotlib.lines.Line2D(range(4), [1,2.5,2,3],
    #                                figure=fig, transform=ax.transData)
    # ax.draw_artist(line2)
    # fig.lines.extend([line2]);
    # fig.canvas.draw()
    # fig.lines.pop()

    for readfile in files:
        index=0;
        with open(join(dirFiles,readfile), 'r') as f:
            print readfile
            for line in f:
                index += 1
                if(index % 2 == 1):
                    continue
                ax = fig.axes[0]
                dataY = [float(x) for x in line.split()]
                dataX = range(0, len(dataY))

                line = matplotlib.lines.Line2D(dataX, dataY,
                                            transform=ax.transData, color=color)
                ax.draw_artist(line)
                del dataX; del dataY;
                gc.collect()
                if index % 100 == 0:
                    import ipdb; ipdb.set_trace();

    print "Done with plot!"
    # import ipdb; ipdb.set_trace();
    save(fig, "out.png")
    # plt.savefig("out.png");
    # plt.show();



