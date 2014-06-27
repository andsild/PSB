#pylint:disable-all

from PIL import Image
import sys

width = 10
height = 3

im = Image.new('L', (width,height))
pixels = im.load()

for xIndex, x in enumerate(range(im.size[0])):
    for yIndex, y in enumerate(range(im.size[1])):
        val = xIndex + (yIndex * im.size[0])
        pixels[x, y] = val

if not len(sys.argv) > 1:
    print "Usage:", sys.argv[0], " <image_name>"
else:
    im.save(sys.argv[1])
