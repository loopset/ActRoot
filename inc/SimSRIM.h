#ifndef SIMSRIM_H
#define SIMSRIM_H

/* New version of ActSRIM
intented to be more complete including
stragglings and stopping powers!

It should replace ActSRIM in ActROOT
*/

#include "TF1.h"
#include "TSpline.h"

#include <cstring>
#include <map>
#include <memory>
#include <string>
#include <vector>

class SimSRIM {
    
private:
    std::vector<std::string> fKeys;
	//Energy->Range
	std::map<std::string, std::unique_ptr<TSpline3>> fSplinesDirect;
	std::map<std::string, std::unique_ptr<TF1>> fInterpolationsDirect;
	//Range->Energy
	std::map<std::string, std::unique_ptr<TSpline3>> fSplinesInverse;
	std::map<std::string, std::unique_ptr<TF1>> fInterpolationsInverse;
    //Energy->Stopping powers (nuclear + electronic)
    std::map<std::string, std::unique_ptr<TSpline3>> fSplinesStoppings;
	std::map<std::string, std::unique_ptr<TF1>> fStoppings;
    //Range->Longitudinal straggling
    std::map<std::string, std::unique_ptr<TSpline3>> fSplinesLongStrag;
	std::map<std::string, std::unique_ptr<TF1>> fLongStrag;
    //Range->Lateral straggling
    std::map<std::string, std::unique_ptr<TSpline3>> fSplinesLatStrag;
	std::map<std::string, std::unique_ptr<TF1>> fLatStrag;
    

public:
	SimSRIM() = default;
	~SimSRIM() = default;

	void ReadInterpolations(std::string key, std::string fileName);
    
	double EvalDirect(std::string key, double energy) { CheckFunctionArgument(energy); return fInterpolationsDirect[key]->Eval(energy); }
	double EvalInverse(std::string key, double range) { CheckFunctionArgument(range); return fInterpolationsInverse[key]->Eval(range);}

    //for new functions
    double EvalStoppingPower(std::string key, double energy){ CheckFunctionArgument(energy); return fStoppings[key]->Eval(energy); }
    double EvalLongStraggling(std::string key, double range){ CheckFunctionArgument(range); return fLongStrag[key]->Eval(range); }
    double EvalLatStraggling(std::string key, double range){ CheckFunctionArgument(range); return fLatStrag[key]->Eval(range); }

    void Draw(std::string what, std::vector<std::string> keys = {});

    double ComputeEnergyLoss(double Tini, std::string material, double thickness, int steps = 10);

    void CheckKeyIsStored(std::string key);

    void CheckFunctionArgument(double val);
};

#endif
