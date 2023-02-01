#include "ActRoot.h"

#include "ActDetectors.h"

ActRoot* ActRoot::gActRoot = nullptr;

ActRoot* ActRoot::GetInstance()
{
    if(!gActRoot)
        gActRoot = new ActRoot();
    return gActRoot;
}

void ActRoot::SetChamber(ChamberMode mode, int rebinFactorZ)
{
    chamber = ChamberDetector(mode, rebinFactorZ);
}

void ActRoot::SetVXIActions(const std::string &fileName)
{
    acq.ReadVXI(fileName);
}
