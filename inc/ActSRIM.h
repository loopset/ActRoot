#ifndef ACTSRIM_H
#define ACTSRIM_H

//Class for interacting with SRIM files
#include <TF1.h>
#include <TSpline.h>

#include <map>
#include <memory>
#include <string>

class ActSRIM
{
protected:
	//direct functions
	std::map<std::string, std::unique_ptr<TSpline3>> fSplinesDirect;
	std::map<std::string, std::unique_ptr<TF1>> fInterpolationsDirect;
	std::map<std::string, std::unique_ptr<TSpline3>> fSplinesInverse;
	std::map<std::string, std::unique_ptr<TF1>> fInterpolationsInverse;

public:
	ActSRIM() = default;
	~ActSRIM() = default;

	void ReadInterpolations(std::string fileName, std::string key);
	double EvalDirect(std::string key, double energy) { return fInterpolationsDirect[key]->Eval(energy); }
	double EvalInverse(std::string key, double range) { return fInterpolationsInverse[key]->Eval(range);}
};

#endif //ACTSRIM_H
