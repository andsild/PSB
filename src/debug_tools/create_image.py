#!/usr/bin/python
from PIL import Image
from os.path import isfile, splitext
import sys

COLOR_RANGE = 255
width = 1
height = 10

xBorderValues = 0
yBorderValues = 0

leftbordervalue = 0
rightbordervalue = 0
uppbordervalue = leftbordervalue
lowbordervalue = leftbordervalue

startVal = COLOR_RANGE - width - 10
startVal = 0

def createImage(filename, width, height, valueGetterFunc,
                startVal = 0, COLOR_RANGE = 255,
                xBordersValues = 0, yBorderValues = 0,
                leftbordervalue = 0, rightbordervalue = 0,
                upperboardvalue = leftbordervalue, lowerbordervalue = leftbordervalue,
                ):

    im = Image.new('L', (width,height))
    pixels = im.load()

    for x in range(xBorderValues, im.size[0] - xBorderValues):
        for y in range(yBorderValues, im.size[1] - yBorderValues):
            # pixels[x, y] = getVal(y,x)
            pixels[x, y] = valueGetterFunc(x,y, width, height, startVal, COLOR_RANGE)

    # borders
    # left
    for x in range(0, xBorderValues):
        for y in range(0, im.size[1]):
            pixels[x,y] = leftbordervalue

    # right
    for x in range(im.size[0] - xBorderValues, im.size[0]):
        for y in range(0, im.size[1]):
            pixels[x,y] = rightbordervalue

    # upper
    for x in range(0, im.size[0]):
        for y in range(0, yBorderValues):
            pixels[x,y] = uppbordervalue

    # lower
    for x in range(0, im.size[0]):
        for y in range(im.size[1] - yBorderValues, im.size[1]):
            pixels[x,y] = lowbordervalue

    im.save(filename);

def allSame(*args): return 100;
def gradientX(xPos,yPos, width,height, *args): return height - (yPos % COLOR_RANGE) + startVal;
def gradientY(xPos,yPos, width,height, *args): return width - (xPos % COLOR_RANGE) + startVal
def nonMonotone(xPos, yPos, *args): return pow(xPos + yPos,2)

def findFilename(prefix):
    if(hasattr(prefix, '__call__')):
       prefix = prefix.__name__ + ".png"
    appender = "0123456789"
    filename, extension = splitext(prefix)
    index = 0;
    while(isfile(filename + extension)):
        filename+=appender[index % len(appender)]
        index += 1
    return filename + extension;

if not len(sys.argv) > 1:
    # createImage(findFilename(allSame), 10, 10, allSame)
    # createImage(findFilename(gradientX), 10, 10, gradientX)
    createImage(findFilename(nonMonotone), 10, 10, nonMonotone)
    # createImage(findFilename(gradientY), 10, 10, gradientY)
else:
    for arg in sys.argv[1:]:
        createImage(findFilename(arg), 10, 10, allSame)

# EOF
