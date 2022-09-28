#include "ActCalibrations.h"

#include "ActParameters.h"
#include "ActStructs.h"

#include <TH2.h>
#include <TCanvas.h>

#include <TROOT.h>
#include <fstream>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <vector>
#include <string>

ActCalibrations::ActCalibrations()
	: fTABLE(6, std::vector<int>(ActParameters::g_NB_COBO *
								 ActParameters::g_NB_ASAD *
								 ActParameters::g_NB_AGET *
								 ActParameters::g_NB_CHANNEL, 0)),
	  fPadAlignCoefs(ActParameters::NCoefRows, std::vector<double>(3)),
	  fSiliconBeamCalibrations(ActParameters::NrowsSiBeam, std::vector<double>(ActParameters::NcolsSiBeam))
{
}

void ActCalibrations::ReadTABLE(std::string& tableFile)
{
	std::ifstream streamer(tableFile.c_str());
	if(streamer.fail())
	{
		throw std::runtime_error("Fail opening LT table!");
	}
	int aux0, aux1, aux2, aux3, aux4, aux5;
	for(int i = 0; i < fTABLE[0].size(); i++)
	{
		streamer >> aux0 >> aux1 >> aux2 >> aux3 >> aux4 >> aux5;
		fTABLE[0][i] = aux0; fTABLE[1][i] = aux1;
		fTABLE[2][i] = aux2; fTABLE[3][i] = aux3;
		fTABLE[4][i] = aux4; fTABLE[5][i] = aux5;
	}
	streamer.close();
}

void ActCalibrations::ReadPadAlignCoefs(std::string &coefsFile)
{
	std::ifstream streamer(coefsFile.c_str());
	if(streamer.fail())
	{
		throw std::runtime_error("Error opening PadAligCoefs file!");
	}
	double aux0, aux1, aux2;
	for(int i = 0; i < ActParameters::NCoefRows; i++)
	{
		streamer >> aux0 >> aux1 >> aux2;
		fPadAlignCoefs[i][0] = aux0;
		fPadAlignCoefs[i][1] = aux1;
		fPadAlignCoefs[i][2] = aux2;
	}
	streamer.close();
}

void ActCalibrations::ReadSilicon01SCalibrations(std::string &coefsFile, std::string panel)
{
	if(!(std::find(ActParameters::SiCalNames.begin(), ActParameters::SiCalNames.end(), panel)
		 != ActParameters::SiCalNames.end()))
	{
		throw std::runtime_error("string passed to ActCalibratons::ReadSiliconCalibrations does not match 0,1 or S panels in ActParameters.h variable SiCalNames");
	}
	std::vector<std::vector<double>> aux(ActParameters::Si01SCalibrationRows[panel],
										 std::vector<double>(ActParameters::NcolsSi01S));//all have 4 columns
	std::ifstream streamer(coefsFile.c_str());
	if(streamer.fail())
	{
		throw std::runtime_error("Error opening Silicons 01S calibration files!");
	}
	double aux0, aux1, aux2, aux3;//4 columns always
	for(int i = 0; i < ActParameters::Si01SCalibrationRows[panel]; i++)
	{
		streamer >> aux0 >> aux1 >> aux2 >> aux3;
		aux[i][0] = aux0; aux[i][1] = aux1;
		aux[i][2] = aux2; aux[i][3] = aux3;
	}

	//write to map
	fSilicon01SCalibrations[panel] = aux;
	streamer.close();
}

void ActCalibrations::ReadSiliconBeamCalibrations(std::string &coefsFile)
{
	std::ifstream streamer(coefsFile.c_str());
	if(streamer.fail())
	{
		throw std::runtime_error("Fail opening Silicon Beam calibration file!");
	}
	double aux0, aux1;//2 columns
	for(int i = 0; i < ActParameters::NrowsSiBeam; i++)
	{
		streamer >> aux0 >> aux1;
		fSiliconBeamCalibrations[i][0] = aux0;
		fSiliconBeamCalibrations[i][1] = aux1;
	}
	streamer.close();
}

void ActCalibrations::InitDriftVelocityHist()
{
	fHistDrift = std::make_unique<TH2D>("fHistDrift", "Drift velocity",
										100, 0., ActParameters::g_NPADX,
										100, 0., ActParameters::g_NPADZ);
}

void ActCalibrations::FillDriftVelocityHist(std::vector<TrackPhysics>& tracks, Silicons& silicons)
{
	//if multiplicity in silicon is > 1, we cannot guarantee that hit silicon is number 4, so we skip event
	if(silicons.fData["S"]["M"] > 1)
	{
		//std::cout<<BOLDRED<<"Drift: M > 1"<<RESET<<'\n';
		return;
	}
	//this algorithm is designed for silicon 4!
	if(silicons.fData["S"]["P"] != 4)
	{
		return;
	}
	for(auto& track : tracks)
	{
		//silicon 4 is placed at left in E796
		if(track.fSiliconPlace == ActParameters::trackHitsSiliconSideLeft)
		{
		
			fHistDrift->Fill(track.fSiliconPoint.X(), track.fSiliconPoint.Z(),
							 silicons.fData["S"]["ES"]);//fill with energy
		}
	}
	
}

void ActCalibrations::ComputeDriftCoefsFromHist()
{
	double xMin { 30.}; int binXMin { fHistDrift->GetXaxis()->FindFixBin(xMin)};
	double xMax { 90.}; int binXMax { fHistDrift->GetXaxis()->FindFixBin(xMax)};
	double yMin { 200.}; int binYMin { fHistDrift->GetYaxis()->FindFixBin(yMin)};
	double yMax { 350.}; int binYMax { fHistDrift->GetYaxis()->FindFixBin(yMax)};

	double zMin { yMin};
	bool zMinIsFound { false};
	for(int y = binYMin; y <= binYMax; y++)
	{
		if(zMinIsFound)
			break;
		for(int x = binXMin; x <= binXMax; x++)
		{
			double content { fHistDrift->GetBinContent(x, y)};
			if(content <= 0.)
				continue;
			zMin = fHistDrift->GetYaxis()->GetBinCenter(y);
			zMinIsFound = true;
			break;
			
		}
	}
	double zMax { yMax};
	bool zMaxIsFound { false};
	for(int y = binYMax; y >= binYMin; y--)
	{
		if(zMaxIsFound)
			break;
		for(int x = binXMax; x >= binXMin; x--)
		{
			double content { fHistDrift->GetBinContent(x, y)};
			if(content <= 0.)
				continue;
			zMax = fHistDrift->GetYaxis()->GetBinCenter(y);
			zMaxIsFound = true;
			break;
			
		}
	}

	fZToLengthUnits =  ActParameters::zSiliconLength / static_cast<double>(zMax - zMin);
	fZToPadUnits = fZToLengthUnits / ActParameters::padSideLength ;	
}

void ActCalibrations::PlotDriftVelocityHist()
{
	gROOT->SetStyle("Plain");
	
	auto fCanvDrift = std::make_unique<TCanvas>("fCanvDrift", "Drift velocity", 1);
	fCanvDrift->cd();
	fHistDrift->Draw("colz");
	fCanvDrift->Update();
	fCanvDrift->cd();
	fCanvDrift->WaitPrimitive();
	fCanvDrift->Close();
	//unique_ptr automatically disallocates memory
}

void ActCalibrations::WriteDriftCoefsToFile(std::string fileName)
{
	std::ofstream streamer(fileName.c_str());
	streamer << fZToLengthUnits << '\n';
	streamer << fZToPadUnits << '\n';
	streamer.close();
}

void ActCalibrations::ReadDriftCoefsFromFile(std::string fileName)
{
	std::ifstream streamer(fileName.c_str());
	double aux;
	streamer >> aux;
	fZToLengthUnits = aux;
	streamer >> aux;
	fZToPadUnits = aux;
	streamer.close();
}
