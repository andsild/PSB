#ifndef _FILE_CPP
#define _FILE_CPP 1

#define PROJECT_DIR "PSB"
#define DEFAULT_MODE      S_IRWXU | S_IRGRP |  S_IXGRP | S_IROTH | S_IXOTH

#include <algorithm>
#include <cmath>
#include <errno.h>
#include <iostream>
#include <stdio.h>
#include <string>
#include <sys/stat.h>
#include <vector>

#include "./main.cpp"

using namespace std;

namespace file_IO
{

class DirNotFound : public exception
{
    public:
        explicit DirNotFound(const std::string& message):
            msg_(message)
         {}

        virtual ~DirNotFound() throw (){}
        virtual const char* what() const throw()
        {
            string ret = string("Could not find folder: ") + msg_;
            return ret.c_str();
        }
    protected:
        std::string msg_;
};

void trimLeadingFileName(string &str)
{
    size_t endpos = str.find_last_of("/");

    /* Ensure filename is valid */
    if( string::npos != endpos )
    {
        str = str.substr(endpos+1,str.size() );
    }
}

void trimTrailingFilename(string &str)
{
    size_t newpos = str.find_last_of(".");
    if( string::npos != newpos)
    {
        str = str.substr(0, newpos);
    }
}


void mkdirp(const char* path, mode_t mode = DEFAULT_MODE) {
  // const cast for hack
  char* p = const_cast<char*>(path);

  // Do mkdir for each slash until end of string or error
  while (*p != '\0') {
    // Skip first character
    p++;

    // Find first slash or end
    while(*p != '\0' && *p != '/') p++;

    // Remember value from p
    char v = *p;

    // Write end of string at p
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
void writeToFile(const vector<t> vRes, string sFilename, string sFolderDest)
{
    trimLeadingFileName(sFilename);
    trimTrailingFilename(sFilename);
    string sFileDir = DATA_DIR + sFolderDest + "/";
    sFilename = sFileDir + sFilename + DATA_EXTENSION;

    ofstream data_file;

    data_file.open(sFilename.c_str(), ios::out);
    if(!data_file)
    {
        mkdirp(sFileDir.c_str());
        data_file.open(sFilename.c_str(), ios::out);
        if(!data_file)
        {
            cout << "Unable to write to file: " << sFilename << endl;
            return;
        }
    }

    typename vector<t>::const_iterator it;
    for (it = vRes.begin(); it != vRes.end(); ++it)
    {
        data_file << setprecision(PRECISION) << fixed << *it << endl;
    }

    data_file.close();
}

vector<string> getFilesInFolder(string sDir)
{
    ifstream fin;
    int num;
    DIR *dp;
    struct stat filestat;
    struct dirent *dirp;

    dp = opendir(sDir.c_str());
    if (dp == NULL)
    {
        throw DirNotFound(sDir);
    }

    vector<string> filenames;

    while ((dirp = readdir( dp )))
    {
        string readFile = sDir + "/" + string(dirp->d_name);

        // Check for valid file(s)
        if (stat( readFile.c_str(), &filestat ))
        {
            cout << "Skipping " << readFile << " : file invalid" << endl;
        }
        if (S_ISDIR( filestat.st_mode ))
        {
            continue;
        }

        filenames.push_back(readFile);
    }
    return filenames;
}

}

#endif
