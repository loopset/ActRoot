#ifndef ACTCALIBRATIONS_H
#define ACTCALIBRATIONS_H
//Class for storing calibrations and lookup table

#include "ActParameters.h"
#include "ActStructs.h"

#include <algorithm>
#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include <TCanvas.h>
#include <TH2.h>

class ActTrackGeometry;

class ActCalibrations
{
	protected:
	//Table with equivalences of pads and electronic channels
	std::vector<std::vector<int>> fTABLE;//has to be initialized in constructor
	//pad align coefficients
	std::vector<std::vector<double>> fPadAlignCoefs;
	std::map<std::string, std::vector<std::vector<double>>> fSilicon01SCalibrations {}; //not initialized in constructor
	std::vector<std::vector<double>> fSiliconBeamCalibrations;

    std::map<std::string, std::map<int, std::vector<double>>> fSiliconSideCalibrations {};

	//store conversion to physical units
	double fXYToLengthUnits { ActParameters::padSideLength};//should be constant!
	double fZToLengthUnits {};
	double fZToPadUnits {};
	//compute drift velocity from TH2D when filling ActEvent
	std::unique_ptr<TH2D> fHistDrift { nullptr};


	public:
	ActCalibrations();
	~ActCalibrations() = default;

	void ReadTABLE(std::string& tableFile);
	const std::vector<std::vector<int>>& GetTABLE() const { return fTABLE; }

	void ReadPadAlignCoefs(std::string& coefsFile);
	const std::vector<std::vector<double>>& GetPadAlignCoefs() const { return fPadAlignCoefs; }

	void ReadSilicon01SCalibrations (std::string& coefsFile, std::string panel);
	void ReadSiliconBeamCalibrations(std::string& coefsFile);
    void ReadSiliconSideCalibrations(const std::string& file);
	const std::map<std::string, std::vector<std::vector<double>>>& GetSilicon01SCalibrations() const { return fSilicon01SCalibrations; }
	const std::vector<std::vector<double>>& GetSiliconBeamCalibrations() const { return fSiliconBeamCalibrations; }
    const std::map<std::string, std::map<int, std::vector<double>>>& GetSiliconSideCalibrations() const { return fSiliconSideCalibrations; }

	//////////////////////////////////////////////////////////////////////////
	//drift velocity and converto-to-physical-units coefs.
	void SetXYToLengthUnitsCoef(double val) { fXYToLengthUnits = val; }
	void SetZToLengthUnitsCoef(double val){ fZToLengthUnits = val; }
	void SetZToPadUnitsCoef(double val){ fZToPadUnits = val; }
	double GetZToLengthUnitsCoef() const { return fZToLengthUnits; }
	double GetZToPadUnitsCoef() const { return fZToPadUnits; }
	double GetXYToLengthUnitsCoef() const { return fXYToLengthUnits; }
	void InitDriftVelocityHist();//initialize histogram and fill it with physics
	void FillDriftVelocityHist(std::vector<TrackPhysics>& tracks, Silicons& silicons);
    void FillDriftVelocityHist(const ActTrackGeometry& tracks, const Silicons& silicons);
	void ComputeDriftCoefsFromHist();//compute coefs once hist is filled
	void PlotDriftVelocityHist();//plot it
	void WriteDriftCoefsToFile(std::string fileName);//write it to files
	void ReadDriftCoefsFromFile(std::string fileName);//read file
    void ComputeZDriftCoefsFromDriftVelocity(const std::string& fileName);
	std::unique_ptr<TH2D> GetDriftVelocityHist(){ return std::move(fHistDrift); }//get histo to write it to ttree
	//////////////////////////////////////////////////////////////////////////
};


#endif //ACTCALIBRATIONS_H
