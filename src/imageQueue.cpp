#include  "imageQueue.hpp"

#include "image2.hpp"
#include "solver.hpp"
#include "fft.hpp"
#include "wavelet.hpp"
#include "iterative_solvers.hpp"
#include "image_types.hpp"
using namespace image_util;

/** Add an iterative solver for an image.
  Since the iterative solvers are split into different parts before they
  are rejoined, they need this method to tag the different parts
  so that other methods can know how to recombine them.
*/
void addIterativeSolver(std::vector<solver::Solver*> &vIn,
        const double dTolerance,
        const std::string sFilename, const std::string sLabel,
        const iterative_func func,
        const std::vector<image_fmt *> &origList,
        const std::vector<image_fmt *> &rhoList,
        const std::vector<image_fmt *> &guessList)
{
    for(int iPos = 0; iPos < origList.size() - 1; iPos++)
    {
        vIn.push_back(new solver::IterativeSolver(origList[iPos],
                                            rhoList[iPos], guessList[iPos],
                                            func, dTolerance,
                                            sFilename, sLabel, true ));
    }
    /* Tag the last image with "isFinal" */
    vIn.push_back(new solver::IterativeSolver(origList.back(),
                                        rhoList.back(), guessList.back(),
                                        func, dTolerance,
                                        sFilename, sLabel, true, true));
}


void stageDirectSolvers(std::vector<solver::Solver*> &vSolvers,
        const image_fmt &img, const data_fmt fieldModifier,
        const double dNoise, const std::string sFilename,
        const bool dst, const bool dct,
        const bool wavelet_5x5, const bool wavelet_7x7, const bool multiwavelet)
{
    std::string sPrefix= "";
    image_fmt* const use_img = new image_fmt();
    if(dNoise != 0.0)
    {
        sPrefix += "noise" + std::to_string(dNoise);
        image_fmt tmp = img.get_noise(dNoise);
        tmp.normalize(0,255);
        *use_img = tmp;
        std::string sSavename = file_IO::SAVE_PATTERN.getSavename(
                                                    sFilename, sPrefix, false);
        file_IO::saveImage(tmp, sSavename, false);
    }
    else
    {
        *use_img = img;
    }
    sPrefix += "__";

    if(fieldModifier != 1.0)
    {
        sPrefix += "re";
    }
    sPrefix += "__";

    // const image_fmt* use_img  = &img;
    image_fmt* field = new image_fmt();
    image_fmt *neuMannField = new image_fmt();
    image_fmt neuMannBorder = image_util::padCore(img.width() + 2, img.height() + 2, img);

    image_util::makeField(*use_img, -1.0 * fieldModifier, *field);
    image_util::makeField(neuMannBorder, -1.0 * fieldModifier, *neuMannField);

    if(dst)
    {
        std::string sLabel = sPrefix + "dst";
        vSolvers.push_back(new solver::DirectSolver(use_img, field,
                                                    solver::FFT_DST,
                                                    sFilename, sLabel, true));
    }
    if(dct)
    {
        image_fmt *negNeuPtr = new image_fmt();
        image_fmt negNeumann = (*neuMannField) * -1;
        *negNeuPtr = negNeumann;
        std::string sLabel = sPrefix + "dct";
        vSolvers.push_back(new solver::DirectSolver(use_img, negNeuPtr,
                                                    solver::FFT_DCT,
                                                    sFilename, sLabel, false));
    }

    if(wavelet_5x5)
    {
        std::string sLabel = sPrefix + "wavelet5x5";
        vSolvers.push_back(new solver::DirectSolver(use_img, neuMannField,
                                                    wavelet::wavelet_5x5,
                                                    sFilename, sLabel, false));
    }
    if(wavelet_7x7)
    {
        std::string sLabel = sPrefix + "wavelet7x7";
        vSolvers.push_back(new solver::DirectSolver(use_img, neuMannField,
                                                    wavelet::wavelet_7x7,
                                                    sFilename, sLabel, false));
    }
    if(multiwavelet)
    {
        // std::string sLabel = sPrefix + "multiwavelet";
        // vSolvers.push_back(new solver::DirectSolver(use_img, negPtr,
        //                                             wavelet::hermite_wavelet,
        //                                             sFilename, sLabel, false));
    }
}

void stageIterativeSolvers(std::vector<solver::Solver*> &vSolvers,
        const image_fmt &img, const double dTolerance, const data_fmt fieldModifier,
        const double dNoise, const std::string sFilename,
        const bool gauss, const bool jacobi, const bool sor)
{
    std::string sPrefix= "";
    image_fmt* const use_img = new image_fmt();
    if(dNoise != 0.0)
    {
        std::cerr << "NOTHING TO DO HERE" << std::endl;
        sPrefix += "noise" + std::to_string(dNoise);
        image_fmt tmp = img.get_noise(dNoise, 3);/* Salt and pepper noise */
        tmp.normalize(0,255);
        *use_img = tmp;
        std::string sSavename = file_IO::SAVE_PATTERN.getSavename(sFilename, sPrefix, false);
        file_IO::saveImage(tmp, sSavename, false);
    }
    else
    {
        *use_img = img;
    }
    sPrefix += "__";

    if(fieldModifier != 1.0)
    {
        sPrefix += "re";
    }
    sPrefix += "__";

    image_fmt* field = new image_fmt;
    makeField(*use_img, fieldModifier, *field);

    // field->save("field.png");
    // exit(EXIT_FAILURE);

    std::vector<image_fmt *> origList, guessList, rhoList;
    image_util::divide(use_img, field, origList, rhoList, guessList);
    // guessList.back()->save("border.png");
    // exit(EXIT_FAILURE);

    if(gauss)
    {
        std::string sLabel = sPrefix + "gauss";
        addIterativeSolver(vSolvers, dTolerance, sFilename, sLabel,
                            solver::iterate_gauss, origList, rhoList, guessList);
    }
    if(jacobi)
    {
        std::string sLabel = sPrefix + "jacobi";
        addIterativeSolver(vSolvers, dTolerance, sFilename, sLabel,
                            solver::iterate_jacobi, origList, rhoList, guessList);
    }
    if(sor)
    {
        std::string sLabel = sPrefix + "sor";
        addIterativeSolver(vSolvers, dTolerance, sFilename, sLabel,
                            solver::iterate_sor, origList, rhoList, guessList);
    }
}

/** The main entry point for solvers to an image.

  For each solver(boolean), add the corresponding method and field,
  then execute solve().
*/
//TODO: there are so many booleans that I should use a struct for this.
void processImage(std::string sFilename, double dNoise, double dTolerance,
                  data_fmt resolve,
                  const bool gauss, const bool jacobi, const bool sor,
                  const bool dst, const bool dct,
                  const bool wavelet_5x5, const bool wavelet_7x7,
                  const bool multiwavelet)
{

    image_fmt use_img;
    std::vector<solver::Solver*> vSolvers;

    if(!image_util::readImage(use_img, sFilename))
    {
        std::cerr << "Error:: could not load image: " << sFilename << std::endl;
        return;
    }

    toGrayScale(use_img);
    stageDirectSolvers(vSolvers, use_img, 1.0, 0.0, sFilename, dst, dct,
                        wavelet_5x5, wavelet_7x7, multiwavelet);
    stageIterativeSolvers(vSolvers, use_img, dTolerance, 1.0, 0.0, sFilename,
                          gauss, jacobi, sor);
    if(dNoise != 0.0)
    {
        stageDirectSolvers(vSolvers, use_img, 1.0, dNoise, sFilename, dst, dct,
                            wavelet_5x5, wavelet_7x7, multiwavelet);
        stageIterativeSolvers(vSolvers, use_img, 1.0, dTolerance, dNoise, sFilename,
                          gauss, jacobi, sor);
    }
    if(resolve != 1.0)
    {
        stageDirectSolvers(vSolvers, use_img, resolve, 0.0, sFilename, dst, dct,
                            wavelet_5x5, wavelet_7x7, multiwavelet);
        stageIterativeSolvers(vSolvers, use_img, dTolerance, resolve, 0.0, sFilename,
                            gauss, jacobi, sor);
    }
    if(dNoise != 0.0 && resolve != 1.0)
    {
        stageDirectSolvers(vSolvers, use_img, resolve, dNoise, sFilename, dst, dct,
                            wavelet_5x5, wavelet_7x7, multiwavelet);
        stageIterativeSolvers(vSolvers, use_img, dTolerance, resolve, dNoise, sFilename,
                            gauss, jacobi, sor);
    }

    imageList_fmt accumulator; /*< container for subdivisions of solved image */
    rawdata_fmt vResults, vTimes;
    int iPartIndex = 0;

    for(auto it : vSolvers) // for each solver for each image (and its divisions)
    {
        image_fmt result = it->solve(vResults, vTimes); /*< result now holds the resulting image,
                                                          < vResults holds the imagediffs */
        /* Multipart images: solve each region before moving past this if block */
        if(it->isMultipart())
        {
            accumulator.push_back(result);
            /* We can now merge the regions together */
            if(it->isFinal())
            {
                result = image_util::joinImage(accumulator);
                accumulator.clear();
                iPartIndex = 0;
            }
            else
            {
                std::string sFilename = it->getFilename() + std::to_string(iPartIndex);
                file_IO::writeData(vResults, vTimes, it->getLabel(), sFilename);
                iPartIndex++;
                vResults.clear(); // important, otherwise it stacks results
                continue;
            }
        }

        /* Before saving the image, round the values so that the image can
           be viewed later */
        image_util::roundValues(result);
        std::string sSavename = file_IO::SAVE_PATTERN.getSavename(sFilename, it->getLabel(), false);
        file_IO::saveImage(result, sSavename, false);
        file_IO::writeData(vResults, vTimes, it->getLabel(), it->getFilename());
        /* Erase before re-iterating */
        vResults.erase(vResults.begin(), vResults.end());
        vTimes.erase(vTimes.begin(), vTimes.end());
        // DO_IF_LOGLEVEL(logging::severity_type::extensive)
        // {
        //     std::string sMsg = "Final image(cut)\n" + printImageAligned(result);
        //     it->log(1, sMsg);
        // }
    }
    vSolvers.back()->clear(); // XXX: this breaks if multiple solvers are invoked!!
}


