#ifndef LOADBAR_H
#define LOADBAR_H

#include <string>
#include <sys/ioctl.h>

namespace loadbar
{

class LoadingBar
{
    private:
        int iNumIterations;
        double dStepSize;
        mutable double dProgress;
        mutable int iTimeRemaining;
        mutable int iImageSize;

    public:
        struct winsize w;
        LoadingBar();
        LoadingBar(int iNumIterations);

    void initialize(int iNumIterations);
    std::string getTimeLeft() const;
    void updateTimeRemaining(int iImageSize) const;
    void increaseProgress(int iIterations = 1) const;
    double getProgress() const ;
    friend std::ostream& operator<< (std::ostream &str,
                                     const LoadingBar& obj);
};

} /* EndOfNamespace */

#endif
