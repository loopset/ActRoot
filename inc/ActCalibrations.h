#ifndef ACTCALIBRATIONS_H
#define ACTCALIBRATIONS_H
//Class for storing calibrations and lookup table

#include "ActParameters.h"

#include <string>
#include <vector>

class ActCalibrations
{
	protected:
	//Table with equivalences of pads and electronic channels
	std::vector<std::vector<int>> fTABLE;//has to be initialized in constructor
	//pad align coefficients
	std::vector<std::vector<double>> fPadAlignCoefs;

	public:
	ActCalibrations();
	~ActCalibrations() = default;

	void ReadTABLE(std::string& tableFile);
	const std::vector<std::vector<int>>& GetTABLE() const { return fTABLE; }

	void ReadPadAlignCoefs(std::string& coefsFile);
	const std::vector<std::vector<double>>& GetPadAlignCoefs() const { return fPadAlignCoefs; }
};


#endif //ACTCALIBRATIONS_H
