/** Small functions and utilities for I/O action

*/

#include "file.hpp"

#include <algorithm>
#include <errno.h>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <stdio.h>
#include <sstream>
#include <string>
#include <vector>

#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "image_types.hpp"
#include "imageUtil.hpp"
#include "loginstance.hpp"


using namespace logging;

namespace file_IO
{

/** Split a string into "path/file"

  @param sFilename is the string to be split and returned.
  @return is the path of sFilename
*/
std::string getFoldername(const std::string sFilename)
{
    size_t found;
    found=sFilename.find_last_of("/\\");
    return sFilename.substr(0, found);
}

/** Split a string into "path/file"

  @param sFilename is the string to be split and returned.
  @return is the path of sFilename
*/
std::string getFilename(const std::string sFilename)
{
    size_t found;
    found=sFilename.find_last_of("/\\");
    return sFilename.substr(found + 1);
}

const std::string SaveBehaviour::getSavename( const std::string sName,
                                const std::string sLabel, const bool bResolve)
    const
{
    std::string sFilename = getFilename(sName);
    std::string sRet = this->sOutdir 
                + sLabel + this->sDelimiter
                + sFilename + this->sSuffix;
    return sRet;
}

const std::string SaveBehaviour::getLogname(
                                const std::string sName,
                                const std::string sLabel,
                                const bool bResolve) const
{
    std::string sFilename = getFilename(sName);
    std::string sResolve = (bResolve) ? this->sResolveTag : "";
    std::string sRet = this->sLogDir + sResolve + this->sDelimiter
                + sLabel + this->sDelimiter
                + sFilename + this->sLogExtension;
    return sRet;
}

const std::string SaveBehaviour::getDelimiter() { return this->sDelimiter; }
const std::string SaveBehaviour::getValueDelimiter() { return this->sValueDelimiter; }


const std::string SaveBehaviour::getResolveLabel(std::string sLabel)
{
    return std::string(this->sResolveTag + this->sDelimiter + sLabel);
}

const void SaveBehaviour::getNames(std::string sSearch, std::string &sOutdir,
                                    std::string &sLabel, std::string &sFilename,
                            bool &isResolved, bool &isNoised, bool &isNoisedOrig)
{
    std::string sCopy = sSearch;
    sOutdir = getFoldername(sSearch);
    size_t searchPos = sSearch.find_last_of("/\\");
    sSearch.erase(0, searchPos + 1);

    int count = 0;
    for (size_t offset = sSearch.find(sDelimiter); offset != std::string::npos;
            offset = sSearch.find(sDelimiter, offset + sDelimiter.length()))
    {
        ++count;
    }
    if(count < 2)
    {
        isNoisedOrig = true;
        sSearch.erase(0, sDelimiter.length());
    }
    else
    {
        searchPos=sSearch.find(this->sDelimiter);
        sLabel = sSearch.substr(0, searchPos);
        isNoised = sSearch.substr(0,searchPos).length() > 1 == true;
        sSearch.erase(0, searchPos + sDelimiter.length());

        searchPos=sSearch.find(this->sDelimiter);
        isResolved = sSearch.substr(0,searchPos).length() > 1 == true;
        sSearch.erase(0, searchPos + sDelimiter.length());
    }

    searchPos=sSearch.find(this->sDelimiter);
    sLabel = sSearch.substr(0, searchPos);
    sSearch.erase(0, searchPos + sDelimiter.length());

    searchPos=sSearch.find(this->sDelimiter);
    sFilename = sSearch.substr(0, searchPos);
    sSearch.erase(0, searchPos + sDelimiter.length());

    // MLOG(severity_type::debug, "From ", sCopy , "\tOutdir : ",  sOutdir ,
    //                            "\tlabel: ", sLabel , "\tfilename: ", sFilename);
}

void splitHeader(const std::string sHeader, std::string &sLabel, std::string &sFilename)
{
    size_t pos;
    pos = sHeader.find(SAVE_PATTERN.getDelimiter(), 0);
    sLabel = sHeader.substr(0,pos);
    sFilename = sHeader.substr(pos + SAVE_PATTERN.getDelimiter().length(), sHeader.length());
}


image_fmt getAxis(const int iPixelSpace, const int iFontHeight, const int iHighVal, const int iLowVal)
{
    int iStepCount = (iPixelSpace / (iFontHeight + 35));
    double *dNums = new double[iStepCount];
    int iInc = (iHighVal - 0) / iStepCount;

    dNums[0] = iLowVal;
    for(int iPos = 1; iPos < iStepCount; iPos ++)
    {
        dNums[iPos] = dNums[iPos - 1] + iInc;
    }
    dNums[iStepCount-1]=iHighVal;

    image_fmt ret(dNums, 1, iStepCount, 1, 1, false);
    delete dNums;
    return ret;
}

const double *getColor(std::string sLabel, bool isAverage = false)
{
    static const double white[] = {255,255,255};
    if(sLabel.compare("gauss") == 0)
    {
        static const double red[] = {255,0,0},
                            lightRed[] = {100, 0, 0};
        if(isAverage)
            return red;
        return lightRed;
    }
    if(sLabel.compare("jacobi") == 0)
    {
        static const double yellow[] = {255,255,0},
                            lightYellow[] = {100, 100, 0};
        if(isAverage)
            return yellow;
        return lightYellow;
    }
    if(sLabel.compare("sor") == 0)
    {
        static const double green[] = {0,255,0},
                            lightGreen[] = {0, 100,0};
        if(isAverage)
            return green;
        return lightGreen;
    }

    return white;
}

void readProperties(std::ifstream &fin, int &iLongestLine,
                    double &dSmallestVal, double &dLargestVal)
{
    while(true) // until EOF
    {
        std::string sHeader, sData;
        std::getline(fin, sHeader);
        std::getline(fin, sData);
        if(fin.eof())
            break;

        std::string sLabel, sFilename;
        splitHeader(sHeader, sLabel, sFilename);
    
        std::string::size_type start = 0;
        rawdata_fmt vData;

        std::istringstream iss(sData);
        double dTmp = 0;
        int iCount = 0;
        
        while(iss >> dTmp)
        {
            iCount++;
            dLargestVal = (dLargestVal > dTmp) ? dLargestVal : dTmp;
            dSmallestVal = (dSmallestVal < dTmp) ? dSmallestVal : dTmp;
        }
        iLongestLine = (iLongestLine > iCount) ? iLongestLine : iCount;
    }

    fin.clear();
    fin.seekg(0, std::ios::beg);
}

image_fmt readData(const bool doAverage, const bool doPlot)
{
    const char *filename = DATA_OUTFILE;
    image_fmt resGraph(500, 400, 1, 3, 0);
    const double blackWhite[] = {255, 255, 255},
                red[] = {255, 0, 0},
                 green[] = {0, 255, 0};
    double yMin = 0, dMaxVal = -1, dSmallestVal = 1E+33;
    int iLongestLine = -1;
    std::map<std::string, std::vector< rawdata_fmt> > mapRes;
    std::ifstream fin(filename, std::ios_base::binary|std::ios_base::in);

    if(fin.good() == false)
    {
        std::string sAction = "";
        if(doAverage) sAction = "calculate average";
        if(doAverage && doPlot) sAction += " and ";
        if(doPlot) sAction += "plot";
        std::cerr << "No output files found when trying to " << sAction
                  << ". Did you remember to run the " "solver first?"
                  << std::endl;
        exit(EXIT_FAILURE);
    }

    readProperties(fin, iLongestLine, dSmallestVal, dMaxVal);

    while(true) // until EOF
    {
        std::string sHeader, sData, sLabel, sFilename;
        rawdata_fmt vData;

        std::getline(fin, sHeader);
        std::getline(fin, sData);
        if(fin.eof())
            break;

        splitHeader(sHeader, sLabel, sFilename);
       
        std::istringstream iss(sData);
        double dtmp = 0;
        while(iss >> dtmp)
            vData.push_back(dtmp);

        image_fmt imgData = image_util::vectorToImage(vData);
        image_fmt padded = image_util::padImage(imgData, iLongestLine);
        yMin = imgData.min();

        const double *color = getColor(sLabel);
        resGraph.draw_graph(padded, color, 1, 1, 1, dSmallestVal, dMaxVal);
        // delete color;

        if(mapRes.find(sLabel) == mapRes.end())
        {
            std::vector<rawdata_fmt> vIn(1, vData);
            mapRes.insert(std::pair<std::string, std::vector<rawdata_fmt> >(sLabel, vIn));
        }
        else
        {
            mapRes[sLabel].push_back(vData);
        }

    }

    if(doAverage)
    {
        for(auto it = mapRes.begin(); it != mapRes.end(); it++)
        {
            std::string sWriteHeader = it->first;
            rawdata_fmt vRes = image_util::averageResult(it->second);
            rawdata_fmt vTimes(1,1);

            writeData(vRes, vTimes, sWriteHeader, "average");
    
            if(doPlot)
            {
                // padding does something bad :(
                image_fmt imgData = image_util::vectorToImage(vRes);
                yMin = imgData.min();
                std::cerr << image_util::printImage(imgData) << std::endl;
                std::cerr << yMin << std::endl;
                image_fmt padded = image_util::padImage(imgData, iLongestLine);
                std::cerr << image_util::printImage(padded) << std::endl;

                const double *color = getColor(it->first, true);
                resGraph.draw_graph(padded, color, 1, 1, 1, dSmallestVal, dMaxVal);
            }
        }
    }
    
    const int iFontSize = 13;
    image_fmt xAxis = getAxis(resGraph.width(), iFontSize, iLongestLine, 0),
              yAxis = getAxis(resGraph.height(),iFontSize, dMaxVal, dSmallestVal);
    yAxis.mirror('y');
    resGraph.mirror('y');
    resGraph.draw_axis(0, yAxis, blackWhite);
    resGraph.draw_axis(xAxis, resGraph.height(), blackWhite);
    
    fin.close();
    return resGraph;
}

void trimFilename(std::string &s)
{
    if(s.back() == '0' || s.back() == '1' || s.back() == '2')
    {
        s.pop_back();
    }
    auto it = std::remove_if(std::begin(s),std::end(s),[](char c){return (c == '.');});
    s.erase(it, std::end(s));
    it = std::remove_if(std::begin(s),std::end(s),[](char c){return (c == '/');});
    s.erase(it, std::end(s));
}

/** Write results from a solver to a file

  @param vData is the vector of dirichlet energies
  @param vTimes is the vector of execution times for each iteration
  @param sLabel is the header that is written for each row of results
  @param sFilename is the filename to write to.

  @note vData and vTimes is assumed to be the same lenght
*/
void writeData(const rawdata_fmt &vData, const rawdata_fmt &vTimes,
                std::string sLabel, std::string sFilename)
{
    const char *filename = DATA_OUTFILE;
    trimFilename(sFilename);
    std::string sFile = "./out" + sLabel + "__" + sFilename + ".dat";
    filename = sFile.c_str();
    std::ofstream fout(filename, std::ios_base::binary|std::ios_base::out|std::ios_base::app);

    std::string sDelim = SAVE_PATTERN.getDelimiter();
    fout << sLabel << sDelim << sFilename << std::endl;

    for(auto const item : vData)
        fout << item << SAVE_PATTERN.getValueDelimiter();
    fout << std::endl;
    for(auto const item : vTimes)
        fout << item << SAVE_PATTERN.getValueDelimiter();
    fout << std::endl;
    fout.close();
}


/** Save an image to a given dest.

  @param image is the CImg<T> instance to save
  @param sSavename is the absolute or relative filename the file will be saved to
  @param bSaveAscii says whether or not to save the image as ascii. Note that
            ascii-saving will not round down the values, whereas other formats
            will.
*/
void saveImage(const image_fmt &image, const std::string sSavename, const bool bSaveAscii)
{
    mkdirp(SAVE_PATTERN.getOutdir().c_str());
    bool isResolved = false;
    std::string _, __, ___; bool ____, _1;
    std::string tmp = sSavename;
    SAVE_PATTERN.getNames(tmp, _, __, ___, isResolved, ____, _1);
    try
    {
        if(isResolved){
            image.save_ascii(sSavename.c_str()); // Needs to be ascii to get good plotting data from imageedit.cpp
        }
        else{
            image.save(sSavename.c_str());
        }
    }
    /* Typically, if the filename is invalid */
    catch(cimg_library::CImgIOException &cioe)
    {
        std::cerr << cioe.what() << std::endl;
    }
}


/** The default handler for when a filename for a directory is not find
*/
const char* DirNotFound:: what() const throw() 
{
    std::string ret = std::string("Could not find folder: ") + msg_;
    return ret.c_str();
}

/** @param str is a filename of type "folder/name".
        It is modified to become of type "name" (trim leading)
*/
void trimLeadingFileName(std::string &str)
{
    size_t endpos = str.find_last_of("/");

    /* Ensure filename is valid */
    if( std::string::npos != endpos )
    {
        str = str.substr(endpos+1,str.size() );
    }
}

/** @param str is a filename of type "folder/name".
        It is modified to become of type "folder" (trim trailing)
*/
void trimTrailingFilename(std::string &str)
{
    size_t newpos = str.find_last_of(".");
    if( std::string::npos != newpos)
    {
        str = str.substr(0, newpos);
    }
}

/** Create a directory, if it does not not already exist

  @param path is the filename of the directory
  @param mode decides the behaviour. @see unistd.h for more info. This parameter
    can usually be ignored.
  */
void mkdirp(const char* path, mode_t mode) {
  // const cast for hack
  char* p = const_cast<char*>(path);

  // Do mkdir for each slash until end of std::string or error
  while (*p != '\0') {
    // Skip first character
    p++;
    // Find first slash or end
    while(*p != '\0' && *p != '/') p++;
    // Remember value from p
    char v = *p;
    // Write end of std::string at p
    *p = '\0';
    // Create folder from path to '\0' inserted at p
    if(mkdir(path, mode) == -1 && errno != EEXIST) {
      *p = v;
      return;
    }
    *p = v;
  }
}

/** Read a directory and it's files.
  
  @param sDir is the directory to read
  @return is a vector of strings with files found in sDir
*/
std::vector<std::string> getFilesInFolder(std::string sDir)
{
    std::ifstream fin;
    int num;
    DIR *dp;
    struct stat filestat;
    struct dirent *dirp;
    std::vector<std::string> vRet;

    dp = opendir(sDir.c_str());
    if (dp == NULL)
    {
        throw DirNotFound(sDir);
    }

    while ((dirp = readdir( dp )))
    {
        std::string readFile = sDir + "/" + std::string(dirp->d_name);

        // Check for valid file(s)
        if (stat( readFile.c_str(), &filestat ))
        {
            // MLOG(severity_type::warning, "Skipping ", readFile, " : file invalid");
        }
        if (S_ISDIR( filestat.st_mode ))
        {
            continue;
        }
        vRet.push_back(readFile);
    }

    return vRet;
}

} /* EndOfNamespace */
