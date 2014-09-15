PSB
===

[P]oisson[S]olver[Benchmarks].

# To build:
```bash
cd build && cmake ../src && make
../bin/main
```


# Images
To get the images: 
```bash
mkdir "./media" && wget http://pascal.inrialpes.fr/data/holidays/jpg{1,2}.tar.gz && tar -xvf jpg{1,2}.tar.gz --strip-components=1 -C media
```

### INTRODUCTION

This program will allow you to benchmark methods for solving poissons equation.
In this case the input is images, and we're performing gradient field reconstruction.

The program is written in C++ for speed, however, concepts like threading
and multigrids are currently avoided for ease of coding.

### EDITIONS

"PSB" is a work in progress, so the structure of the code is changed from
time to time. Do frequent "git pull" if you intend to use it.

#### PSB

* PSB keeps it simple, so **you** can modify the code to your needs.

#### CImg

* Open-source
* Minimalistic, only include "CImg.h"
* Lot of features

See http://cimg.sourceforge.net/

### CONFIGURATION / CONTROL:

PSB is written to be operated from the command line. 
It will per default write to "./output" and "./log" in the ${PWD} from which it is invoked.

### COMMAND SYNTAX

Please refer to the manpage in our "doc" folder, by "man -l PSB.1",
or by running the compiled program with "--help".

### PROGRAM SHORTCUTS
Image previews:
* Arrow UP : next image
* Arrow DOWN : previous image
* Arrow LEFT : next solver (if any)
* Arrow RIGHT : previous solver (if any)

### tryPlot.py

The script that does the plotting.
It is currently not handling large inputs, so use with patience and little care.

### BUGS

Please report new issues to the github page https://github.com/andsild/PSB/ or the developer @ andsild@gmail.com

