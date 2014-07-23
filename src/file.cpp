#include "file.hpp"

#include <errno.h>
#include <iomanip>
#include <iostream>
#include <stdio.h>
#include <fstream>
#include <string>
#include <vector>

#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "image_types.hpp"
#include "loginstance.hpp"

#include "image2.hpp" // just for printImage, debug

using namespace logging;

namespace file_IO
{

std::string getFoldername(const std::string sFilename)
{
    size_t found;
    found=sFilename.find_last_of("/\\");
    return sFilename.substr(0, found);
}


std::string getFilename(const std::string sFilename)
{
    size_t found;
    found=sFilename.find_last_of("/\\");
    return sFilename.substr(found + 1);
}

std::string SaveBehaviour::getSavename(
                                const std::string sName,
                                const std::string sLabel,
                                const bool bResolve) const
{
    std::string sFilename = getFilename(sName);
    std::string sResolve = (bResolve) ? this->sResolveTag : "";
    std::string sRet = this->sOutdir + sResolve + this->sDelimiter
                + sLabel + this->sDelimiter
                + sFilename + this->sSuffix;
    return sRet;
}

std::string SaveBehaviour::getLogname(
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

std::string SaveBehaviour::getDelimiter() { return this->sDelimiter; }
std::string SaveBehaviour::getValueDelimiter() { return this->sValueDelimiter; }


std::string SaveBehaviour::getResolveLabel(std::string sLabel)
{
    return std::string(this->sResolveTag + this->sDelimiter + sLabel);
}

void SaveBehaviour::getNames(std::string sSearch,
                            std::string &sOutdir, std::string &sLabel, std::string &sFilename,
                            bool &isResolved)
{
    std::string sCopy = sSearch;
    sOutdir = getFoldername(sSearch);
    size_t searchPos = sSearch.find_last_of("/\\");
    sSearch.erase(0, searchPos + 1);

    searchPos=sSearch.find(this->sDelimiter);
    isResolved = sSearch.substr(0,searchPos).length() > 1 == true;
    sSearch.erase(0, searchPos + sDelimiter.length());

    searchPos=sSearch.find(this->sDelimiter);
    sLabel = sSearch.substr(0, searchPos);
    sSearch.erase(0, searchPos + sDelimiter.length());

    searchPos=sSearch.find(this->sDelimiter);
    sFilename = sSearch.substr(0, searchPos);
    sSearch.erase(0, searchPos + sDelimiter.length());

    MLOG(severity_type::debug, "From ", sCopy , "\tOutdir : ",  sOutdir ,
                               "\tlabel: ", sLabel , "\tfilename: ", sFilename);
}

void splitHeader(const std::string sHeader, std::string &sLabel, std::string &sFilename)
{
    size_t pos;
    pos = sHeader.find_last_of(SAVE_PATTERN.getDelimiter());
    sLabel = sHeader.substr(0,pos);
    sFilename = sHeader.substr(pos + SAVE_PATTERN.getDelimiter().length(), sHeader.length());
}


image_fmt readData()
{
    const char *filename = DATA_OUTFILE;
    std::ifstream fin(filename, std::ios_base::binary|std::ios_base::in);
    image_fmt resGraph(500, 400, 1, 3, 0);
    const double blackWhite[] = {255, 255, 255};

    
    
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
    
        while ((start = sData.find(SAVE_PATTERN.getValueDelimiter(), start)) != std::string::npos)
        {
            vData.push_back(atof(sData.substr(0,start).c_str()));
            sData.erase(0,start);
            start += SAVE_PATTERN.getValueDelimiter().length();
        }
    
        double *dData = new double[vData.size()];
    
        for(int iPos = 0; iPos < vData.size(); iPos++)
        {
            dData[iPos] = vData[iPos];
        }
        image_fmt imgData(dData, vData.size(), 1, 1, 1, true);
    
        resGraph.draw_graph(imgData, blackWhite);
        // std::cerr << image_psb::printImage(resGraph) << std::endl;
    }
    
    resGraph.draw_axes(resGraph, resGraph, blackWhite);
    
    fin.close();
    return resGraph;
}


void writeData(const rawdata_fmt &vData, std::string sLabel, std::string sFilename)
{
    const char *filename = DATA_OUTFILE;
    std::ofstream fout(filename, std::ios_base::binary|std::ios_base::out|std::ios_base::app);

    std::string sDelim = SAVE_PATTERN.getDelimiter();
    fout << sLabel << sDelim << sFilename << std::endl;

    for(auto const item : vData)
        fout << item << SAVE_PATTERN.getValueDelimiter();
    fout << std::endl;
    fout.close();
}


void saveImage(const image_fmt &image, const std::string sSaveName)
{
    mkdirp(SAVE_PATTERN.getOutdir().c_str());
    try
    {
        image.save_ascii(sSaveName.c_str());
    }
    catch(cimg_library::CImgIOException &cioe)
    {
        MLOG(severity_type::error, cioe.what());
    }
}


const char* DirNotFound:: what() const throw() 
{
    std::string ret = std::string("Could not find folder: ") + msg_;
    return ret.c_str();
}

void trimLeadingFileName(std::string &str)
{
    size_t endpos = str.find_last_of("/");

    /* Ensure filename is valid */
    if( std::string::npos != endpos )
    {
        str = str.substr(endpos+1,str.size() );
    }
}

void trimTrailingFilename(std::string &str)
{
    size_t newpos = str.find_last_of(".");
    if( std::string::npos != newpos)
    {
        str = str.substr(0, newpos);
    }
}


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
            MLOG(severity_type::warning, "Skipping ", readFile, " : file invalid");
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
