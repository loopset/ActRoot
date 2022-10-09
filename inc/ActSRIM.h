#ifndef ACTSRIM_H
#define ACTSRIM_H

//Class for interacting with SRIM files
#include <TF1.h>
#include <TSpline.h>

#include <map>
#include <memory>
#include <string>
#include <vector>

class ActSRIM
{
protected:
	//store values of interpolations
	std::map<std::string, std::vector<double>> fXAxis {};
	std::map<std::string, std::vector<double>> fYAxis {};
	//direct functions
	std::map<std::string, std::unique_ptr<TSpline3>> fSplinesDirect;
	std::map<std::string, std::unique_ptr<TF1>> fInterpolationsDirect;
	//inverse functions
	std::map<std::string, std::unique_ptr<TSpline3>> fSplinesInverse;
	std::map<std::string, std::unique_ptr<TF1>> fInterpolationsInverse;

public:
	ActSRIM() = default;
	~ActSRIM() = default;

	void ReadInterpolations(std::string key, std::string fileName);
	double EvalDirect(std::string key, double energy) { return fInterpolationsDirect[key]->Eval(energy); }
	double EvalInverse(std::string key, double range) { return fInterpolationsInverse[key]->Eval(range);}

	void DrawDirect(std::vector<std::string> keys);
	void DrawInverse(std::vector<std::string> keys);

};

#endif //ACTSRIM_H
