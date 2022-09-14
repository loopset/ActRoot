#include "ActParameters.h"
#include "ActHit.h"
#include "ActEvent.h"
#include "ActCalibrations.h"
#include "ActStructs.h"
#include "ActClusteringResults.h"
#include "ActSample.h"
#include "ActRANSAC.h"
#include "ActDraw.h"

#include "cobo_libs/inc/MEvent.h"
#include "cobo_libs/inc/MEventReduced.h"

#include <TString.h>
#include <TROOT.h>
#include <TChain.h>
#include <TTree.h>

#include <iostream>
#include <map>
#include <string>

void MacroFull(int initRun, int endRun)
{
	//enable multi threading
	ROOT::EnableImplicitMT();

	//file names
	std::string tableCalibrationFile { "/media/Datos/ApuntesUSC/TESE/ACTAR_tracking/analysis_2022/ACTAR_ANALYSIS_LIGHT_root6/dat/LT.dat"};
	std::string padCalibrationFile { "/media/Datos/ApuntesUSC/TESE/ACTAR_tracking/analysis_2022/ACTAR_ANALYSIS_LIGHT_root6/root/CalibrationFiles_E796_v2022/pad_align_coef_v2022_V5.cal"};
	std::string  VXIActionFile { "/media/Datos/ApuntesUSC/TESE/ACTAR_tracking/analysis_2022/ACTAR_ANALYSIS_LIGHT_root6/root/ACTIONS_e796_run111.CHC_PAR"};
	std::map<std::string, std::string> siCalibrationFiles = {
		{"0", "/media/Datos/ApuntesUSC/TESE/ACTAR_tracking/analysis_2022/ACTAR_ANALYSIS_LIGHT_root6/root/CalibrationFiles_E796_v2022/Si0_Calibration_E796_2022_Feb25.cal"},
		{"1", "/media/Datos/ApuntesUSC/TESE/ACTAR_tracking/analysis_2022/ACTAR_ANALYSIS_LIGHT_root6/root/CalibrationFiles_E796_v2022/Si1_Calibration_E796_Nov_2021_Nov.cal"},
		{"S", "/media/Datos/ApuntesUSC/TESE/ACTAR_tracking/analysis_2022/ACTAR_ANALYSIS_LIGHT_root6/root/CalibrationFiles_E796_v2022/SiS_Calibration_E796_2022_Feb28_NewGain.cal"}
	};
	std::string siBeamCalibrationFile { "/media/Datos/ApuntesUSC/TESE/ACTAR_tracking/analysis_2022/ACTAR_ANALYSIS_LIGHT_root6/root/calibrationFiles_21/SiBeam_calibration.cal"};

	//////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////// READ CALIBRATIONS ////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////
	ActCalibrations calibrations;
	//Look up table
	calibrations.ReadTABLE(tableCalibrationFile);
	//Pads calibration
	calibrations.ReadPadAlignCoefs(padCalibrationFile);
	//silicons 0, 1 and S
	for(auto& el : ActParameters::SiCalNames)
	{
		calibrations.ReadSilicon01SCalibrations(siCalibrationFiles[el], el);
	}
	//si beam
	calibrations.ReadSiliconBeamCalibrations(siBeamCalibrationFile);
	//////////////////////////////////////////////////////////////////////////////////
	/////////////////////////// END READ CALIBRATIONS ////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////

	//chain to store trees
	TChain ch("ACTAR_TTree");
	int NumberOfFiles { 0};
	for (int i = initRun; i <= endRun; i++)
	{
		ch.Add(TString::Format("/media/Datos/ApuntesUSC/TESE/ACTAR_tracking/analysis_2022/Data/Tree_Run_%04d_Merged.root", i));
		NumberOfFiles++;
	}
	std::cout<<"Number of files to process: "<<NumberOfFiles<<'\n';

	// auto test { calibrations.GetSilicon01SCalibrations()};
	// for(auto& el : test["S"])
	// {
	// 	for(auto& e : el)std::cout<<"CalS: "<<e<<'\n';
	// }

	/////////////////////////////////////////////////////////////////////////////////
	///////////////////// INITIALIZE DRAWING STRUCTURE //////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////
	ActDraw painter;
	painter.Init();
	/////////////////////////////////////////////////////////////////////////////////
	//////////////////////// FINISH INIT DRAW STRUCTURE /////////////////////////////
	
	/////////////////////////////////////////////////////////////////////////////////
	////////////////////////////// LOOP OVER EVENTS /////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////

	//auxiliar pointers and MEvents needed
	TTree* runTree;
	//pointer to MEventReduced
 	MEventReduced* myEventReduced { nullptr};
	//getVXI indexes with MEvent
	MEvent* myEvent { new MEvent()};
	myEvent->InitVXIParameters(VXIActionFile.data(), ActParameters::VXINames, ActParameters::VXINumbers);
	int entryNumber { 0};
	auto pad { calibrations.GetPadAlignCoefs()};
	for(int run = 0; run < NumberOfFiles; run++)
	{
		std::cout<<BOLDGREEN<<"Processing file number: "<<run<<RESET<<'\n';
		//read TTree containing entryNumber
		ch.LoadTree(entryNumber);
		runTree = ch.GetTree();
		runTree->SetBranchAddress("data", &myEventReduced);
		int entriesInRun { static_cast<int>(runTree->GetEntries())};
		entryNumber += entriesInRun;
		std::cout<<BOLDGREEN<<"\t with "<<entriesInRun<<" events"<<RESET<<'\n';

		//iterate over run entries = events
		for(int iEvent = 0; iEvent < entriesInRun; iEvent++)
		{
			runTree->GetEntry(iEvent);
			if(!(iEvent % 1000)) std::cout<<BOLDGREEN<<"At event "<<iEvent<<RESET<<'\n';
			
			ActEvent event;
			event.ReadEvent(calibrations, myEvent, myEventReduced);
			TriggersAndGates triggers = { event.GetEventTriggers()};
			//choose only GATCONF = 8
			if(triggers.GATCONF != 8) continue;

			//////// CALIBRATE AND READ SILICONS ///////
			event.CalibrateSilicons(calibrations);
			event.ReadSiliconsData();
			//test
			// Silicons sil { event.GetEventSilicons()};
			// //for(auto& s : sil.fSi0_cal) std::cout<<"Value 0 calibrated: "<<s<<'\n';
			// std::cout<<"Data SiS: "<<sil.fData["S"]["ES"]<<'\n';


			//////// RANSAC ///////
			SampleConsensus::ActRANSAC estimator { 500, 15, 4.};
			estimator.SetSampleMethod(RandomSampling::SamplingMethod::kGaussian);
			auto out = estimator.Solve(event.GetEventHits());

			//painter.DrawEvent(event.GetEventHits());
			painter.DrawResults(event.GetEventHits(), out);

			
		}
	}
	
}
