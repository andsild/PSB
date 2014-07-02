#ifndef _FILE_H
#define _FILE_H 1

#define PROJECT_DIR "PSB"

#include <string>
#include <sys/stat.h>
#include <vector>

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

template <typename T>
void writeToFile(const std::vector<T>, std::string, std::string);
void getFilesInFolder(std::string, std::vector<std::string> &arg);

} /* EndOfNamespace */

#endif
