#ifndef ACTEVENTDATA_H
#define ACTEVENTDATA_H

#include "ActDetectors.h"
#include "ActHit.h"

#include "RtypesCore.h"
#include "TH1D.h"

#include <map>
#include <string>
#include <utility>
#include <vector>

//USE ROOT TYPES for classes being written to disk

class SiliconData
{
public:
    std::map<std::pair<SiliconMode, SiliconPanel>, std::map<std::string, double>> fData {};

    void Print() const;
};

class SiliconRawData
{
public:
    std::map<std::pair<SiliconMode, SiliconPanel>, std::map<int, double>> fRaw {};
    std::map<std::pair<SiliconMode, SiliconPanel>, std::map<int, double>> fCal {};
    
    void Calibrate(const std::map<int, std::vector<double>>& calibration,
                   const std::map<int, double>& raw,
                   std::map<int, double>& cal);
    void ReadAndWrite(SiliconData& fin, bool individualThreshold = false);

    void Print() const;
};

class TriggersAndGates
{
public:
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

class TimeOfFlight
{
public:
    double tSilR13 {-1};
    double tSilR46 {-1};
    double tSilL13 {-1};
    double tSilL46 {-1};

    TimeOfFlight() = default;
    ~TimeOfFlight() = default;

    void Print() const;
};

class Voxels
{
public:
    std::vector<ActHit> fHits {};
};

class RunInfo
{
public:
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

#endif
