#ifndef SIMGEOMETRY_H
#define SIMGEOMETRY_H

/*
  Class for simulate naive ACTAR TPC geometry + silicons
*/

#include "Math/Point3Dfwd.h"
#include "Math/Vector3Dfwd.h"
#include "RtypesCore.h"
#include "TCanvas.h"
#include "TGeoManager.h"
#include "TGeoMaterial.h"
#include "TGeoMedium.h"
#include "TGeoVolume.h"
#include "TString.h"
#include "TView3D.h"
#include "TGeoNavigator.h"
#include "TObject.h"
#include <map>
#include <string>
#include <tuple>
#include <utility>

struct DriftInfo
{
    double X {};
    double Y {};
    double Z {};
};

struct SilInfo
{
    std::map<int, double> unitX {};
    std::map<int, double> unitY {};
    std::map<int, double> unitZ {};

    double XOffset {};
    bool IsXOffset {};
    double YOffset {};
    bool IsYOffset {};
    std::map<int, std::map<int, std::pair<double, double>>> AssemblyMap {};
    std::map<int, std::map<int, bool>> IsRotatedMap {};
    
};

class SimGeometry
{
 public:
    using XYZPoint  = ROOT::Math::XYZPoint;
    using XYZVector = ROOT::Math::XYZVector;
 private:
    TGeoManager* manager {nullptr};
    TGeoNavigator* navigator {nullptr};

    //shapes (stored in maps for silicons, since we have various types)
    TGeoVolume* topVol {nullptr};
    TGeoVolume* driftVol {nullptr};
    std::map<int, TGeoVolume*> unitSilVolMap {};
    TGeoVolume* silAssembly {nullptr};

    //materials and mediums (not relevant, just we need some kind of material)
    TGeoMaterial* noneMaterial {nullptr};
    TGeoMedium* noneMedium {nullptr};

    //sizes (remember that they are half lengths)
    SilInfo silicons {};
    DriftInfo actar {};

    //for plotting
    TCanvas* canvas {nullptr};

 public:
    SimGeometry();
    ~SimGeometry();

    //setters
    void SetDriftSizes(double sizeX, double sizeY, double sizeZ);
    void SetSiliconPlacementInAssembly(std::map<int, std::map<int, std::pair<double, double>>> places) { silicons.AssemblyMap = places; }
    void SetSiliconRotation(std::map<int, std::map<int, bool>> rota) { silicons.IsRotatedMap = rota; }
    void SetSiliconAssemblyOffset(double offsetX, double offsetY);
    void SetSiliconUnit(int type, double x, double y, double z);
    //getters
    DriftInfo GetDriftParameters() const { return actar; }
    SilInfo   GetSilParameters()   const { return silicons; }
    
    void Construct();

    void PrintGeometry();

    void Draw();

    void PropagateTrackToSilicons(const XYZPoint& point,
                                  const XYZVector& direction,
                                  double& distance,
                                  int& assemblyIndex,
                                  int& silType,
                                  int& silIndex,
                                  bool debug = false);

    void CheckIfStepIsInsideDriftChamber(const XYZPoint& point,
                                         const XYZVector& direction,
                                         double step,
                                         bool& isInside,
                                         bool debug);

    void ReadGeometry(std::string path, std::string fileName);

    void WriteGeometry(std::string path, std::string fileName);

private:
    std::tuple<int, int, int> GetSilTypeAndIndexFromTString(TString& path);
};

#endif
