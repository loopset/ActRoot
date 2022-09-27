#ifndef ACTCALIBRATIONS_H
#define ACTCALIBRATIONS_H
//Class for storing calibrations and lookup table

#include "ActParameters.h"
#include "ActStructs.h"

#include <algorithm>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include <TCanvas.h>
#include <TH2.h>

class ActCalibrations
{
	protected:
	//Table with equivalences of pads and electronic channels
	std::vector<std::vector<int>> fTABLE;//has to be initialized in constructor
	//pad align coefficients
	std::vector<std::vector<double>> fPadAlignCoefs;
	std::map<std::string, std::vector<std::vector<double>>> fSilicon01SCalibrations {}; //not initialized in constructor
	std::vector<std::vector<double>> fSiliconBeamCalibrations;

	//compute drift velocity from TH2D when filling ActEvent
	std::unique_ptr<TCanvas> fCanvDrift { nullptr};
	std::unique_ptr<TH2D> fHistDrift { std::make_unique<TH2D>("fHistDrift", "Drift velocity",
											100, 0., ActParameters::g_NPADX,
											100, 0., ActParameters::g_NPADZ)};
	double fZToLengthUnits {};

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

	//drift velocity
	void ReadDriftVelocity(std::vector<TrackPhysics>& tracks, Silicons& silicons);
	void PlotDriftVelocity();
	std::unique_ptr<TH2D> GetDriftVelocity(){ return std::move(fHistDrift); }
};


#endif //ACTCALIBRATIONS_H
