#ifndef _FILE_H
#define _FILE_H 1

#define PRECISION 20
#define DATA_DIR "./output/"
#define DATA_EXTENSION ".dat"
#define PROJECT_DIR "PSB"

#include <string>
#include <vector>
#include <iomanip>
#include <fstream>

#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "image_types.hpp"

#define DEFAULT_MODE      S_IRWXU | S_IRGRP |  S_IXGRP | S_IROTH | S_IXOTH

namespace file_IO
{

void saveImage(const image_fmt &arg, std::string);

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

class SaveBehaviour
{
    private:
        const std::string sDelimiter,
                    sResolveTag,
                    sSuffix,
                    sOutdir;
    public:
        SaveBehaviour(const std::string sDelim,
                    const std::string sResolve,
                    const std::string sSuff)
            : sDelimiter(sDelim), sResolveTag(sResolve), sSuffix(sSuff), sOutdir(DATA_DIR)
        {
        }
        std::string getSavename(const std::string,
                              const std::string, const bool) const;
        void getNames(const std::string, std::string &arg1, std::string &arg2,
                    std::string &arg3, bool &arg4);
        std::string getResolveLabel(std::string arg);
        std::string getOutdir() { return this->sOutdir; }
};


static SaveBehaviour SAVE_PATTERN("__", "re", "");

void trimLeadingFileName(std::string &arg);
void trimTrailingFilename(std::string &arg);
void mkdirp(const char *arg, mode_t = DEFAULT_MODE);

void getFilesInFolder(std::string, std::vector<std::string> &arg);
std::vector<std::string> getFilesInFolder2(std::string);
std::string getFoldername(const std::string sFilename);
std::string getFilename(const std::string sFilename);

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
