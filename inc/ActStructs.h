#ifndef ACTSTRUCTS_H
#define ACTSTRUCTS_H

#include <Math/Point3Dfwd.h>
#include <Math/Point3D.h>

#include <Math/Vector3Dfwd.h>
#include <Math/Vector3D.h>

#include <vector>
#include <map>
#include <string>

class ActCalibrations;

struct Silicons
{
	//VECTORS with energy per channel
	//initialized in .cpp file
	std::vector<double> fSilLeft0, fSilLeft0_cal;
	std::vector<double> fSilLeft1, fSilLeft1_cal;
    std::vector<double> fSilRight0, fSilRight0_cal;
	std::vector<double> fSilRight1, fSilRight1_cal;
	std::vector<double> fSilFront0, fSilFront0_cal;
    std::vector<double> fSilFront1, fSilFront1_cal;
	std::vector<double> fSiBeam, fSiBeam_cal;
    //new version using maps
    std::map<std::string, std::vector<double>> fSilSide0 {};
    std::map<std::string, std::vector<double>> fSilSide0_cal {};

	//maps storing information for each event
	std::map<std::string, std::map<std::string, double>> fData {};

	Silicons();
	~Silicons() = default;

	void Print();
};

struct TriggersAndGates
{
	int INCONF {};
	int GATCONF {};
	int TVAL_CFA_HF    {};
	int TVAL_CFA_CATD4 {};
	int TVAL_CFA_CATD6 {};

	unsigned long DT_CLK_UP_UP {};
	unsigned long DT_CLK_UP {};
	unsigned long DT_CLK {};
	unsigned long DT_GET_UP_UP {};
	unsigned long DT_GET_UP {};
	unsigned long DT_GET {};
	unsigned long DT_VXI_UP_UP {};
	unsigned long DT_VXI_UP {};
	unsigned long DT_VXI {};

	unsigned long CTR_TIMEH_UP {};
	unsigned long CTR_TIMEH {};
	unsigned long CTR_TIMEML_UP {};
	unsigned long CTR_TIMEML {};
	unsigned long CTR_EVT_UP {};
	unsigned long CTR_EVT {};

	unsigned long SCA_TiD3_UP {};
	unsigned long SCA_TiD3 {};	
	unsigned long SCA_CATSD4_UP {};
	unsigned long SCA_CATSD4 {};	
	unsigned long SCA_CATSD5_UP {};
	unsigned long SCA_CATSD5 {};	
	unsigned long SCA_CFA_UP {};
	unsigned long SCA_CFA {};	
	unsigned long SCA_SiBeam1_UP {};
	unsigned long SCA_SiBeam1 {};
	unsigned long SCA_SiBeam2_UP {};
	unsigned long SCA_SiBeam2 {};	
	unsigned long SCA_Si0_UP {};
	unsigned long SCA_Si0 {};
	unsigned long SCA_SiS_UP {};
	unsigned long SCA_SiS {};
	unsigned long SCA_L1ok_UP {};
	unsigned long SCA_L1ok {};

	TriggersAndGates() = default;
	~TriggersAndGates() = default;
};

struct TrackPhysics
{
	using XYZPoint = ROOT::Math::XYZPoint;
	using XYZVector = ROOT::Math::XYZVector;
	
	int fTrackID {-1};
	double fTheta {-1.};
	double fPhi {-1.};
	XYZPoint fReactionPoint {-1, -1, -1};
	XYZPoint fSiliconPoint {-1, -1, -1};
	XYZPoint fBoundaryPoint {-1, -1, -1};
	double fTrackLength {-1.};
	double fTrackLengthInGas {-1.};
	double fTotalCharge {-1.};
	double fAverageCharge {-1.};
    double fAverageChargeInChamber {-1};
	std::string fReactionPlace {""};
	//std::string fBoundaryPlace {""};
	std::string fSiliconPlace {""};
	bool fRPInChamber {false};
	bool fSPInArray   {false};
    int fSaturatedPads {-1};
	
	TrackPhysics() = default;
	~TrackPhysics() = default;

	void Print(std::string mode = "full");
	void SetTrackFullPhysics(ActCalibrations& calibrations);

private:
	void CalculateThetaTrack();
	void CalculatePhiTrack();
	void CalculateReactionPoint(ActCalibrations& calibrations);
	void CalculateSiliconPoint(ActCalibrations& calibrations);
	void CalculateBoundaryPoint(ActCalibrations& calibrations);
	void CalculateTrackLength();
	void CalculateTrackAverageCharge();

	inline XYZPoint IntersectionTrackPlane(XYZPoint Pp, XYZVector vp)
	{
		auto Pt { fReactionPoint};//point of plane
		auto vt { (fSiliconPoint - fReactionPoint).Unit()};
		auto interesection { Pt + (((Pp - Pt).Dot(vp)) / (vt.Dot(vp))) * vt};
		return interesection;
	}
};

struct EventInfo
{
    int fSaturatedPads {};
    double fAveragedCharge {};

    EventInfo() = default;
    ~EventInfo() = default;

    void Print() const;
};

#endif //ACTSTRUCTS_H
