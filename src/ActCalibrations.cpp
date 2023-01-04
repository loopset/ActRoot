#include "ActCalibrations.h"

#include "ActParameters.h"
#include "ActStructs.h"
#include "ActTrackPlus.h"
#include "TString.h"

#include <TH2.h>
#include <TCanvas.h>

#include <TROOT.h>
#include <cmath>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <vector>
#include <string>

void ActCalibrations::ReadTABLE(std::string& tableFile)
{
    //Initialize table
    fTABLE = std::vector<std::vector<int>>(6, std::vector<int>(ActParameters::g_NB_COBO *
                                                               ActParameters::g_NB_ASAD *
                                                               ActParameters::g_NB_AGET *
                                                               ActParameters::g_NB_CHANNEL, 0));
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
    //initialize
    fPadAlignCoefs = {};
    std::ifstream streamer(coefsFile.c_str());
	if(streamer.fail())
	{
		throw std::runtime_error("Error opening PadAligCoefs file!");
	}
    //new version with auto column and row number
    std::string line {};
    int row {};
    while(std::getline(streamer, line, '\n'))
    {
        std::istringstream lineStreamer {line};
        std::string value {};
        int column {};
        fPadAlignCoefs.push_back({});
        while(std::getline(lineStreamer, value, ' '))
        {
            //std::cout<<"Row: "<<row<<" column: "<<column<<" value: "<<value<<'\n';
            fPadAlignCoefs.at(row).push_back(std::stod(value));
            column++;
        }
        row++;
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
    //initialize
    fSiliconBeamCalibrations = std::vector<std::vector<double>>(ActParameters::NrowsSiBeam, std::vector<double>(ActParameters::NcolsSiBeam));
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

void ActCalibrations::InitDriftVelocityHist(const std::string& silSide, const int& silIndex)
{
    fSilSide  = silSide;
    fSilIndex = silIndex;
	fHistDrift = std::make_unique<TH2D>("fHistDrift", ("XZ Silicon Point for " + fSilSide + " and index " + fSilIndex),
										ActParameters::g_NPADX, 0., ActParameters::g_NPADX,
										ActParameters::g_NBINSZ, 0., ActParameters::g_NPADZ);
}


void ActCalibrations::FillDriftVelocityHist(const ActTrackPlus& track, const Silicons& silicons)
{
    //sil index to fill histogram (for both sides!)
    int chosenSil {2};//it is at center for both sides
    if(track.fSiliconIndex == chosenSil)
    {
        fHistDrift->Fill(track.fSiliconPoint.X(), track.fSiliconPoint.Z(),
                         silicons.fData.at(track.fSiliconSide).at("E"));
    
    }
}

void ActCalibrations::FillDriftVelocityHistPlus(const XYZPoint& point, const std::string& silSide, const int& silIndex, const double& silEnergy)
{
    if(silSide == fSilSide && silIndex == fSilIndex)
    {
        fHistDrift->Fill(point.X(), point.Z(),
                         silEnergy);
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

void ActCalibrations::ReadDriftCoefsFromFile(const std::string& fileName)
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
    //v is given in cm/us
    //freq in MHz, so we only need to convert cm -> mm
    fZToLengthUnits = 10.0 * vdrift / freq;
    fZToPadUnits    = fZToLengthUnits / ActParameters::padSideLength;
    //print
    std::cout<<BOLDGREEN<<"==== Auto drift parameters ===="<<'\n';
    std::cout<<"Drift velocity     : "<<vdrift<<" cm/mus"<<'\n';
    std::cout<<"Samplig freq       : "<<freq<<" MHz"<<'\n';
    std::cout<<"Time buckets to mm : "<<fZToLengthUnits<<" mm/tb"<<'\n';
    std::cout<<" ==================== "<<RESET<<std::endl;
}

void ActCalibrations::ReadPileUpSetup(const std::string &fileName)
{
    //file must contain in the following order
    // zMean(time buckets): xx.x (separated by a blank space!)
    // zWidth(time buckets): zz.z
    std::ifstream streamer {fileName.c_str()};
    if(!streamer)
    {
        throw std::runtime_error("Error reading PileUp configuration from file!");
    }
    double mean {-1};
    double width   {-1};
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
                    mean = std::stod(value);
                    break;
                case 1:
                    width = std::stod(value);
                    break;
                default:
                    break;
                }
            }
            column++;
        }
        row++;
    }
    //v is given in cm/us
    //freq in MHz, so we only need to convert cm -> mm
    fZPileUpMean  = mean;
    fZPileUpWidth = width;
    //print
    std::cout<<BOLDCYAN<<"== PileUp Configuration =="<<'\n';
    std::cout<<" Read file : "<<fileName<<'\n';
    std::cout<<" zMean     : "<<fZPileUpMean<<" tb"<<'\n';
    std::cout<<" zWidth    : "<<fZPileUpWidth<<" tb"<<'\n';
    std::cout<<"========================"<<RESET<<std::endl;
}

void ActCalibrations::ReadManualDriftParameters(const std::string &fileName)
{
    std::ifstream streamer {fileName.c_str()};
    if(!streamer)
    {
        throw std::runtime_error("Error reading MANUAL drift coefficients from file!");
    }
    double realSize {-1};
    double zMax {-1};
    double zMin   {-1};
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
                    realSize = std::stod(value);
                    break;
                case 1:
                    zMax = std::stod(value);
                    break;
                case 2:
                    zMin = std::stod(value);
                    break;
                default:
                    break;
                }
            }
            column++;
        }
        row++;
    }
    fZToLengthUnits = realSize / std::abs(zMax - zMin);
    fZToPadUnits    = fZToLengthUnits / ActParameters::padSideLength;
    //print
    std::cout<<BOLDGREEN<<"==== Manual drift parameters ===="<<'\n';
    std::cout<<"Real Z Sil size    : "<<realSize<<" mm"<<'\n';
    std::cout<<"Histo Z Max        : "<<zMax<<" tb"<<'\n';
    std::cout<<"Histo Z Min        : "<<zMin<<" tb"<<'\n';
    std::cout<<"Time buckets to mm : "<<fZToLengthUnits<<" mm/tb"<<'\n';
    std::cout<<"============================="<<RESET<<std::endl;
}
