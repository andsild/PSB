#!/usr/bin/python
from PIL import Image
import sys

def printMe(filename):
    im = Image.open(filename)
    pixels = im.load()

    for x in range(0, im.size[0]):
        for y in range(0, im.size[1]):
            print("%3d " % (pixels[x,y])) ,
        print

if not len(sys.argv) > 1:
    print "Usage:", sys.argv[0], " <image_name>"
else:
    for arg in sys.argv[1:]:
        printMe(arg)
        print

# EOF 
