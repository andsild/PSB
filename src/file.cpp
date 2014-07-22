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

void writeHeader(std::string sFilename, std::string sLabel)
{
    const char *filename = DATA_OUTFILE;
    std::ofstream fout(filename, std::ios_base::binary|std::ios_base::out|std::ios_base::app);
    fout.close();
}

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

void writeData(const std::vector<double> &vData, std::string sLabel, std::string sFilename)
{
    const char *filename = DATA_OUTFILE;
    std::ofstream fout(filename, std::ios_base::binary|std::ios_base::out|std::ios_base::app);

    std::string sDelim = SAVE_PATTERN.getDelimiter();
    fout << sLabel << sDelim << sFilename << std::endl;

    for(auto const item : vData)
        fout << item << " ";
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
