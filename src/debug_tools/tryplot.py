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
from math import log

import gc
import objgraph
from mem_top import mem_top

def save(fig, filename):
    """We have to work around `fig.canvas.print_png`, etc calling `draw`."""
    renderer = fig.canvas.renderer
    with open(filename, 'w') as outfile:
        _png.write_png(renderer._renderer.buffer_rgba(),
                       renderer.width, renderer.height,
                       outfile, fig.dpi)
def plot2D(files, colors, fig, ax):
    plt.xlabel("Iterations")
    plt.ylabel("Error")

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

def heatmap(numFiles, fig, ax, xStart, yStart, xMax, yMin):
    """plot a heatmap"""
    plt.xlabel("Iterations")
    plt.ylabel("Error")
    ax.set_xscale("log") # base 10 is default
    ax.set_yscale("log")

    from PIL import Image

    # im = Image.new('L', (plt.get_width(), plt.get_height()))
    origo = np.array([xStart, yStart])
    xOrigoPix, yOrigoPix = [int(x) for x in ax.transData.transform_point( np.array([xStart, yStart]))]
    xEndPix, yEndPix = [int(x) for x in ax.transData.transform_point( np.array([xMax, 9.999999e-11]))]

    im_width = 300
    im_height = 300
    x_range = (1, 500000)
    y_range = (100,  9.999999e-11)
    base = 10

    im = Image.new('RGB', (im_width, im_height), color=3)
    pixels = im.load()
    for x in range(0, im.size[0]):
        for y in range(0, im.size[1]):
            pixels[x,y] = 0
    
    inc = int(pow(8,log(numFiles, 2)))

    x_inc = pow(base, log(x_range[0], base))
    y_inc = pow(base, -log(y_range[0], base)+1)
    x_inc_orig = x_inc; y_inc_orig = y_inc
    x_pos = x_range[0]; y_pos = y_range[0]

    index_x = 0; index_y = 0

    print x_inc, y_inc

    R, G, B = 0, 0, 0
    for (index, (path, folder)) in enumerate(files, 1):
        useColor=colors.pop()
        if index == 1:
            R, G, B = inc, 0, 0
        elif index == 2:
            R, G, B = 0, inc, 0
        else:
            R, G, B = 0, 0, inc
        for readfile in folder:
            index=0;
            with open(join(path,readfile), 'r') as f:
                print readfile
                for line in f:
                    index += 1
                    if(index % 2 == 1):
                        continue
                    dataY = [float(x) for x in line.split()]
                    x_inc = x_inc_orig; y_inc = y_inc_orig
                    x_pos = 0; y_pos = 0
                    while int(x_pos) < len(dataY):
                        for (x, y) in enumerate(dataY[int(x_pos):int(x_pos*base)], int(x_pos)):
                            xPix = int(x_pos * x); yPix = int(y_pos * y)
                            x_pos += x_inc
                            y_pos += y_inc

                            RGB = pixels[xPix,yPix]
                            pixels[xPix, yPix] = ( RGB[0] + R, RGB[1] + G, RGB[2] + B)
                        x_inc /= base ; y_inc /= base
                    # import ipdb; ipdb.set_trace()
    im.save("heatmap.png")

    # im = Image.new('RGB', (xEndPix - xOrigoPix, yOrigoPix - yEndPix), color=3)
    # pixels = im.load()
    # for x in range(0, im.size[0]):
    #     for y in range(0, im.size[1]):
    #         pixels[x,y] = 0
    # print im.size[0], im.size[1]
    #
    # inc = int(pow(8,log(numFiles, 2)))
    # print inc
    #
    #
    #
    # R, G, B = 0, 0, 0
    # for (index, (path, folder)) in enumerate(files, 1):
    #     useColor=colors.pop()
    #     if index == 1:
    #         R, G, B = inc, 0, 0
    #     elif index == 2:
    #         R, G, B = 0, inc, 0
    #     else:
    #         R, G, B = 0, 0, inc
    #     for readfile in folder:
    #         index=0;
    #         with open(join(path,readfile), 'r') as f:
    #             print readfile
    #             for line in f:
    #                 index += 1
    #                 if(index % 2 == 1):
    #                     continue
    #                 dataY = [float(x) for x in line.split()]
    #                 for (x, y) in enumerate(dataY, 1):
    #                     cords = [int(a) for a in ax.transData.transform_point(np.array([x,y]))]
    #                     xPix = cords[0] - xOrigoPix
    #                     yPix = yOrigoPix - cords[1]
    #                     RGB = pixels[xPix,yPix]
    #                     pixels[xPix, yPix] = ( RGB[0] + R, RGB[1] + G, RGB[2] + B)
    #                 # import ipdb; ipdb.set_trace()
    # im.save("heatmap.png")
    #
if __name__ == "__main__":
    if(len(argv) < 2):
        print "Usage: bin < folder>"
        exit(1)

    colors=[]
    files=[]
    method=argv[1]
    method = method.upper()

    shiftargs = 2
    for index in range(shiftargs, len(argv), 2):
        colors.append(argv[index+1])
        files.append((argv[index], sorted([ f for f in listdir(argv[index]) if isfile(join(argv[index], f))])))

    numFiles=sum(len(x) for x in files)

    colors.reverse()

    plt.xscale('log')
    plt.yscale('log')
    plt.ioff()


    # facecolor = transparancy
    fig, ax = plt.subplots(facecolor='none') # note that ax and fig are now related
    size=100
    ax.set_xscale("log") # base 10 is default
    ax.set_yscale("log")
    xStart = 1; yStart = 100
    xMax = 450000 + xStart; yMax = size + yStart
    ax.axis([xStart, xMax, yStart, yMax]) # xmin xmax ymin ymax
    ax.set_ybound(9.999999e-11, size)
    fig.canvas.draw() # draw canvas first
    if method == 'plot2d'.upper():
        plot2D(files, colors, fig, ax)
    elif method == "heatmap".upper():
        heatmap(numFiles, fig, ax, xStart, yStart, xMax, yMax)


    print "Done with plot!"
    save(fig, "out.png")
# EOF
