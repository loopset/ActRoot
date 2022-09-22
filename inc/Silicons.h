#ifndef SILICONS_H
#define SILICONS_H

#include <map>
#include <string>
#include <vector>

struct Silicons
{
	//VECTORS with energy per channel
	//initialized in .cpp file
	std::vector<double> fSi0, fSi0_cal;
	std::vector<double> fSi1, fSi1_cal;
	std::vector<double> fSiS, fSiS_cal;
	std::vector<double> fSiF, fSiF_cal; //front array: combination of Si0 + Si1
	std::vector<double> fSiBeam, fSiBeam_cal;

	//maps storing information for each event
	std::map<std::string, std::map<std::string, double>> fData;

	Silicons();
	~Silicons() = default;
};


#endif //SILICONS_H
