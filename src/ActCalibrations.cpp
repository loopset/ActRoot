#include "ActCalibrations.h"

#include "ActParameters.h"
#include "ActStructs.h"
#include "ActTrackGeometry.h"
#include "TString.h"

#include <TH2.h>
#include <TCanvas.h>

#include <TROOT.h>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
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

void ActCalibrations::ReadSiliconSideCalibrations(const std::string &file)
{
    std::ifstream streamer {file.c_str()};
    if(!streamer)
    {
        throw std::runtime_error("Error opening Silicons 01S calibration files!");
    }
    std::string line {};
    while(std::getline(streamer, line, '\n'))
    {
        std::istringstream lineStreamer {line};
        std::string value {};
        int column {0};
        std::string side {};
        int index {}; double p0 {}; double p1 {}; double pec {}; double sigmaPec {};
        while(std::getline(lineStreamer, value, '\t'))
        {
            switch (column)
            {
            case 0:
                side = value;
                break;
            case 1:
                index = std::stoi(value) - 1;//to work always with 0...5 values, indexes of vector
                break;
            case 2:
                p0 = std::stod(value);
                break;
            case 3:
                p1 = std::stod(value);
                break;
            case 4:
                pec = std::stod(value);
                break;
            case 5:
                sigmaPec = std::stod(value);
                break;
            default:
                continue;
            }
            column++;
        }
        fSiliconSideCalibrations[side][index] = std::vector<double>{p0, p1, pec, sigmaPec};
    }
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
    int silIndex {2};
    auto sideFinder = [&](const TrackPhysics& tr)
    {
        auto silPlace {TString(tr.fSiliconPlace)};
        std::string side {};
        if(silPlace.Contains("left"))
            side = "left";
        else if(silPlace.Contains("right"))
            side = "right";
        else
            side = "none";
        return side;
    };
    for(const auto& track : tracks)
    {
        auto side { sideFinder(track)};
        if(side == "none")
            continue;
        //check multiplicity
        if(silicons.fData.at(side).at("M") > 1)
            continue;;
        //check silicon number matches desired by us
        if(silicons.fData.at(side).at("P") != silIndex)
            continue;;

        //otherwise, fill
        fHistDrift->Fill(track.fSiliconPoint.X(), track.fSiliconPoint.Z(),
							 silicons.fData.at(side).at("E"));
    }
}

void ActCalibrations::FillDriftVelocityHist(const ActTrackGeometry& track, const Silicons& silicons)
{
    //sil index to fill histogram (for both sides!)
    int chosenSil {2};//it is at center for both sides
    if(track.fSilIndex == chosenSil)
    {
        fHistDrift->Fill(track.fSiliconPoint.X(), track.fSiliconPoint.Z(),
                         silicons.fData.at(track.fSiliconPlace).at("E"));
    
    }
}

void ActCalibrations::ComputeDriftCoefsFromHist()
{
	double xMin { 30.}; int binXMin { fHistDrift->GetXaxis()->FindFixBin(xMin)};
	double xMax { 90.}; int binXMax { fHistDrift->GetXaxis()->FindFixBin(xMax)};
	double yMin { 200.}; int binYMin { fHistDrift->GetYaxis()->FindFixBin(yMin)};
	double yMax { 310.}; int binYMax { fHistDrift->GetYaxis()->FindFixBin(yMax)};

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

void ActCalibrations::ComputeZDriftCoefsFromDriftVelocity(const std::string &fileName)
{
    //file must contain the following information
    std::ifstream streamer {fileName.c_str()};
    if(!streamer)
    {
        throw std::runtime_error("Error reading drift coefficients from file!");
    }
    double vdrift {-1};
    double freq   {-1};
    std::string line {};
    int row {0};
    while(std::getline(streamer, line, '\n'))
    {
        std::istringstream lineStreamer {line};
        std::string value {};
        int column {0};
        while(std::getline(lineStreamer,value, ' '))
        {
            if(column == 1)
            {
                switch (row)
                {
                case 0:
                    freq = std::stod(value);
                    break;
                case 1:
                    vdrift = std::stod(value);
                    break;
                default:
                    break;
                }
            }
            column++;
        }
        row++;
    }
    std::cout<<"Drift velocity : "<<vdrift<<" cm/mus"<<'\n';
    std::cout<<"Samplig freq   : "<<freq<<" MHz"<<'\n';
    //v is given in cm/us
    //freq in MHz, so we only need to convert cm -> mm
    fZToLengthUnits = 10.0 * vdrift / freq;
    fZToPadUnits    = fZToLengthUnits / ActParameters::padSideLength;
}
