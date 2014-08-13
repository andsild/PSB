#!/usr/bin/python
from PIL import Image
from os.path import isfile, splitext
import sys
from random import randint, seed

seed(10)

COLOR_RANGE = 255;


def _createImage(width, height, valueGetterFunc,
                startVal = 0, COLOR_RANGE = 255):
    filename = findFilename(valueGetterFunc)
    createImage(filename,width,height,valueGetterFunc,startVal,COLOR_RANGE);

def createImage(filename, width, height, valueGetterFunc,
                startVal = 0, COLOR_RANGE = 255):

    im = Image.new('L', (width,height))
    pixels = im.load()

    for x in range(0, im.size[0]):
        for y in range(0, im.size[1]):
            pixels[x, y] = valueGetterFunc(x,y, width, height, startVal, COLOR_RANGE)
    im.save(filename);


#pixels[x, y] = valueGetterFunc(x,y, width, height, startVal, COLOR_RANGE)
def allSame(*args): return 100;
def gradientX(xPos,yPos, width,height, *args): return height - (yPos % COLOR_RANGE) + startVal;
def gradientY(xPos,yPos, width,height, *args): return width - (xPos % COLOR_RANGE) + startVal
def nonMonotone(xPos, yPos, *args): return pow(xPos + yPos,2)
def random(_x,_y,_w,_h,_s,COLOR_RANGE): return randint(0, COLOR_RANGE);
def randomBordered(x,y,w,h,_s,COLOR_RANGE):
    border=3
    if x < border or x > w - border \
    or y < border or y > w - border:
        return 0
    return randint(0, COLOR_RANGE);
def square(x,y,w,h,*args):
    border=2
    h-=1;w-=1; # indexing
    if y == (w - border) and (x <= w - border and x >= 0 + border) \
    or y == (0 + border) and (x <= w - border and x >= 0 + border) \
    or x == (h - border) and (y <= h - border and y >= 0 + border) \
    or x == (0 + border) and (y <= h - border and y >= 0 + border):
        return 100
    return 0
def circle(x,y,w,h,*args):
    h-=1;w-=1; # indexing
    radius = 3;
    centerPix_x = w / 2
    centerPix_y = h / 2
    disXCenter = abs(centerPix_x - x)
    disYCenter = abs(centerPix_y - y)
    # pythagoras
    lhs = (disXCenter)**2 + (disYCenter)**2
    rhs = radius**2
    # images are square, so this is an approximated circle
    if lhs >= (rhs - 1) and lhs <= (rhs + 1):
        return 100
    return 0
global_variable = 0
def dim1DStep(x,y,w,_h, _s, COLOR_RANGE):
    global global_variable
    border = 2
    if x >= border and x < w - border:
        global_variable += 50
        return global_variable % COLOR_RANGE
    return 0

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
    # _createImage(10, 10, allSame)
    # _createImage(10, 10, gradientX)
    # _createImage(10, 10, nonMonotone)
    # _createImage(10, 10, gradientY)
    # _createImage(8, 8, random)
    # _createImage(8, 8, square);
    # _createImage(11, 11, circle);
    # _createImage(10, 10, randomBordered);
    _createImage(8, 1, dim1DStep);
    pass
else:
    for arg in sys.argv[1:]:
        createImage(findFilename(arg), 10, 10, allSame)

# EOF
