#include <RtypesCore.h>
#include <TInterpreter.h>
#include <TStopwatch.h>
#include <TString.h>
void RunRANSAC()
{
	gSystem->SetFlagsOpt("-O2 -std=c++17");
	TString pwd { gSystem->pwd()};
	TString inc { pwd  + "/inc/"};
	//std::cout<<inc<<'\n';
	gSystem->AddIncludePath(" -I" + inc);
	//gInterpreter->AddIncludePath(inc);
	gROOT->ProcessLine(".L ./build/libActRoot.so");

	//run or compile macros
	gROOT->ProcessLine(".L MacroRANSAC.cpp++");
	gROOT->ProcessLine("MacroRANSAC()");
}
