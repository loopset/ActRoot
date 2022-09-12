#include "ActHit.h"
#include "ActParameters.h"
#include "ActEvent.h"
#include "ActCalibrations.h"
#include "ActDraw.h"

#include "cobo_libs/inc/MEventReduced.h"

#include <TString.h>
#include <TROOT.h>
#include <TChain.h>
#include <TTree.h>

#include <iostream>
#include <string>

void MacroFull(int initRun, int endRun)
{
	//enable multi threading
	ROOT::EnableImplicitMT();

	//file names
	std::string tableCalibrationFile { "/media/Datos/ApuntesUSC/TESE/ACTAR_tracking/analysis_2022/ACTAR_ANALYSIS_LIGHT_root6/dat/LT.dat"};
	std::string padCalibrationFile { "/media/Datos/ApuntesUSC/TESE/ACTAR_tracking/analysis_2022/ACTAR_ANALYSIS_LIGHT_root6/root/CalibrationFiles_E796_v2022/pad_align_coef_v2022_V5.cal"};

	//chain to store trees
	TChain ch("ACTAR_TTree");
	int NumberOfFiles { 0};
	for (int i = initRun; i <= endRun; i++)
	{
		ch.Add(TString::Format("/media/Datos/ApuntesUSC/TESE/ACTAR_tracking/analysis_2022/Data/Tree_Run_%04d_Merged.root", i));
		NumberOfFiles++;
	}
	std::cout<<"Number of files to process: "<<NumberOfFiles<<'\n';

	//READ CALIBRATIONS
	ActCalibrations calibrations;
	//Look up table
	calibrations.ReadTABLE(tableCalibrationFile);
	//Pads calibration
	calibrations.ReadPadAlignCoefs(padCalibrationFile);

	//initialize drawing structure
	ActDraw painter;
	painter.Init();

	//iterate over runs
	TTree* runTree;
	//pointer to MEventReduced
	MEventReduced* myEventReduced { nullptr};
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
		std::cout<<BOLDGREEN<<"\t with "<<entriesInRun<<" entries"<<RESET<<'\n';

		//iterate over run entries = events
		for(int iEvent = 0; iEvent < entriesInRun; iEvent++)
		{
			runTree->GetEntry(iEvent);
			if(!(iEvent % 1000)) std::cout<<iEvent<<'\n';
			ActEvent event;
			event.ReadEvent(calibrations, myEventReduced);

			//draw
			painter.DrawEvent(event.GetEventHits());
		}
	}
	
}
