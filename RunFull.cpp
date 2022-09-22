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
	//gSystem->AddIncludePath(" -I/media/Datos/ApuntesUSC/TESE/ACTAR_tracking/analysis_2022/ACTAR_ANALYSIS_LIGHT_root6/ActRoot/cobo_libs/inc");
	//gSystem->AddIncludePath(" -I/media/Datos/ApuntesUSC/TESE/ACTAR_tracking/analysis_2022/ACTAR_ANALYSIS_LIGHT_root6/ActRoot/cobo_libs/libMFM");
	gInterpreter->AddIncludePath(inc);
	//gROOT->ProcessLine(".L ./cobo_libs/libMFM/libMFM.so");
	gROOT->ProcessLine(".L ../MyEvents/build/libMyActarEvents.so");
	gROOT->ProcessLine(".L ./build/libActRoot.so");

	//run or compile macros
	gROOT->ProcessLine(".L MacroFull.cpp++");
	gROOT->ProcessLine(TString::Format("MacroFull(%d, %d)", initRun, endRun));
}
