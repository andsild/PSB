#!/usr/bin/python
from PIL import Image
import sys

def printMe(filename):
    im = Image.open(filename)
    pixels = im.load()

    offset = 0
    columns = 8

    for i in range(im.size[0] / columns):

        for y in range(im.size[1]):
            for x in range(offset, offset+columns):
                if x >= im.size[0]: break;
                print("%3d " % (pixels[x,y])) ,
            print
        offset += columns
        print "\n"

if not len(sys.argv) > 1:
    print "Usage:", sys.argv[0], " <image_name>"
else:
    for arg in sys.argv[1:]:
        printMe(arg)
        print

# EOF 
