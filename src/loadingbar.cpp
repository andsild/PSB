#include "loadingbar.hpp"

#include <string>

#include <math.h>
#include <unistd.h>
#include <sys/ioctl.h>

namespace loadbar
{

LoadingBar::LoadingBar() {};
LoadingBar::LoadingBar(int iNumIterations) : iNumIterations(iNumIterations)
{
    this->dProgress = 0;
    this->dStepSize = ((double)100 / iNumIterations);
    this->iTimeRemaining = 0;//filenames.size() * vIf.size() 
                         //* (pow( (img.height() * img.width()), 0.3));
}

void LoadingBar::initialize(int iNumIterations)
{
    this->iNumIterations = iNumIterations;
    this->dProgress = 0;
    this->dStepSize = ((double)100 / iNumIterations);
    this->iTimeRemaining = 0;//filenames.size() * vIf.size() 
                         //* (pow( (img.height() * img.width()), 0.3));
}
std::string LoadingBar::getTimeLeft() const
{
    int iMinutesLeft = this->iTimeRemaining / 60;
    int iSecondsLeft = this->iTimeRemaining - (iMinutesLeft * 60);
    return std::string(std::to_string(iMinutesLeft) + "m"
            + std::to_string(iSecondsLeft) + "s");
}
void LoadingBar::updateTimeRemaining(int iImageSize) const
{
    this->iImageSize = iImageSize;
    int iIterationsLeft = int((100 - this->getProgress()) 
                               / this->dStepSize);
    // rough estimate
    this->iTimeRemaining = iIterationsLeft * pow(iImageSize, 0.3); 
}
void LoadingBar::increaseProgress(int iIterations) const
{
    this->dProgress += this->dStepSize * iIterations;
    updateTimeRemaining(this->iImageSize);
}
double LoadingBar::getProgress() const
{
    return this->dProgress;
}

std::ostream& operator<< (std::ostream &str, const LoadingBar& obj)
{
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &obj.w);

    if(obj.getProgress() > 100) 
    { 
        // str << "(error: loading bar exceeded 100 %)" << endl;
        return str;
    }

    const int xPos = 0, yPos = 0;
    // To compensate for []>::space:: symbols
    int iColWidth = obj.w.ws_col - 20; 
    int iSignPercentage = (int) ((obj.getProgress() * iColWidth) / 100),
        iPercentage     = (int)obj.getProgress();
    std::string sMarker(iColWidth, '=');

    // printf("\033[%d;%dH[%s>%*c] %3d%%\n", xPos, yPos
    printf("\r[%s>%*c] %3d%% ETA %s"
                             , sMarker.substr(0,iSignPercentage).c_str()
                             , iColWidth - iSignPercentage, ' '
                             , iPercentage
                             , obj.getTimeLeft().c_str());
    obj.increaseProgress();
    return str;
}

} /* EndOfNamespace */
