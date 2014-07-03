#ifndef _PLOT
#define _PLOT   1

#include <stdlib.h>
#include <string>

namespace plot
{

void plot()
{
    //TODO: fix path..
    std::string sPlotDir = "../plot.sh";
    #ifdef __APPLE__
    std::string exe = "open " + sPlotDir;
    system(exe.c_str());
    #elif __linux
    system(sPlotDir.c_str());
    #endif
}

} //EndOfNamespace

#endif
