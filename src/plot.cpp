#ifndef _PLOT
#define _PLOT   1

#include <stdlib.h>

#include "file.cpp"

namespace plot
{

void plot()
{
    string sPlotDir = file_IO::get_path() + "plot.sh";
    #ifdef __APPLE__
    string exe = "open " + sPlotDir;
    system(exe.c_str());
    #elif __linux
    system(sPlotDir.c_str());
    #endif
}

} //EndOfNamespace

#endif
