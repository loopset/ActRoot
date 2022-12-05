#ifndef ACTTRACKGEOMETRY_H
#define ACTTRACKGEOMETRY_H
#include "ActCalibrations.h"
#include "ActEvent.h"
#include "ActTrack.h"
#include "SimGeometry.h"

#include "TCanvas.h"
#include "TH2D.h"

#include <Math/Point3Dfwd.h>
#include <Math/Point3D.h>

#include <Math/Vector3Dfwd.h>
#include <Math/Vector3D.h>
#include <memory>
#include <string>
#include <tuple>
#include <map>
#include <utility>
#include <vector>

class ActTrackGeometry
{
public:
    using XYZPoint = ROOT::Math::XYZPoint;
	using XYZVector = ROOT::Math::XYZVector;
    
    //basic track info
    int fTrackID {-1};
    int fNSaturatedPads {-1};
    double fTotalCharge {-1};
    double fAverageChargeAlongPads {-1};
    //1-fitting info
    XYZPoint fGravityPoint {-1, -1, -1};
    XYZPoint fGravityPointMM {-1, -1, -1};
    XYZVector fUnitaryDirection {-1, -1, -1};
    XYZVector fUnitaryDirectionMM {-1, -1, -1};
    //2-silicon info
    std::string fSiliconPlace {};
    int fSilIndex {-1};
    XYZPoint fSiliconPoint {-1, -1, -1};
    XYZPoint fSiliconPointMM {-1, -1, -1};
    XYZPoint fBoundaryPoint{-1, -1, -1};
    XYZPoint fBoundaryPointMM {-1, -1, -1};
    bool fBPInChamber {false};
    //3-inner point
    int fYWidthInPads {-1};
    XYZPoint fInnerPoint {-1,-1,-1};
    XYZPoint fInnerPointMM {-1,-1,-1};
    //4-temporary (or not) fLengthInRegion and fChargeInRegion
    bool fTrackAlsoOutside {false};
    double fLengthInRegion {-1};
    double fChargeInRegion {-1};
    //5-other info
    double fTheta {-1.};
    double fPhi   {-1.};
    
    std::map<std::pair<int, int>, double> fPad {};
    std::map<std::pair<int, int>, bool> fSatMatrix {};
public:
    ActTrackGeometry() = default;
    ActTrackGeometry(const ActEvent& event,
                     const ActTrack& track,
                     const std::string& side, int silIndex);
    ~ActTrackGeometry() = default;

    const std::map<std::pair<int, int>, double>& GetTrackPad() const { return fPad; }
    const std::map<std::pair<int, int>, bool>&   GetSaturationMatrix() const { return fSatMatrix; }

    void Print(std::string mode = "pad") const;
        
    void ConvertToLengthUnits(const ActCalibrations& calibrations);
    std::pair<double, double> ComputeChargeInRegion(int yPads, const ActCalibrations& calibrations);
    void ComputeAngles();

private:
    void CalculateChargesAndSaturatedPads();
    
    void FillMatrices(const ActEvent& event, const ActTrack& track);
    
    void CalculateUnitaryDirectionAndSiliconPoint(const ActTrack& track);

    void CalculateBoundaryPoint();

    //and convert to length units
    template<typename T>
    T ScalePoint(const ActCalibrations& calibrations, const T& oldPoint);
    
    
     inline XYZPoint IntersectionTrackPlane(XYZPoint Pp, XYZVector vp, XYZPoint Tp, XYZVector Tv)
	{
		auto Pt { Tp};//point of track
		auto vt { Tv};//vt is the direction vector of the track
		//following https://math.stackexchange.com/questions/3412199/how-to-calculate-the-intersection-point-of-a-vector-and-a-plane-defined-as-a-poi
		auto interesection { Pt + (((Pp - Pt).Dot(vp)) / (vt.Dot(vp))) * vt};
		return interesection;
	}
	inline bool IsInChamber(XYZPoint point)
	{
		bool condX { point.X() >= 0. && point.X() <= ActParameters::g_NPADX};
        //WARNING! Cast to int to avoid double over/underflow after computing interesection point! Only in fixed dimension
		bool condY { static_cast<int>(point.Y()) >= 0 && static_cast<int>(point.Y()) <= ActParameters::g_NPADY};
		bool condZ { point.Z() >= 0. && point.Z() <= ActParameters::g_NPADZ};
		return (condX && condY && condZ);
	}


};


#endif
