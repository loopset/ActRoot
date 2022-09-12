#include <RtypesCore.h>
#include <TSystem.h>
#include <TROOT.h>
#include <TStopwatch.h>
#include <TString.h>
void RunFull(int initRun, int endRun)
{
	gSystem->SetFlagsOpt("-O2 -std=c++17");
	TString pwd { gSystem->pwd()};
	TString inc { pwd  + "/inc/"};
	//std::cout<<inc<<'\n';
	gSystem->AddIncludePath(" -I" + inc);
	//gInterpreter->AddIncludePath(inc);
	gROOT->ProcessLine(".L ./cobo_libs/libMFM/libMFM.so");
	gROOT->ProcessLine(".L ./cobo_libs/libACTARshared.so");
	gROOT->ProcessLine(".L ./build/libActRoot.so");

	//run or compile macros
	gROOT->ProcessLine(".L MacroFull.cpp+");
	gROOT->ProcessLine(TString::Format("MacroFull(%d, %d)", initRun, endRun));
}
