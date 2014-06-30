#pylint:disable-all

from PIL import Image
import sys

COLOR_RANGE = 255
width = 30
height = 8

xBorderValues = 0
yBorderValues = 0

leftbordervalue = 0
rightbordervalue = 0
uppbordervalue = leftbordervalue
lowbordervalue = leftbordervalue

startVal = COLOR_RANGE - width - 10

im = Image.new('L', (width,height))
pixels = im.load()

def getVal(xPos, yPos):
    # return 100
    return (xPos % COLOR_RANGE) + startVal


for x in range(xBorderValues, im.size[0] - xBorderValues):
    for y in range(yBorderValues, im.size[1] - yBorderValues):
        pixels[x, y] = getVal(y,x)

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

if not len(sys.argv) > 1:
    print "Usage:", sys.argv[0], " <image_name>"
else:
    im.save(sys.argv[1])
