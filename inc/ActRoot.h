#ifndef ACTROOT_H
#define ACTROOT_H

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

    static ActRoot* GetInstance();
    static ChamberDetector GetChamber();
    static SiliconDetector GetSilicons();
    
    void SetChamber(ChamberMode mode, int rebinFactorZ = 1);
    void SetVXIActions(const std::string& fileName);
    
};

#endif
