#ifndef _FILE_H
#define _FILE_H 1

#define PRECISION 20
#define DATA_DIR "./output/"
#define DATA_EXTENSION ".dat"
#define PROJECT_DIR "PSB"

#include <string>
#include <vector>
#include <iostream>
#include <iomanip>
#include <fstream>

#include <dirent.h>
#include <math.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define DEFAULT_MODE      S_IRWXU | S_IRGRP |  S_IXGRP | S_IROTH | S_IXOTH

namespace file_IO
{

class DirNotFound : public std::exception
{
    public:
        explicit DirNotFound(const std::string& message):
            msg_(message) {}

        virtual ~DirNotFound() throw (){}
        virtual const char* what() const throw();
    protected:
        std::string msg_;
};

void trimLeadingFileName(std::string &arg);
void trimTrailingFilename(std::string &arg);
void mkdirp(const char *arg, mode_t = DEFAULT_MODE);

void getFilesInFolder(std::string, std::vector<std::string> &arg);

template <typename t>
void writeToFile(const std::vector<t> vRes, std::string sFilename,
                 std::string sFolderDest)
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
            // LOG(severity_type::error)(sMsg);
            // CLOG(severity_type::error)(sMsg);
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

} /* EndOfNamespace */

#endif
