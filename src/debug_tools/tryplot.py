#!/usr/bin/python

import numpy as np
import matplotlib
matplotlib.use('agg')
import matplotlib.pyplot as plt
from matplotlib import cm as colormap
from matplotlib.colors import LogNorm
import pylab
from matplotlib import _png
from itertools import chain
from ipdb import set_trace
from PIL import Image
from scipy.interpolate import interp1d


from sys import argv, exit
from os import listdir
from os.path import isfile, join
from math import log

import gc
import objgraph
from mem_top import mem_top


DPI = 80
# the pixel-width/height is not entirely accurate, misses by 40/20/10...
BINS_X = float(600 / DPI) # where the scalar is pixel width
BINS_Y = float(600 / DPI) # where the scalar is pixel height

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

def primHeatMap(files, colors, xMin, yMin, xMax, yMax):
    global DPI, BINS_X, BINS_Y

    X_MAX_RANGE = xMax
    Y_MAX_RANGE = yMin

    fig = plt.figure(frameon=False, facecolor='none',
                     figsize=(BINS_X,BINS_Y), dpi=DPI)
    ax = fig.add_axes([0,0,1,1])
    ax.axis('off')

    width, height = fig.canvas.get_width_height()

    xdim = 560
    ydim = 560

    xbins = np.linspace(1, width,  xdim + 1)
    ybins = np.linspace(1, height, ydim + 1)

    histograms = []


    fig = plt.figure(frameon=False, facecolor='none',
                    figsize=(BINS_X,BINS_Y), dpi=DPI)
    ax = fig.add_axes([0,0,1,1])
    ax.set_xlim( (10, X_MAX_RANGE) )
    ax.set_ylim( (yMax, Y_MAX_RANGE) )
    ax.axis('off')
    ax.set_xscale("log")
    ax.set_yscale("log")

    for (path, folder) in files:
        sumHistogram = np.zeros( (ydim, xdim) )
        index=0;
        iterIndex = 0

        for readfile in folder:
            with open(join(path,readfile), 'r') as f:
                print readfile
                for line in f:
                    index += 1
                    if(index % 2 == 1):
                        continue
                    dataY = [float(x) for x in line.split()]
                    cords = ax.transData.transform( 
                        np.array([(x,y) for (x,y) in enumerate(dataY, 1) ]))
                    xcords, ycords = zip(*cords)
                    tmp, _, _ = np.histogram2d([int(y) for y in ycords],
                                               [int(x) for x in xcords],
                                               bins=[ybins, xbins])
                    sumHistogram += tmp
        histograms.append(sumHistogram)
        
    for hist in histograms:
        useColor = colors.pop()
        plt.clf()
        fig = plt.figure(frameon=False, facecolor='none',
                        figsize=(BINS_X,BINS_Y), dpi=DPI)
        ax = fig.add_axes([0,0,1,1])
        ax.axis('off')
        # ax.pcolor(sumHistogram, cmap=colormap.OrRd)
        # ax.imshow(sumHistogram, cmap=colormap.OrRd)
        # ax.contourf(sumHistogram, cmap=colormap.OrRd)
        light_jet = cmap_map(lambda x: x/2+0.5, colormap.jet)
        orange = cmap_map(lambda x: x, colormap.OrRd)
        # orange = cmap_map(lambda x: func(x), colormap.OrRd)
        ax.imshow(sumHistogram, cmap=orange, interpolation="quadric", vmin=0)
        ax.invert_yaxis()
        fig.canvas.print_png("test" + useColor + ".png")
        print "wrote test" + useColor + ".png"

def func(x):
    ret = [ int( 200 *  log(255 * x[0]) / 255.0) ]
    for num in x[1:]:
        ret.append(num)
    return np.array(ret)

def cmap_map(function,cmap):
    """ Applies function (which should operate on vectors of shape 3:
    [r, g, b], on colormap cmap. This routine will break any discontinuous     points in a colormap.
    """
    cdict = cmap._segmentdata
    step_dict = {}
    # Firt get the list of points where the segments start or end
    for key in ('red','green','blue'):         step_dict[key] = map(lambda x: x[0], cdict[key])
    step_list = sum(step_dict.values(), [])
    step_list = np.array(list(set(step_list)))
    # Then compute the LUT, and apply the function to the LUT
    reduced_cmap = lambda step : np.array(cmap(step)[0:3])
    old_LUT = np.array(map( reduced_cmap, step_list))
    new_LUT = np.array(map( function, old_LUT))
    # Now try to make a minimal segment definition of the new LUT
    cdict = {}
    for i,key in enumerate(('red','green','blue')):
        this_cdict = {}
        for j,step in enumerate(step_list):
            if step in step_dict[key]:
                this_cdict[step] = new_LUT[j,i]
            elif new_LUT[j,i]!=old_LUT[j,i]:
                this_cdict[step] = new_LUT[j,i]
        colorvector=  map(lambda x: x + (x[1], ), this_cdict.items())
        colorvector.sort()
        cdict[key] = colorvector

    return matplotlib.colors.LinearSegmentedColormap('colormap',cdict,1024)

piSq = np.square(np.pi)
ERROR=0.01
START_POINT = 10
def sorBound(pixels):
    global ERROR
    scalar = (np.sqrt(pixels) / -4.0) / (np.pi * 2)
    return scalar * np.log(ERROR)
def jacobiBound(pixels):
    global piSq, ERROR
    scalar = (pixels * 2 / -4.0) / piSq
    return scalar * np.log(ERROR)
def gaussBound(pixels):
    global piSq, ERROR
    scalar = (pixels / -4.0) / piSq
    return scalar * np.log(ERROR)

def theo_line(colors):
    global ERROR
    solverFuncs = [sorBound, jacobiBound, gaussBound]
    solverStart = 1

    yPlotPoints = np.array([10, ERROR])

    for solverFunc,color in zip(solverFuncs, colors):
        plt.clf()
        fig, ax = plt.subplots()
        ax.set_ylim(10e-3, 10e1)
        ax.set_xscale("log")
        ax.set_yscale("log")
        for n in np.logspace(4, 8, num=8):
            iterationEnd = solverFunc(n)
            xnew = np.linspace(1, iterationEnd, num=10000)
            interFunc = interp1d(np.array([1, iterationEnd]), yPlotPoints,
                                 kind="linear")
            ynew = interFunc(xnew)
            ax.plot(xnew, ynew, color=color)
         
        fig.canvas.print_png("test" + str(color) + ".png")
        plt.close(fig)

def theoretical_bound(files, solvers, xMin, yMin, xMax, yMax):
    solverFuncs = [sorBound, jacobiBound, gaussBound]
    global START_POINT, BINS_X, BINS_Y, DPI, ERROR
    X_MAX_RANGE = 2000000
    Y_MAX_RANGE = 100
    yMax = 9.99999999e-3

    fig = plt.figure(frameon=False, facecolor='none',
                    figsize=(BINS_X, BINS_Y), dpi=DPI)
    ax = fig.add_axes([0,0,1,1])
    ax.set_xlim( (100, X_MAX_RANGE) )
    ax.set_ylim( (yMax, Y_MAX_RANGE) )
    ax.axis('off')
    ax.set_xscale("log")
    ax.set_yscale("log")

    histograms = []
    yPlotPoints = np.array([8, ERROR])

    width, height = fig.canvas.get_width_height()
    xdim = 560
    ydim = 560

    xbins = np.linspace(1, width,  xdim + 1)
    ybins = np.linspace(1, height, ydim + 1)

    for (path, folder) in files:
        solver = solverFuncs.pop()
        sumHistogram = np.zeros( (ydim, xdim) )

        for readFile in folder:
            im = Image.open(join(path,readFile));
            pixels = im.size[0] * im.size[1]

            iterationEnd = solver(pixels)

            xnew = np.linspace(1, iterationEnd, xdim)
            interFunc = interp1d(np.array([1, iterationEnd]), yPlotPoints,
                                 kind="linear")
            ynew = interFunc(xnew)
            
            cords = ax.transData.transform( 
                np.array([(x,y) for (x,y) in zip(xnew, ynew) ]))
            xcords, ycords = zip(*cords)

            tmp, _, _ = np.histogram2d([int(y) for y in ycords],
                                        [int(x) for x in xcords],
                                        bins=[ybins, xbins])
            sumHistogram += tmp
        histograms.append(sumHistogram)

    for hist in histograms:
        useColor = colors.pop()
        plt.clf()
        fig = plt.figure(figsize=(BINS_X+1,BINS_Y+1), dpi=DPI)
        ax = fig.add_axes([0.125,0.125,(1.0 - 0.125), (1.0-0.125)])
        # ax.axis('off')
        # ax.pcolor(hist, cmap=colormap.OrRd)
        # ax.imshow(sumHistogram, cmap=colormap.OrRd)
        # ax.contourf(sumHistogram, cmap=colormap.OrRd)
        ax.set_xlim(0, 200)
        ax.set_ylim(0, 200)
        light_jet = cmap_map(lambda x: x/2+0.5, colormap.jet)
        orange = cmap_map(lambda x: x, colormap.OrRd)
        # orange = cmap_map(lambda x: func(x), colormap.OrRd)
        ax.imshow(hist, cmap=orange, norm=LogNorm(),
                  extent=[0, 200, 0, 200],
                  interpolation="bilinear")
        ax.invert_yaxis()
        fig.canvas.print_png("test" + useColor + ".png")
        print "wrote test" + useColor + ".png"



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
        files.append((argv[index], sorted([ f for f in listdir(argv[index]) \
                                           if isfile(join(argv[index], f))])))

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
    elif method == "primHM".upper():
        primHeatMap(files, colors, xStart, yStart, xMax, yMax)
    elif method == "tboundOMG".upper():
        theoretical_bound(files, colors, xStart, yStart, xMax, yMax)
    elif method == "tbound".upper():
        theo_line(colors)
    else:
        print "no method found for %s" % (method)
        exit(1)


    print "Done with plot!"
# EOF
