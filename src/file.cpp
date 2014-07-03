#include "file.hpp"


#include <algorithm>
#include <errno.h>
#include <iomanip>
#include <stdio.h>
#include <string>
#include <sys/stat.h>
#include <vector>


#include <dirent.h>
#include <math.h>
#include <sys/types.h>
#include <unistd.h>

#include "./logger.hpp"

using namespace logging;

namespace file_IO
{

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
    // Restore path to it's former glory
    *p = v;
  }
}

template <typename t>
void writeToFile(const std::vector<t> vRes, std::string sFilename, std::string sFolderDest)
{
    trimLeadingFileName(sFilename);
    trimTrailingFilename(sFilename);
    std::string sFileDir = DATA_DIR + sFolderDest + "/";
    sFilename = sFileDir + sFilename + DATA_EXTENSION;

    std::ofstream data_file;

    data_file.open(sFilename.c_str(), std::ios::out);
    if(!data_file)
    {
        mkdirp(sFileDir.c_str());
        data_file.open(sFilename.c_str(), std::ios::out);
        if(!data_file)
        {
            std::string sMsg = "Unable to write to file: " + sFilename;
            LOG(severity_type::error)(sMsg);
            CLOG(severity_type::error)(sMsg);
            return;
        }
    }

    typename std::vector<t>::const_iterator it;
    for (it = vRes.begin(); it != vRes.end(); ++it)
    {
        data_file << std::setprecision(PRECISION) << std::fixed << *it << std::endl;
    }

    data_file.close();
}

void getFilesInFolder(std::string sDir, std::vector<std::string> &output)
{
    std::ifstream fin;
    int num;
    DIR *dp;
    struct stat filestat;
    struct dirent *dirp;

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
            LOG(severity_type::warning)("Skipping ", readFile, " : file invalid");
            CLOG(severity_type::warning)("Skipping ", readFile, " : file invalid");
        }
        if (S_ISDIR( filestat.st_mode ))
        {
            continue;
        }
        output.push_back(readFile);
    }
}

} /* EndOfNamespace */
