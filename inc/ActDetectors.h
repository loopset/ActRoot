#ifndef ACTDETECTORS_H
#define ACTDETECTORS_H

#include "Math/Point3D.h"
#include "Math/Vector3D.h"
#include <map>
#include <ostream>
#include <string>
#include <map>
#include <set>
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
std::ostream& operator<<(std::ostream& s, const SiliconMode& mode);

enum class SiliconPanel {kLayer0, kLayer1};
std::ostream& operator<<(std::ostream& s, const SiliconPanel& panel);

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
    SiliconUnit(const XYZPoint& centre, double width, double height, double thresh = 0.0);
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
    double fCommonEnergyThreshold {-1};
    //for each silicon!
    std::map<int, SiliconUnit> fPlacements {};

    SiliconLayer() = default;
    SiliconLayer(SiliconMode mode, double offsetInPads);
    ~SiliconLayer() = default;
    void AddUnit(int index, const SiliconUnit& unit){fPlacements[index] = unit; }
    void ReadFile(const std::string& fileName);
    void SetCommonEnergyThreshold(double val) { fCommonEnergyThreshold = val; }
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
    std::map<std::pair<SiliconMode, SiliconPanel>, SiliconLayer> fMap {};
    std::set<std::pair<SiliconMode, SiliconPanel>> fModes {};
    SiliconDetector() = default;
    ~SiliconDetector() = default;
    void AddLayer(SiliconMode mode, SiliconPanel panel, const SiliconLayer& layer);
    void SetModes(std::set<std::pair<SiliconMode, SiliconPanel>> modes){ fModes = modes; }
};
#endif
