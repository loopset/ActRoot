#ifndef ACTEVENTPLUS_H
#define ACTEVENTPLUS_H

#include "ActCalibrations.h"
#include "ActHit.h"
#include "ActStructs.h"
#include "MEvent.h"
#include "MEventReduced.h"
#include "Math/Point3Dfwd.h"
#include "Math/Vector3Dfwd.h"
#include <map>
#include <tuple>
#include <vector>
#include <unordered_map>

class ActEventPlus
{
    //members of an event
public:
    using XYZPoint = ROOT::Math::XYZPoint;
    using XYZVector = ROOT::Math::XYZVector;
    
    Voxels voxel {};
    TriggersAndGates triggers {};
    TimeOfFlight tof {};
    SiliconsPlus silicons {};
    int eventID {-1};

    
public:
    ActEventPlus() = default;
    ActEventPlus(ActCalibrations* calibrations, MEvent*& Evt, MEventReduced*& EvtRed);
    ~ActEventPlus() = default;

    void ReadData(ActCalibrations* calibrations, MEvent*& Evt, MEventReduced*& EvtRed);

private:
    void ReadAllButHits(MEvent* Evt,
                        MEventReduced* EvtRed,
                        Silicons& oldSilicons,
                        const int& it);
    void ReadHits(MEvent* Evt,
                  MEventReduced* EvtRed,
                  int& hitID, const std::vector<std::vector<int>>& TABLE,
                  std::map<int, std::vector<int>>& overrideHits,
                  const int& it, const int& where);

    void ReadAndCalibrateSilicons(Silicons& oldSilicons, ActCalibrations* calibrations);
    
};

#endif
