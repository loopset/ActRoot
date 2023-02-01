#ifndef ACTROOT_H
#define ACTROOT_H

#include "ActEventData.h"
#include "ActDetectors.h"
#include "ActLegacy.h"

#include <map>
#include <string>
class ActRoot
{
public:
    ChamberDetector chamber {};
    std::map<SiliconMode, SiliconEnsemble> silicons {};
    ActLegacy acq {};
private:
    ActRoot() {};
    //singleton model!
    static ActRoot* gActRoot;
public:
    ActRoot(ActRoot& ) = delete;
    void operator=(const ActRoot&) = delete;

    static ActRoot* GetInstance();
    
    void SetChamber(ChamberMode mode, int rebinFactorZ = 1);
    void SetVXIActions(const std::string& fileName);
    
};

#endif
