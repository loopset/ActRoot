#ifndef MEXPERIMENT_H
#define MEXPERIMENT_H

#include <stdio.h>
#include <iostream>
#include <vector>
#include <string.h>
#include <TObject.h>
#include <TString.h>
#include <TTree.h>
#include <TFile.h>
#include <sys/time.h>
#include <DataParameters.h>
using namespace std;

#include <MEvent.h>
#include <MFile.h>
#include <MUnpacker.h>
#include <MVisu.h>

class MExperiment
{
	public:
	MExperiment(int, char**);
	~MExperiment();
	
	void TreatRuns();
	bool TreatEvent(MFMCommonFrame *);
	void OpenNewTree(long);
	void CloseTree();
	void EndOfExperiment();

	int Experiment_Number;
	TString Experiment_Name;
	TString Base_Path;
	TString Tree_Base_Path;
	TString SingleFileName;

	MEvent Event;
	MFile* File;
	MUnpacker* Unpacker;
	MVisu* Visu;
	TTree* Tree;
	TFile* TreeFile;
	
	int runf;
	int runl;	

	bool VisuOpt;
	bool TreeOpt;
	bool FullTreeOpt;
	bool SingleFile;
	bool IsFastPeak;
	long int SplitRuns;


	//for the Time measurement
	struct timeval  tv1, tv2, tv3, tv4;
	double tdiff1, tdiff2, tdiff3, tdiff4;

	
};



#endif
