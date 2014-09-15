import numpy as np
import matplotlib
matplotlib.use('agg')
import matplotlib.pyplot as plt

from scipy.interpolate import interp1d

piSq = np.square(np.pi)
ERROR=0.01
START_POINT = 10
def sorBound(pixels):
    global ERROR
    scalar = (np.sqrt(pixels) / -4.0) / (np.pi * 2)
    return (scalar * np.log(ERROR))
def jacobiBound(pixels):
    global piSq, ERROR
    scalar = (pixels * 2 / -4.0) / piSq
    return (scalar * np.log(ERROR))
def gaussBound(pixels):
    global piSq, ERROR
    scalar = (pixels / -4.0) / piSq
    return (scalar * np.log(ERROR))

def theo_line():
    global ERROR
    solverFuncs = [sorBound, jacobiBound, gaussBound]
    colors = ["sor", "jacobi", "gauss"]
    solverStart = 1
    yPlotPoints = np.array([10, ERROR])

    colorDict = { "sor" : "green",
                 "jacobi" : "cyan",
                 "gauss" : "black",
                }

    for solverFunc,color in zip(solverFuncs, colors):
        color =  colorDict[color]
        plt.clf()
        fig, ax = plt.subplots(facecolor="none")
        ax.set_ylim(10e-3, 10e1)
        ax.set_xscale("log")
        ax.set_yscale("log")
        ax.set_xlabel("Number of iterations")
        ax.set_ylabel("Difference from original signal")
        for n in np.logspace(4, 8, num=8):
            iterationEnd = solverFunc(n)
            xnew = np.linspace(1, iterationEnd, num=10000)
            interFunc = interp1d(np.array([1, iterationEnd]), yPlotPoints,
                                 kind="linear")
            ynew = interFunc(xnew)
            ax.plot(xnew, ynew, color=color)

        ax.set_xlim(1.0, ax.get_xlim()[1] * 10)
        
        fig.canvas.print_png("test" + str(color) + ".png")
        plt.close(fig)

theo_line()
