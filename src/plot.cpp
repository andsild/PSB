#include "plot.hpp"

#include <string>

#include "CImg.h"
#include "file.hpp"

namespace plot
{

void plot()
{
    //TODO: fix path..
    std::string sPlotDir = "../plot.sh";
    #ifdef __APPLE__
    std::string exe = "open " + sPlotDir;
    cimg_library::cimg::system(exe.c_str());
    #elif __linux
    cimg_library::cimg::system(sPlotDir.c_str());
    #endif
}

} /* EndOfNamespace */
