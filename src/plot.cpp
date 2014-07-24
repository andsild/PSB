#include "plot.hpp"

#include "CImg.h"

#include "image_types.hpp"
#include "file.hpp"

namespace plot
{

cimg_library::CImgDisplay plot(const image_fmt &graph)
{
    // //TODO: fix path..
    // std::string sPlotDir = "../plot.sh";
    // #ifdef __APPLE__
    // std::string exe = "open " + sPlotDir;
    // cimg_library::cimg::system(exe.c_str());
    // #elif __linux
    // cimg_library::cimg::system(sPlotDir.c_str());
    // #endif

    // image_fmt graph = file_IO::readData(false, true);
    cimg_library::CImgDisplay disp(graph,"graph.png : graph for all images in folder", 0, false, true) ;
    return disp;
}

} /* EndOfNamespace */
