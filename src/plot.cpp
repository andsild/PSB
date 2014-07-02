#ifndef _PLOT
#define _PLOT   1

#include <stdlib.h>

#include "file.hpp"

namespace plot
{

void plot()
{
    //TODO: fix path..
    string sPlotDir = "../plot.sh";
    #ifdef __APPLE__
    string exe = "open " + sPlotDir;
    system(exe.c_str());
    #elif __linux
    system(sPlotDir.c_str());
    #endif
}

} //EndOfNamespace

#endif
