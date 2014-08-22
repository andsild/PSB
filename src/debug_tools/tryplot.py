#!/usr/bin/python

import numpy as np
import matplotlib
matplotlib.use('agg')
import matplotlib.pyplot as plt
from matplotlib import cm as colormap
import pylab
from matplotlib import _png
from itertools import chain
from ipdb import set_trace


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

def primHeatMap(files, fig, ax, xMin, yMin, xMax, yMax):

    X_MAX_RANGE = 3000 # max for sor
    Y_MAX_RANGE = 10

    DPI = 80
    # the pixel-width/height is not entirely accurate, misses by 40/20/10...
    BINS_X = float(600 / DPI) # where the scalar is pixel width
    BINS_Y = float(600 / DPI) # where the scalar is pixel height
    fig = plt.figure(frameon=False, facecolor='none',
                     figsize=(BINS_X,BINS_Y), dpi=DPI)
    ax = fig.add_axes([0,0,1,1])
    ax.axis('off')

    width, height = fig.canvas.get_width_height()

    for (path, folder) in files:
        useColor=colors.pop()
        plt.clf()
        sumHistogram = np.zeros( (height,width) )
        for readfile in folder:
            index=0;
            with open(join(path,readfile), 'r') as f:
                print readfile
                for line in f:
                    index += 1
                    if(index % 2 == 1):
                        continue
                    dataY = [float(x) for x in line.split()]
                    dataX = range(1, len(dataY) + 1)

                    fig = plt.figure(frameon=False, facecolor='none',
                                    figsize=(BINS_X,BINS_Y), dpi=DPI)
                    ax = fig.add_axes([0,0,1,1])
                    ax.axis('off')
                    ax.plot(dataY, color="blue", transform=ax.transData)
                    ax.set_autoscale_on(False)
                    ax.set_yscale("log")
                    ax.set_xlim( (0, X_MAX_RANGE) )
                    ax.set_ylim( (9.99999999e-11, Y_MAX_RANGE) )

                    fig.canvas.draw()
                    data=np.fromstring(fig.canvas.tostring_rgb(), dtype=np.uint8, sep='')
                    # tuple with width and height, reversed, with padded 3 (for z = 3)
                    d3 = data.reshape(fig.canvas.get_width_height()[::-1] + (3,))
                    colRange=[ [num[2] for num in s] for s in d3]
                    COORDS_LIST = [ x for x in chain.from_iterable(
                        [ [ (height-rowIndex, pos) for (pos,pixel) in enumerate(row) \
                             if pixel > 0] \
                          for (rowIndex, row) in enumerate(colRange)])]
                    dataY, dataX = zip(*COORDS_LIST)
                    htmp, xedge, yedge = np.histogram2d(dataY, dataX,
                        bins=[np.linspace(0, height, num=height+1), np.linspace(1, width, num=width+1)])
                    
                    # htmp, xedge, yedge = np.histogram2d(dataX, dataX,
                    #                                     bins=[binY, binX])
                                        #bins=[yRange, xRange])
                    sumHistogram += htmp
                    plt.close(fig)

        plt.clf()
        fig = plt.figure(frameon=False, facecolor='none',
                        figsize=(BINS_X,BINS_Y), dpi=DPI)
        ax = fig.add_axes([0,0,1,1])
        ax.axis('off')
        ax.set_xlim(0, width-1)
        ax.set_ylim(1, height-1)
        ax.pcolor(sumHistogram, cmap=colormap.OrRd)
        # ax.contourf(sumHistogram, cmap=colormap.OrRd)
        # ax.set_yscale("log")
        # ax.set_xscale("log")
        # ax.imshow(sumHistogram, cmap=colormap.OrRd)
        # ax.invert_yaxis()
        fig.canvas.print_png("test.png")
        print "wrote test.png"
        plt.show()
        exit(0)

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
    #                 x_inc = x_inc_orig; y_inc = y_inc_orig
    #                 x_pos = 1; y_pos = 1
    #                 while int(x_pos*base) < len(dataY):
    #                     for (x, y) in enumerate(dataY[int(x_pos):int(x_pos*base)], int(x_pos)):
    #                         xPix = int(x_pos); yPix = int(y_pos * y)
    #                         x_pos += x_inc
    #                         y_pos += y_inc
    #
    #                         RGB = pixels[xPix,yPix]
    #                         pixels[xPix, yPix] = ( RGB[0] + R, RGB[1] + G, RGB[2] + B)
    #                     x_inc /= base ; y_inc /= base
    #                 # import ipdb; ipdb.set_trace()
    # im.save("heatmap.png")

    im = Image.new('RGB', (xEndPix - xOrigoPix, yOrigoPix - yEndPix), color=3)
    pixels = im.load()
    for x in range(0, im.size[0]):
        for y in range(0, im.size[1]):
            pixels[x,y] = 0
    print im.size[0], im.size[1]
    
    inc = int(pow(8,log(numFiles, 2)))
    print inc
    
    xMax = 300
    numbins = 200
    mybinsX = np.linspace(xStart, xMax, numbins)
    mybinsY = np.linspace(xStart, xMax, numbins)
    sumHistogram = np.zeros( (numbins - 1, numbins - 1) )
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
    #                 dataX = range(len(dataY))
    #
    #                 heatmap, xedges, yedges = np.histogram2d(dataX, dataY, bins=(512,384))
    #                 import ipdb; ipdb.set_trace()
    
                    # for (x, y) in enumerate(dataY, 1):
                    #     cords = [int(a) for a in ax.transData.transform_point(np.array([x,y]))]
                    #     xPix = cords[0] - xOrigoPix
                    #     yPix = yOrigoPix - cords[1]
                    #     RGB = pixels[xPix,yPix]
                    #     pixels[xPix, yPix] = ( RGB[0] + R, RGB[1] + G, RGB[2] + B)
                    # import ipdb; ipdb.set_trace()
    # im.save("heatmap.png")
    
if __name__ == "__main__":
    if(len(argv) < 2):
        print "Usage: bin < folder>"
        exit(1)

    colors=[]
    files=[]
    method=argv[1].upper()

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
    xMax = 450000 + xStart; yMax = 9.999999e-11
    ax.axis([xStart, xMax, yStart, yMax]) # xmin xmax ymin ymax
    ax.set_ybound(9.999999e-11, size)
    fig.canvas.draw() # draw canvas first
    if method == 'plot2d'.upper():
        plot2D(files, colors, fig, ax)
    elif method == "heatmap".upper():
        heatmap(numFiles, fig, ax, xStart, yStart, xMax, yMax)
    elif method == "primHM".upper():
        primHeatMap(files, fig, ax, xStart, yStart, xMax, yMax)
    else:
        print "no method found for %s" % (method)
        exit(1)


    print "Done with plot!"
    save(fig, "out.png")
# EOF
