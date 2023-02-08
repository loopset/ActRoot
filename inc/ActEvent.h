#ifndef ACTEVENT_H
#define ACTEVENT_H

#include "ActCalibrations.h"
#include "ActHit.h"
#include "ActEventData.h"

#include "MEvent.h"
#include "MEventReduced.h"

#include "Math/Point3Dfwd.h"
#include "Math/Vector3Dfwd.h"
#include "RtypesCore.h"

#include <fstream>
#include <map>
#include <string>
#include <utility>
#include <vector>
#include <unordered_map>

class ActEvent
{
    //members of an event
public:
    using XYZPoint = ROOT::Math::XYZPoint;
    using XYZVector = ROOT::Math::XYZVector;
    
    Voxels voxel {};
    TriggersAndGates triggers {};
    TimeOfFlight tof {};
    SiliconData silicons {};
    unsigned int eventID {};
    unsigned int entryID {};
    unsigned int runID {};

    
public:
    ActEvent() = default;
    ActEvent(unsigned int run, unsigned int entry,
             MEventReduced*& EvtRed);
    ~ActEvent() = default;

    void ReadData(ActCalibrations*& calibrations,
                  const std::vector<std::vector<int>>& TABLE,
                  const std::vector<std::vector<double>>& padAlignCoefs,
                  MEvent*& Evt, MEventReduced*& EvtRed,
                  bool alignCharge = true,
                  bool silIndividualThresh = false,
                  bool writeSilicons = true);

private:
    void ReadAllButHits(MEvent*& Evt,
                        MEventReduced*& EvtRed,
                        SiliconRawData& siliconraw,
                        int it);

    void ReadHits(MEvent*& Evt,
                  MEventReduced*& EvtRed,
                  int& hitID,
                  const std::vector<std::vector<int>>& TABLE,
                  const std::vector<std::vector<double>>& padAlignCoefs,
                  std::unordered_map<int, std::pair<int, int>>& overrideHits,
                  bool alignCharge,
                  int it, int where);

    void CalibrateSilicons(ActCalibrations* calibrations, SiliconRawData& raw, bool silIndividualThresh);
};

#endif
