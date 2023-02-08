#include "ActRoot.h"

#include "ActDetectors.h"
#include <stdexcept>

ActRoot* ActRoot::fgActRoot = nullptr;

ActRoot* ActRoot::Get()
{
    if(!fgActRoot)
        fgActRoot = new ActRoot();
    return fgActRoot;
}

ChamberDetector ActRoot::GetChamber()
{
    if(!fgActRoot)
        throw std::runtime_error("Error: gActRoot global pointer has not been initialized yet! See config macro!");
    return fgActRoot->chamber;
}

SiliconDetector ActRoot::GetSilicons()
{
    if(!fgActRoot)
        throw std::runtime_error("Error: gActRoot global pointer has not been initialized yet! See config macro!");
    return fgActRoot->silicons;
}

void ActRoot::SetChamber(ChamberMode mode, int rebinFactorZ)
{
    chamber = ChamberDetector(mode, rebinFactorZ);
}

void ActRoot::SetVXIActions(const std::string &fileName)
{
    acq.ReadVXI(fileName);
}
