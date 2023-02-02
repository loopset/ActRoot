#include "ActRoot.h"

#include "ActDetectors.h"
#include <stdexcept>

ActRoot* ActRoot::gActRoot = nullptr;

ActRoot* ActRoot::GetInstance()
{
    if(!gActRoot)
        gActRoot = new ActRoot();
    return gActRoot;
}

ChamberDetector ActRoot::GetChamber()
{
    if(!gActRoot)
        throw std::runtime_error("Error: gActRoot global pointer has not been initialized yet! See config macro!");
    return gActRoot->chamber;
}

SiliconDetector ActRoot::GetSilicons()
{
    if(!gActRoot)
        throw std::runtime_error("Error: gActRoot global pointer has not been initialized yet! See config macro!");
    std::cout<<"Ref in GetSilicons() "<<&(gActRoot->silicons)<<'\n';
    return gActRoot->silicons;
}

void ActRoot::SetChamber(ChamberMode mode, int rebinFactorZ)
{
    chamber = ChamberDetector(mode, rebinFactorZ);
}

void ActRoot::SetVXIActions(const std::string &fileName)
{
    acq.ReadVXI(fileName);
}
