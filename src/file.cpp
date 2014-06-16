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

void writeToFile(vector<string> vRes, string sFilename, string sFolderDest)
{
    ofstream data_file(DATA_DIR + sFolderDest + "/" + sFilename
                       + DATA_EXTENSION);

    for (vector<string>::iterator it = vRes.begin();
            it != vRes.end();
            ++it)
    {
        data_file << setprecision(PRECISION) << fixed << *it << endl;
    }

    data_file.close();
}

}

#endif
