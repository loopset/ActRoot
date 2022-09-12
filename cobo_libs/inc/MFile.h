///////////////////////////////////////
//                                   //
// T. Roger - GANIL 2015             //
// email: roger@ganil.fr             //
//                                   //
// MFile class definition            //
//                                   //
///////////////////////////////////////

#ifndef MFILE_H
#define MFILE_H

#include <iostream>
#include <vector>

#include <TString.h>
#include <TObjArray.h>
#include <TObjString.h>
#include <TSystem.h>
#include <TList.h>

#include <MFMCommonFrame.h>

#ifdef CC_IN2P3
#include "XrdPosix.hh"
#endif

using namespace std;



class MFile
{
	public:
	MFile(TString, int, int);
	MFile(TString);
	~MFile();
	long int OpenNext(TString);
	int GetNextBuffer(MFMCommonFrame *);
	int GetPrevBuffer(MFMCommonFrame *, int);
	int SetConfigDirectories(std::string="");
	TString LSCommand(int, TString, TString);
	
	char* vector;
	char ** pvector;
	int vectorsize;
	
//*********** user parameters ************//
	
	long int run_number;
	long int prev_run_number=-1;
	bool isNewRun=false;
	bool isSingleFile;

	int nFrames;

	int fLun;
	bool EOFreached;
		
	TString runpath;
	TString ACTIONpath;
	
	TString LSbase;
	
	TString ACTIONfileName;
	std::vector<TString> List;	
};

#endif
