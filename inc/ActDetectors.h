#ifndef ACTDETECTORS_H
#define ACTDETECTORS_H

#include "Math/Point3D.h"
#include "Math/Vector3D.h"
#include <map>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
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
enum class SiliconPanel {kLayer0, kLayer1};

class SiliconUnit
{
public:
    using XYZPoint = ROOT::Math::XYZPoint;
    using XYZVector = ROOT::Math::XYZVector;

    XYZPoint fCentre {};//mm units
    double fWidth {};//mm units
    double fHeight {};//mm units
    double fEnergyThreshold {};//MeV
    SiliconUnit() = default;
    SiliconUnit(const XYZPoint& centre, double width, double height);
    ~SiliconUnit() = default;
    void Print() const;
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
    void ReadFile(const std::string& fileName);
};

// class SiliconEnsemble
// {
// public:
//     SiliconLayer fLayer0;
//     SiliconLayer fLayer1;
//     SiliconEnsemble() = default;
//     ~SiliconEnsemble() = default;
//     void AddLayer0(const SiliconLayer& l0){ fLayer0 = l0; }
//     void AddLayer1(const SiliconLayer& l1){ fLayer1 = l1; }
// };

class SiliconDetector
{
public:
    std::unordered_map<SiliconMode, std::pair<SiliconPanel, SiliconLayer>> fSilicons {};
    std::unordered_set<SiliconMode> fModes {};
    SiliconDetector() = default;
    ~SiliconDetector() = default;
    void AddLayer(SiliconMode mode, SiliconPanel panel, const SiliconLayer& layer);
    void SetModes(std::unordered_set<SiliconMode> modes){ fModes = modes; }
};
#endif
