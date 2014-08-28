#!/usr/bin/python

import numpy as np
import matplotlib
matplotlib.use('agg')
import matplotlib.pyplot as plt
from matplotlib import cm as colormap
from matplotlib.colors import LogNorm
import pylab
from matplotlib import _png
from itertools import chain, tee, izip
from ipdb import set_trace
from PIL import Image
# from scipy.interpolate import interp1d

from sys import argv, exit
from os import listdir
from os.path import isfile, join
from math import log

from time import sleep

import gc

np.set_printoptions(threshold=np.nan)

GAUSS_C  = 8.5
JACOBI_C = 6.95
SOR_C    = 11.07
cdict = {"gauss" : GAUSS_C,
         "jacobi" : JACOBI_C,
         "sor" : SOR_C,
        }

DPI = 80
# the pixel-width/height is not entirely accurate, misses by 40/20/10...
BINS_X = float(600 / DPI) # where the scalar is pixel width
BINS_Y = float(600 / DPI) # where the scalar is pixel height
xMin = 100
yMin = 100
xMax = 450000 + xMin
yMax = 9.999999e-11
xMin = 9.999999e-7; xMax = 3600

def save(fig, filename):
    """We have to work around `fig.canvas.print_png`, etc calling `draw`."""
    renderer = fig.canvas.renderer
    with open(filename, 'w') as outfile:
        _png.write_png(renderer._renderer.buffer_rgba(),
                       renderer.width, renderer.height,
                       outfile, fig.dpi)
        
def drange(start, stop, step):
    r = start
    while r < stop:
        yield r
        r += step

def plot2DIterative(files, colors):
    global xMin, yMin, xMax, yMax
    plt.clf()
    plt.xlabel("Iterations")
    plt.ylabel("Error")
    fig, ax = plt.subplots(facecolor='none')
    ax.axis([xMin, xMax, yMax, yMin])
    ax.set_xscale("log")
    ax.set_yscale("log")
    ax.set_ybound(yMax, yMin)
    fig.canvas.draw()

    iterIndex = 0
    error = 0
    for (path, folder, solver), useColor in zip(files, colors):
        c = float(cdict[solver])
        for readfile in folder:
            print readfile
            index=0;
            with open(join(path,readfile), 'r') as f:
                origImage = join("/home/andesil/media", readfile[readfile.find("__")+2:-7] + ".jpg").replace("projectsNaturalnessdata", "")
                try:
                    im = Image.open(origImage);
                except IOError:
                    print "SKIPPING" + origImage
                    continue
                pixels = (im.size[0] * im.size[1]) / 4
                for line in f:
                    index += 1
                    if(index % 2 == 1):
                        continue
                    # ax = fig.axes[0]
                    lineData = line.split()
                    index = 0
                    dataY = lineData
                    # dataY = [0.0] * len(lineData)
                    # if len(lineData) > 1000:
                    #     for (r,s) in zip(range(1000), lineData):
                    #         dataY[r] = s
                    #     for (r,s) in zip(range(1000, len(lineData), 1000), lineData):
                    #         dataY[r] = s
                    # else:
                    #     dataY = lineData
                    # dataY = [float(x) for x in line.split()]
                    dataX = [0.0] * len(dataY)
                    for rangeIndex,x in enumerate(drange(0.1, xMax+1, c)):
                        if  rangeIndex > len(dataY) - 1: break
                        dataX[rangeIndex] = ((c * rangeIndex) / pixels)

                    plotLine = matplotlib.lines.Line2D(dataX, dataY,
                                                transform=ax.transData, color=useColor)
                    ax.draw_artist(plotLine)
                    lineData = []
                    # import ipdb; ipdb.set_trace()
                    # del dataX; del dataY;
            iterIndex += 1
            if iterIndex % 300 == 0:
                gc.collect()
                set_trace()

        save(fig, "plot2DIterative" + str(solver) + ".png")



def plot2D(files, colors):
    global xMin, yMin, xMax, yMax
    plt.clf()
    fig, ax = plt.subplots(facecolor='none')

    ax.set_xlabel("Iterations")
    ax.set_ylabel("Error")
    ax.axis([xMin, xMax, yMax, yMin])
    ax.set_xscale("log")
    ax.set_yscale("log")
    # ax.set_ybound(yMax, yMin)
    # fig.canvas.draw()

    iterIndex = 0
    error = 0
    dataX, dataY = [], []
    for (path, folder, solver), useColor in zip(files, colors):
        for readfile in folder:
            index=0;
            with open(join(path,readfile), 'r') as f:
                print readfile
                for line in f:
                    index += 1
                    if(index % 3 == 1):
                        continue
                    elif(index % 3 == 2):
                        error = float(line.split()[0])
                        continue
                    dataX.append(float(line.split()[0]) / 1000.0)
                    dataY.append(error)
        ax.scatter(dataX, dataY, color=useColor)
        dataX, dataY = [], []
    fig.savefig("plot2d" + "all.png")

def pairwise(iterable):
    "s -> (s0,s1), (s1,s2), (s2, s3), ..."
    a, b = tee(iterable)
    next(b, None)
    return izip(a, b)

def primHeatMap(files, colors):
    global DPI, BINS_X, BINS_Y
    global xMin, yMin, xMax, yMax

    fig = plt.figure(frameon=False, facecolor='none',
                     figsize=(BINS_X,BINS_Y), dpi=DPI)
    ax = fig.add_axes([0,0,1,1])
    ax.axis('off')

    width, height = fig.canvas.get_width_height()
    xbins = np.linspace(1, width,  width + 1)
    ybins = np.linspace(1, height, height + 1)

    histograms = []

    fig = plt.figure(frameon=False, facecolor='none',
                    figsize=(BINS_X,BINS_Y), dpi=DPI)
    ax = fig.add_axes([0,0,1,1])
    ax.set_xlim( (xMin, xMax) )
    ax.set_ylim( (yMax, yMin) )
    ax.axis('off')
    ax.set_xscale("log")
    ax.set_yscale("log")

    for (path, folder) in files:
        sumHistogram = np.zeros( (height, width) )
        index=0;
        iterIndex = 0

        for readfile in folder[:5]:
            with open(join(path,readfile), 'r') as f:
                print readfile
                for line in f:
                    index += 1
                    if(index % 2 == 1):
                        continue
                    dataY = [float(x) for x in line.split()]
                    cords = ax.transData.transform(
                        np.array([(x,y) for (x,y) in enumerate(dataY, 1) ]))
                    # cords = ax.transData.transform(
                    #     np.array([(x,y) for (x,y) in zip(dataY, np.logspace(0,5.0, len(dataY))) ]))
                    xcords, ycords = zip(*cords.astype(np.int))
                    interPolateSize = len(xcords)
                    # for cur,nxt in pairwise(xcords):
                    #     interPolateSize += abs(nxt - cur)
                    #
                    # xnew = np.linspace(0, max(xcords), num=interPolateSize)
                    # interFunc = interp1d(xcords, ycords,
                    #                     kind="linear")
                    # ynew = interFunc(xnew)
                    # tmp, _, _ = np.histogram2d(ynew, xnew,
                    #                            bins=[ybins, xbins])

                    tmp, _, _ = np.histogram2d(ycords, xcords,
                                               bins=[ybins, xbins])
                    tmp[tmp > 1] = 1
                   
                    sumHistogram += tmp
        histograms.append(sumHistogram)
       
    filenames = []
    for hist,color in zip(histograms, colors):
        filenames.append(renderHistogram(hist, color))

    if len(histograms) == 3:
        renderedImages = []
        for filename in filenames:
            img = Image.open(filename).convert("L")
            # renderedImages.append(plt.imread(filename).astype(np.uint8))
            renderedImages.append(np.asarray(img).astype(np.uint8))
            x = renderedImages[-1]

        rgb = np.dstack( x.astype(np.uint8) for x in renderedImages)
        img = Image.fromarray(rgb)
        img.save("combined.png")

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

def theoretical_bound(files, solvers):
    global xMin, yMin, xMax, yMax
    solverFuncs = [sorBound, jacobiBound, gaussBound]
    global START_POINT, BINS_X, BINS_Y, DPI, ERROR
    X_MAX_RANGE = 2000000
    Y_MAX_RANGE = 100
    yMax = 9.99999999e-3

    fig = plt.figure(frameon=False, facecolor='none',
                    figsize=(BINS_X, BINS_Y), dpi=DPI)
    ax = fig.add_axes([0,0,1,1])
    ax.set_xlim( (xMin, X_MAX_RANGE) )
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
def renderHistogram(histoGram, color):
    global DPI, BINS_X, BINS_Y
    global xMin, yMin, xMax, yMax

    plt.clf()
    fig = plt.figure(facecolor="none",figsize=(BINS_X+1,BINS_Y+1), dpi=DPI)
    xShift = yShift = (1.0 / ((BINS_X + 1) * DPI)) * DPI - 0.05
    ax = fig.add_axes([xShift,yShift,1.0-xShift-0.03,1.0-yShift-0.03])
    ax.set_frame_on(False)
   
    light_jet = cmap_map(lambda x: x/2+0.5, colormap.jet)
    orange = cmap_map(lambda x: x, colormap.OrRd)
    # orange = cmap_map(lambda x: func(x), colormap.OrRd)
    vmin=histoGram.min()

    ax.set_xscale("log")
    ax.set_yscale("log")
    ax.set_xlabel("Iterations (logscale)")
    ax.set_ylabel("Difference from original image (logscale)")
    if vmin == 0: vmin += 1
    ax.imshow(histoGram, cmap=orange, interpolation="quadric",
                norm=LogNorm(vmin=vmin, vmax=histoGram.max()),
                extent=[xMin, xMax, yMax, yMin],
                aspect="auto", origin="lower")
    ax.invert_yaxis()
    ax.set_xlim(xMin, xMax)
    ax.set_ylim(yMax, yMin)
    filename = "solverHeatmap" + color + ".png"
    fig.canvas.print_png(filename)
    print "wrote " + filename
    return filename

def directSolverHeatmap(files, colors):
    for (path, folder) in files:
        index=0;
        iterIndex = 0
        dataX, dataY = [], []

        for readfile in folder:
            with open(join(path,readfile), 'r') as f:
                print readfile
                for line in f:
                    index += 1
                    if(index % 2 == 1):
                        continue
                    line = line.split()
                    dataX.append(float(line[1]))
                    dataY.append(float(line[0]))
        heatmap, _, _ = np.histogram2d(dataX, dataY)

        plt.clf()
        xMin = 1
        xMax = 3500
        yMin = 10e-15
        yMax = 10e2
        fig, ax = plt.subplots(facecolor='none')
        _, _, _, ref = ax.hist2d(dataX, dataY,
                                 cmap=colormap.OrRd, cmin=0.9)
        set_trace()
        ax.set_xlabel("Solver time in ms")
        ax.set_ylabel("Difference from original image ")
        ax.set_xlim(xMin, xMax)
        ax.set_ylim(yMin, yMax)
        ax.set_yscale("log")
        cb = fig.colorbar(ref)
        cb.set_label("Number of solvers in buckets")
        # plt.imshow(heatmap, cmap=colormap.OrRd)
        plt.savefig("heatmap.png")

def doAverage(directFiles, iterativeFiles, colors):
    global DPI, BINS_X, BINS_Y
    global xMin, yMin, xMax, yMax

   

if __name__ == "__main__":
    if(len(argv) < 2):
        print "Usage: bin < folder>"
        exit(1)

    colors=[]
    files=[]
    directSolverFiles = []
    method=argv[1].upper()
    directsolvers = ["dst", "dct", "wavelet5", "wavelet7"]
    iterativeColors, directColors = [], []

    colorDict = { "dct": "yellow",
                 "dst": "orange",
                 "wavelet5": "blue",
                 "wavelet7": "pink",
                 "sor" : "green",
                 "jacobi" : "cyan",
                 "gauss" : "black",
                }

    shiftargs = 2
    for index in range(shiftargs, len(argv), 2):
        boolFlag = False
        for s in directsolvers:
            if s.upper() in argv[index].upper():
                directSolverFiles.append((argv[index],
                                        sorted([ f for f in listdir(argv[index]) \
                                            if isfile(join(argv[index], f))]),
                                          s))
                directColors.append(colorDict[s])
                set_trace()
                # directColors.append(colorDict[s]argv[index+1])
                boolFlag = True
        if not boolFlag:
            solver = argv[index].split("/")
            if solver[-1] == '':
                solver = solver[-2]
            else:
                solver = solver[-1]
            files.append((argv[index], sorted([ f for f in listdir(argv[index]) \
                                               if isfile(join(argv[index], f))]), solver))
            iterativeColors.append(colorDict[solver])

    numFiles=sum(len(x) for x in files)

    # plt.ioff()

    # facecolor = transparancy
    if method == 'plot2d'.upper():
        plot2DIterative(files, iterativeColors)
        plot2D(directSolverFiles, directColors)
    elif method == "average".upper():
        doAverage(directSolverFiles, files, colors)
    elif method == "primHM".upper():
        primHeatMap(files, iterativeColors)
        directSolverHeatmap(directSolverFiles, directColors)
    elif method == "tboundOMG".upper():
        theoretical_bound(files, iterativeColors)
    elif method == "tbound".upper():
        theo_line(iterativeColors)
    else:
        print "no method found for %s" % (method)
        exit(1)


    print "Done with plot!"
# EOF
