#ifndef ACTCALIBRATIONS_H
#define ACTCALIBRATIONS_H
//Class for storing calibrations and lookup table

#include "ActParameters.h"

#include <map>
#include <string>
#include <vector>

class ActCalibrations
{
	protected:
	//Table with equivalences of pads and electronic channels
	std::vector<std::vector<int>> fTABLE;//has to be initialized in constructor
	//pad align coefficients
	std::vector<std::vector<double>> fPadAlignCoefs;
	std::map<std::string, std::vector<std::vector<double>>> fSilicon01SCalibrations {}; //not initialized in constructor
	std::vector<std::vector<double>> fSiliconBeamCalibrations;

	public:
	ActCalibrations();
	~ActCalibrations() = default;

	void ReadTABLE(std::string& tableFile);
	const std::vector<std::vector<int>>& GetTABLE() const { return fTABLE; }

	void ReadPadAlignCoefs(std::string& coefsFile);
	const std::vector<std::vector<double>>& GetPadAlignCoefs() const { return fPadAlignCoefs; }

	void ReadSilicon01SCalibrations (std::string& coefsFile, std::string panel);
	void ReadSiliconBeamCalibrations(std::string& coefsFile);
	const std::map<std::string, std::vector<std::vector<double>>>& GetSilicon01SCalibrations() const { return fSilicon01SCalibrations; }
	const std::vector<std::vector<double>>& GetSiliconBeamCalibrations() const { return fSiliconBeamCalibrations; }
};


#endif //ACTCALIBRATIONS_H
