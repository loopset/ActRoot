#ifndef ACTTRACKPLUS_H
#define ACTTRACKPLUS_H

//The best version of a track, improving ActTrackGeometry
#include "ActCalibrations.h"
#include "ActEventPlus.h"
#include "ActHit.h"
#include "ActStructs.h"
#include "ActTrack.h"

#include "SimSRIM.h"
#include "SimKinematics.h"

#include "Math/Point3D.h"
#include "Math/Vector3D.h"
#include "TCanvas.h"
#include "TF1.h"
#include "TH1.h"

#include <fstream>
#include <map>
#include <string>
#include <utility>
#include <vector>
class ActTrackPlus
{
public:
    using XYZPoint = ROOT::Math::XYZPoint;
    using XYZVector = ROOT::Math::XYZVector;

    TH1D fHistProfile {};
    std::map<std::pair<int, int>, std::pair<double, bool>> fPadMatrix {};
    XYZPoint fGravityPoint {-1, -1, -1};
    XYZPoint fSiliconPoint {-1, -1, -1};
    XYZPoint fBoundaryPoint {-1, -1, -1};
    XYZPoint fReactionPoint {-1, -1, -1};
    double fTotalCharge {-1};
    double fChargePerPad {-1};
    double fChargeInRegion {-1};
    double fLengthInRegion {-1};
    double fPIDInRegion {-1};
    double fLengthInChamber {-1};
    double fTrackLength {-1};
    double fSilEnergy {-1};
    double fRPEnergy {-1};
    double fReconstructedBeamEnergy {-1};
    double fTheta {-1};
    double fPhi   {-1};
    std::string fSiliconSide {"-1"};
    unsigned int fNSatPads {};
    unsigned int fSiliconIndex {};
    unsigned int fRegionWidth {};
    unsigned int fNPeaksInChargeProfile {};
    unsigned int fTrackID {};
    unsigned int fEntryID {};
    unsigned int fEventID {};
    unsigned int fRunID   {};
    bool fMatchesRealSilicon {};
    bool fBPInChamber {};
    bool fIsPIDCorrected {};
    bool fInsideAnalysisCuts {};

    ActTrackPlus() = default;
    ActTrackPlus(unsigned int run, unsigned int eve, unsigned int entry,
                 const ActTrack& track, const SiliconsPlus& silicons);
    ~ActTrackPlus() = default;

    void ConvertToLengthUnits(const ActCalibrations& calibrations);

    void MatchSPWithRealPlacement();
    
    void ComputeAngles();

    void ComputeChargeInRegion(int yPads, const ActCalibrations& calibrations);

    void ComputeReactionPointFromChargeProfile(const ActTrack& cluster,
                                               const ActCalibrations& calibrations,
                                               TCanvas* canv = nullptr);

    void CountNumberOfPeaksInChargeProfile(double sigma = 2,
                                           std::string options = "noMarkov nobackground",
                                           double threshold = 0.5);

    bool ComputeRMSInChargeProfile(double threshold = 1000);

    bool ComputeBraggPeakInChargeProfile(double threshold = 0.7);
    
    void ComputeEnergyAtVertexWithSRIM(SimSRIM* srim, const std::string& srimString);

    void ReconstructBeamEnergyFromLAB(SimKinematics* kinematics);

    void CorrectPIDInRegion(TF1* funCorr);

    void Print() const;

    void WriteToStreamer(std::ofstream& streamer, const int& anaEntry = {-1}) const;
    
private:
    void FillPadMatrix(const ActTrack& track);
    void CalculateSiliconPoint(const ActTrack& track, const SiliconsPlus& silicons);
    void CalculateBoundaryPoint();
    void GetChargeProfile(const ActTrack& cluster,
                          const ActCalibrations& calibrations,
                          TH1D*& histProfile);
    

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
        //WARNING! Cast to int to avoid double over/underflow after computing interesection point! Only in fixed dimension = Y
		bool condY { static_cast<int>(point.Y()) >= 0 && static_cast<int>(point.Y()) <= ActParameters::g_NPADY};
		bool condZ { point.Z() >= 0. && point.Z() <= ActParameters::g_NPADZ};
		return (condX && condY && condZ);
	}
    template<typename T>
    inline T ScalePointOrVector(const ActCalibrations& calibrations, const T& oldPoint)
    {
        T newPoint;
        newPoint.SetX(oldPoint.X() * calibrations.GetXYToLengthUnitsCoef());
        newPoint.SetY(oldPoint.Y() * calibrations.GetXYToLengthUnitsCoef());
        newPoint.SetZ(oldPoint.Z() * calibrations.GetZToLengthUnitsCoef());
        return newPoint;
    }
    template<typename T>
    static inline bool isInVector(T val, std::vector<T> vec)
	{
		if (vec.size() == 0)
			return false;
		return std::find(vec.begin(), vec.end(), val) != vec.end();
	}
    
};

#endif
