import os, sys
from PIL import Image

im = Image.open(sys.argv[1])
pix = im.load()
print pix[int(sys.argv[2]), int(sys.argv[3])]
