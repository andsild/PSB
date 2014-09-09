/** Small functions and utilities for I/O action
  Ideally, this file should comprise all methods that needs to do I/O
*/
#ifndef _FILE_H
#define _FILE_H

#define PRECISION 20
#define DATA_DIR "./output/"
#define DATA_OUTFILE "./output.txt"
#define DATA_EXTENSION ".dat"
#define PROJECT_DIR "PSB"

#include <iostream>
#include <string>
#include <vector>
#include <iomanip>
#include <fstream>

#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "image_types.hpp"
#include "loginstance.hpp"

#define DEFAULT_MODE      S_IRWXU | S_IRGRP |  S_IXGRP | S_IROTH | S_IXOTH

namespace file_IO
{

/** Save an image to a given dest.

  @param image is the CImg<T> instance to save
  @param sSavename is the absolute or relative filename the file will be saved to
  @param bSaveAscii says whether or not to save the image as ascii. Note that
            ascii-saving will not round down the values, whereas other formats
            will.
*/
void saveImage(const image_fmt &arg, const std::string, const bool);
/** Write results from a solver to a file

  @param vData is the vector of dirichlet energies
  @param vTimes is the vector of execution times for each iteration
  @param sLabel is the header that is written for each row of results
  @param sFilename is the filename to write to.

  @note vData and vTimes is assumed to be the same lenght
*/
void writeData(const rawdata_fmt &arg1, const rawdata_fmt &arg2, std::string, std::string);
image_fmt readData(const bool, const bool);

/** Exception class for file I/O
  The most important feature of this class is the name; it has no special
  features.
*/
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

/** Class that standardizes how filenames are saved.
  Since the class knows how to save, it can also load all the data.
  This class is made to ensure that all methods use I/O the same way,
  and thus making small changes to I/O should not require a lot of changes
  to the code elsewhere.
*/
class SaveBehaviour
{
    private:
        const std::string sDelimiter,
                    sValueDelimiter,
                    sResolveTag,
                    sLogDir,
                    sLogExtension,
                    sSuffix,
                    sOutdir;
    public:
        SaveBehaviour(const std::string sDelim,
                     const std::string sValueDelim,
                    const std::string sResolve,
                    const std::string sSuff)
            : sDelimiter(sDelim), sResolveTag(sResolve), sSuffix(sSuff),
             sValueDelimiter(sValueDelim),
            sOutdir(DATA_DIR), sLogDir(LOG_DIR), sLogExtension(".log")
        {
        }
        const std::string getSavename(const std::string,
                              const std::string, const bool) const;
        const std::string getLogname(const std::string,
                              const std::string, const bool) const;
        const void getNames(const std::string, std::string &arg1, std::string &arg2,
                    std::string &arg3, bool &arg4, bool &arg5, bool &arg6);
        const std::string getResolveLabel(std::string arg);
        const std::string getOutdir() { return this->sOutdir; }
        const std::string getDelimiter();
        const std::string getValueDelimiter();
};

/** The global instance that all other methods can use for I/O.
*/
static SaveBehaviour SAVE_PATTERN("__", "  ", "re", "");

/** @param str is a filename of type "folder/name".
        It is modified to become of type "name" (trim leading)
*/
void trimLeadingFileName(std::string &arg);
/** @param str is a filename of type "folder/name".
        It is modified to become of type "folder" (trim trailing)
*/
void trimTrailingFilename(std::string &arg);
/** Create a directory, if it does not not already exist

  @param path is the filename of the directory
  @param mode decides the behaviour. @see unistd.h for more info. This parameter
    can usually be ignored.
  */
void mkdirp(const char *arg, mode_t = DEFAULT_MODE);

/** Read a directory and it's files.
  
  @param sDir is the directory to read
  @return is a vector of strings with files found in sDir
*/
std::vector<std::string> getFilesInFolder(std::string);
/** Split a string into "path/file"

  @param sFilename is the string to be split and returned.
  @return is the path of sFilename
*/
std::string getFoldername(const std::string sFilename);
/** Split a string into "path/file"

  @param sFilename is the string to be split and returned.
  @return is the path of sFilename
*/
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
            std::cerr << sMsg << std::endl;
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
