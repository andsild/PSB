#ifndef _FILE_CPP
#define _FILE_CPP 1

#define PROJECT_DIR "PSB"

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


string get_path()
{
    char cwd[1024];
    getcwd(cwd, sizeof(cwd));
    string sCwd = string(cwd);

    size_t project_index = sCwd.find(PROJECT_DIR);
    if (project_index!=string::npos)
    {
        size_t slash_index = sCwd.find_first_of("/", project_index);
        if (slash_index!=string::npos)
            return sCwd.substr(0, slash_index + 1);  //include slash
    }
    else {
        return string("./");
    }
}



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
    string sFinalname = get_path() + DATA_DIR + sFolderDest + "/"
                        + sFilename + DATA_EXTENSION;

    ofstream data_file(sFinalname.c_str(),  ios::out);

    for (vector<string>::iterator it = vRes.begin();
            it != vRes.end();
            ++it)
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

    string sFullDir = get_path() + sDir;

    dp = opendir(sFullDir.c_str());
    if (dp == NULL)
    {
        cout << "Error in openin" << endl;
        cout << sFullDir <<  endl;
        throw;
    }

    vector<string> filenames;

    while ((dirp = readdir( dp )))
    {
        string readFile = sFullDir + string(dirp->d_name);

        // Check for valid file(s)
        if (stat( readFile.c_str(), &filestat )) continue;
        if (S_ISDIR( filestat.st_mode ))         continue;

        filenames.push_back(readFile);
    }
    return filenames;
}

}

#endif
