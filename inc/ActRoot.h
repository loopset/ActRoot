#ifndef ACTROOT_H
#define ACTROOT_H

//Colored output
#include <tuple>
#include <utility>
#define RESET   "\033[0m"
#define BLACK   "\033[30m"      /* Black */
#define RED     "\033[31m"      /* Red */
#define GREEN   "\033[32m"      /* Green */
#define YELLOW  "\033[33m"      /* Yellow */
#define BLUE    "\033[34m"      /* Blue */
#define MAGENTA "\033[35m"      /* Magenta */
#define CYAN    "\033[36m"      /* Cyan */
#define WHITE   "\033[37m"      /* White */
#define BOLDBLACK   "\033[1m\033[30m"      /* Bold Black */
#define BOLDRED     "\033[1m\033[31m"      /* Bold Red */
#define BOLDGREEN   "\033[1m\033[32m"      /* Bold Green */
#define BOLDYELLOW  "\033[1m\033[33m"      /* Bold Yellow */
#define BOLDBLUE    "\033[1m\033[34m"      /* Bold Blue */
#define BOLDMAGENTA "\033[1m\033[35m"      /* Bold Magenta */
#define BOLDCYAN    "\033[1m\033[36m"      /* Bold Cyan */
#define BOLDWHITE   "\033[1m\033[37m"      /* Bold White */

#include "ActEventData.h"
#include "ActDetectors.h"
#include "ActLegacy.h"

#include <map>
#include <string>
#include <unordered_set>
class ActRoot
{
public:
    ChamberDetector chamber {};
    SiliconDetector silicons {};
    ActLegacy acq {};
private:
    ActRoot() {};
    //singleton model!
    static ActRoot* gActRoot;
public:
    ActRoot(ActRoot& ) = delete;
    void operator=(const ActRoot&) = delete;

    static ActRoot* Get();
    static ChamberDetector GetChamber();
    static SiliconDetector GetSilicons();
    
    void SetChamber(ChamberMode mode, int rebinFactorZ = 1);
    void SetVXIActions(const std::string& fileName);
    
};

#endif
