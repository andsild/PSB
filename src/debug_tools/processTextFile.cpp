#include <vector>
#include <iostream>
#include <fstream>
#include <string>


#include <dirent.h>
#include <math.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define DEFAULT_MODE      S_IRWXU | S_IRGRP |  S_IXGRP | S_IROTH | S_IXOTH


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

const char* DirNotFound:: what() const throw() 
{
    std::string ret = std::string("Could not find folder: ") + msg_;
    return ret.c_str();
}


void skipLine(std::ifstream &fin)
{
    char cRead = '\n';
    for(fin.get(cRead); cRead != '\n'; fin.get(cRead))
    {}
}

bool validChar(int iChar)
{
    /* regex match for '[0-9.e-]' */
    return ((iChar > 44 && iChar < 58) || iChar == 101);
}

int findMaxCols(std::ifstream &fin, double &dMSE)
{
    char cRead = '\n';
    int iMaxCol = 0;
    int iChar = 0;
    std::string sNum = "";
    double dRowSum = 0;
    int iRowCount = 0;
    dMSE = 0;

    while(fin.good() && fin.peek() != EOF)
    {
        int iCols = 0;
        bool bChain = false;
        double dTmp = 0;

        skipLine(fin);

        for(fin.get(cRead); cRead != '\n'; fin.get(cRead))
        {
            iChar = (int)cRead;
            /* If digit char or '.' or ',' */
            if(validChar(iChar))
            {
                bChain = false;
                sNum += cRead;
            }
            else
            {
                if(bChain) continue;
                // std::cerr << "as num: " << sNum << std::endl;
                dTmp = std::stod(sNum);
                dRowSum += pow(dTmp, 2);
                sNum = "";
                iCols++;
                bChain = true;
            }
        }
        iRowCount++;
        iMaxCol = (iCols > iMaxCol) ? iCols : iMaxCol;
        dMSE += sqrt(dRowSum);
        dRowSum = 0;
    }
    dMSE = dMSE / (double)iRowCount;

    fin.clear();
    fin.seekg(0, std::ios::beg);
    return iMaxCol;
}

std::string retFilename(std::string sOutdir, std::string sStart, double dMSE, int iCols)
{
    std::string ret = "file1";
    size_t pos;

    pos=sStart.find_last_of("/\\");
    sStart.erase(0, pos);

    pos=sStart.find_first_of("out");
    sStart.erase(0, pos + 3);

    pos=sStart.find_first_of(".");
    sStart.erase(pos, sStart.length() - 1);

    sStart = sOutdir + sStart + "_" + std::to_string(iCols) 
             + "_" + std::to_string(dMSE) + ".dat";
    
    return sStart;
}

void padX(std::ifstream &fin, const int iPadNum, std::string sNewname)
{
    char cRead = '\n';
    int iMaxCol = 0;
    int iChar = 0;

    std::ofstream fout(sNewname, std::ios_base::out|std::ios_base::ate|std::ios_base::trunc);

    while(fin.good() && fin.peek() != EOF)
    {
        int iCols = 1;
        bool bChain = false;

        for(fin.get(cRead); cRead != '\n'; fin.get(cRead))
        {
            // fout << cRead;
        }

        // fout << '\n';

        for(fin.get(cRead); cRead != '\n'; fin.get(cRead))
        {
            fout << cRead;
            iChar = (int)cRead;
            /* If digit char or '.' or ',' */
            if(validChar(iChar))
            {
                bChain = false;
            }
            else
            {
                if(bChain) continue;
                iCols++;
                bChain = true;
            }
        }

        fout << ' ';

        while(iCols < iPadNum + 1)
        {
            fout << "X ";
            iCols++;
        }

        fout << '\n';
    }

    fout.close();
}


void mkdirp(const char* path, mode_t mode = DEFAULT_MODE) {
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
            // MLOG(severity_type::warning, "Skipping ", readFile, " : file invalid");
        }
        if (S_ISDIR( filestat.st_mode ))
        {
            continue;
        }
        vRet.push_back(readFile);
    }

    return vRet;
}



int main(int argc, char **argv)
{
    std::string sOutdir = "out/";
    std::string sFilename = "./emptyARG.dat";
    mkdirp(sOutdir.c_str());

    if(argc == 3)
    {
        try
        {
            sOutdir = argv[1];
            sFilename = argv[2];
        }
        catch(...)
        {
            std::cerr << "error" << std::endl;
            exit(EXIT_FAILURE);
        }
    }
    else
    {
            std::cerr << "bad argc" << std::endl;
            exit(EXIT_FAILURE);
    }

    std::vector<std::string> vFiles = getFilesInFolder(sFilename);
    std::string sTest;
    std::cout << "Folder output: " << sOutdir << ", folder input: " << sFilename
              << "\n Continue? (press enter) ";
    getline(std::cin, sTest);

    for(auto it : vFiles)
    {
        std::ifstream fin(it.c_str(), std::ios_base::binary|std::ios_base::in);

        double dMSE = 0;
        int iCols = findMaxCols(fin, dMSE);

        std::string sNewname = retFilename(sOutdir, it, dMSE, iCols);

        padX(fin, iCols, sNewname);
        fin.close();
    }
    std::cout << "Finished!" << std::endl;

    return 0;
}
