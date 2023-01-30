#ifndef ACTSTRUCTS_H
#define ACTSTRUCTS_H

#include "ActHit.h"
#include <Math/Point3Dfwd.h>
#include <Math/Point3D.h>
#include <TH1D.h>

#include <Math/Vector3Dfwd.h>
#include <Math/Vector3D.h>

#include <utility>
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

	void Print() const;
    std::pair<std::string, int> GetSilSideAndIndex() const;
};

struct TriggersAndGates
{
	int INCONF {};
	int GATCONF {};
	// int TVAL_CFA_HF    {};
	// int TVAL_CFA_CATD4 {};
	// int TVAL_CFA_CATD6 {};

	//unsigned long DT_CLK_UP_UP {};
	unsigned long DT_CLK_UP {};
	unsigned long DT_CLK {};
	// unsigned long DT_GET_UP_UP {};
	unsigned long DT_GET_UP {};
	unsigned long DT_GET {};
	// unsigned long DT_VXI_UP_UP {};
	unsigned long DT_VXI_UP {};
	unsigned long DT_VXI {};

	unsigned long CTR_TIMEH_UP {};
	unsigned long CTR_TIMEH {};
	unsigned long CTR_TIMEML_UP {};
	unsigned long CTR_TIMEML {};
	unsigned long CTR_EVT_UP {};
    unsigned long CTR_EVT {};

	// unsigned long SCA_TiD3_UP {};
	// unsigned long SCA_TiD3 {};	
	// unsigned long SCA_CATSD4_UP {};
	// unsigned long SCA_CATSD4 {};	
	// unsigned long SCA_CATSD5_UP {};
	// unsigned long SCA_CATSD5 {};	
	// unsigned long SCA_CFA_UP {};
	// unsigned long SCA_CFA {};	
	// unsigned long SCA_SiBeam1_UP {};
	// unsigned long SCA_SiBeam1 {};
	// unsigned long SCA_SiBeam2_UP {};
	// unsigned long SCA_SiBeam2 {};	
	// unsigned long SCA_Si0_UP {};
	// unsigned long SCA_Si0 {};
	// unsigned long SCA_SiS_UP {};
	// unsigned long SCA_SiS {};
	// unsigned long SCA_L1ok_UP {};
	// unsigned long SCA_L1ok {};

	TriggersAndGates() = default;
	~TriggersAndGates() = default;

    inline double GetTimeStampFromCTRs()
    {
        double val {std::pow(2, 16) * std::pow(2, 16) * std::pow(2, 16) * CTR_TIMEH_UP +
            std::pow(2, 16) * std::pow(2, 16) * CTR_TIMEH + std::pow(2, 16) * CTR_TIMEML_UP +
            CTR_TIMEML};
        return val;
    }
};

struct Voxels
{
    std::vector<ActHit> fHits {};
};

struct SiliconsPlus
{
    std::map<std::string, std::map<std::string, double>> fData {};

    void Print() const;
    std::pair<std::string, int> GetSilSideAndIndex() const;
};

struct EventPlusCuts
{
    double fZPileUpMean;
    double fZPileUpWidth;
    unsigned int fNSat;
    bool fSilCut;
    bool fTopoCut;

    EventPlusCuts() = default;
    ~EventPlusCuts() = default;
};

struct TimeOfFlight
{
    double tSilR13 {-1};
    double tSilR46 {-1};
    double tSilL13 {-1};
    double tSilL46 {-1};

    TimeOfFlight() = default;
    ~TimeOfFlight() = default;

    void Print() const;
};

struct RunInfo
{
    double fRunDuration {};
    double fDT_VXI {}; double fuDT_VXI {};
    double fDT_GET {}; double fuDT_GET {};
    unsigned long int fTotalRecordedEvents {};

    void Print() const;
    inline void ComputeDurationFromScalerSum(double ctrTimeMaximum)
    {
        fRunDuration = ctrTimeMaximum / 1.0E8;//1 unit = 10 ns!
    }
    inline void ComputeDTsFromHistos(TH1D*& histVXI, TH1D*& histGET)
    {
        fDT_VXI = histVXI->GetMean(); fuDT_VXI = histVXI->GetMeanError();
        fDT_GET = histGET->GetMean(); fuDT_GET = histGET->GetMeanError();
    }
};
#endif //ACTSTRUCTS_H
