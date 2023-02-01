#ifndef ACTDETECTORS_H
#define ACTDETECTORS_H

#include "Math/Point3D.h"
#include "Math/Vector3D.h"
#include <map>
#include <vector>

///// DRIFT CHAMBER AND GET ELECTRONICS PARAMETERS
enum class ChamberMode{kACTAR_TPC, kProtoACTAR_TPC};
class ChamberDetector
{
public:
    //pads
    int fNPADSX {};//unfilled fields depend on mode!
    int fNPADSY {};
    int fNPADSZ {512};
    int fREBINZ {};
    int fNBINSZ {};
    double fPadSide {2.0};//mm

    //ADQ
    int fNB_COBO {18};
    int fNB_ASAD {4};
    int fNB_AGET {4};
    int fNB_CHANNEL {68};
    int fNB_SAMPLES {512};

    //Saturation flag check
    double fMinChargeToSaturate {3300.};

    ChamberDetector() = default;
    ChamberDetector(ChamberMode mode, int rebinFactorZ = 1);
    ~ChamberDetector() = default;
    void Print() const;
};

///// SILICONS
enum class SiliconMode {kFront, kLeft, kRight};

class SiliconUnit
{
public:
    using XYZPoint = ROOT::Math::XYZPoint;
    using XYZVector = ROOT::Math::XYZVector;

    XYZPoint fCentre {};//mm units
    double fWidth {};//mm units
    double fHeight {};//mm units
    SiliconUnit() = default;
    SiliconUnit(const XYZPoint& centre, double width, double height);
    ~SiliconUnit() = default;
};

class SiliconLayer
{
public:
    using XYZPoint = ROOT::Math::XYZPoint;
    using XYZVector = ROOT::Math::XYZVector;
    //type
    SiliconMode fMode {};
    double fOffsetInPads {};//PAD UNITS! only here
    XYZVector fNormalVector {};
    //for each silicon!
    std::map<int, SiliconUnit> fPlacements {};

    SiliconLayer() = default;
    SiliconLayer(SiliconMode mode, double offsetInPads);
    ~SiliconLayer() = default;
    void AddUnit(int index, const SiliconUnit& unit){fPlacements[index] = unit; }
};

class SiliconEnsemble
{
public:
    SiliconLayer fLayer0;
    SiliconLayer fLayer1;
    SiliconEnsemble() = default;
    ~SiliconEnsemble() = default;
    void AddLayer0(const SiliconLayer& l0){ fLayer0 = l0; }
    void AddLayer1(const SiliconLayer& l1){ fLayer1 = l1; }
};
#endif
