#ifndef _FILE_CPP
#define _FILE_CPP 1

//TODO: check for diagonal dominance
//If you know that a matrix is diagonally dominant,
//
#include <iostream>
#include <cmath>
#include <algorithm>
#include <vector>
#include <stdio.h>
#include <cmath>
#include <string>

#include "./main.cpp"

using namespace std;


namespace file_IO
{

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


//TODO: does not warn of IO-error
void writeToFile(vector<string> vRes, string sFilename, string sFolderDest)
{
    trimLeadingFileName(sFilename);
    trimTrailingFilename(sFilename);
    string sFinalname = DATA_DIR + sFolderDest + "/" + sFilename 
                        + DATA_EXTENSION;

    ofstream data_file(sFinalname.c_str(),  ios::out);

    for (vector<string>::iterator it = vRes.begin();
            it != vRes.end();
            ++it)
    {
        data_file << setprecision(PRECISION) << fixed << *it << endl;
    }

    data_file.close();
}

vector<string> getFilesInFolder(char *dir)
{
    ifstream fin;
    int num;
    DIR *dp;
    struct stat filestat;
    struct dirent *dirp;

    dp = opendir( dir);
    if (dp == NULL)
    {
        cout << "Error in openin" << endl;
        cout << dir << endl;
        throw;
    }

    vector<string> filenames;

    while ((dirp = readdir( dp )))
    {
        string readFile = string(dir) + string(dirp->d_name);

        // Check for valid file(s)
        if (stat( readFile.c_str(), &filestat )) continue;
        if (S_ISDIR( filestat.st_mode ))         continue;

        filenames.push_back(readFile);
    }
    return filenames;
}

}

#endif
