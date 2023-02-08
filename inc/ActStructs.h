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

#endif //ACTSTRUCTS_H
