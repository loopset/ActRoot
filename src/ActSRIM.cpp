#include "ActSRIM.h"

#include <TF1.h>
#include <TSpline.h>
#include <fstream>
#include <memory>
#include <vector>

void ActSRIM::ReadInterpolations(std::string fileName, std::string key)
{
	std::ifstream streamer(fileName.c_str());
	double aux0, aux1;
	std::vector<double> vaux0, vaux1;
	while(streamer >> aux0 >> aux1)
	{
		vaux0.push_back(aux0);
		vaux1.push_back(aux1);
	}
	streamer.close();

	fSplinesDirect[key] = std::make_unique<TSpline3>(("spdirect" + key).c_str(),
											   &(vaux0[0]),//energy
											   &(vaux1[0]),//range
											   vaux0.size(),
											   "b2, e2",
											   0., 0.);
	fInterpolationsDirect[key] = std::make_unique<TF1>(("funcdirect" + key).c_str(),
													   [&](double* x, double* p)
													   {return fSplinesDirect[key]->Eval(x[0]);},
													   0., 1000., 1);//between [0,1000] MeV

	fSplinesInverse[key] = std::make_unique<TSpline3>(("spinverse" + key).c_str(),
											   &(vaux1[0]),//range
											   &(vaux0[0]),//energy
											   vaux0.size(),
											   "b2, e2",
											   0., 0.);
	fInterpolationsInverse[key] = std::make_unique<TF1>(("funcinverse" + key).c_str(),
													   [&](double* x, double* p)
													   {return fSplinesInverse[key]->Eval(x[0]);},
													   0., 1000., 1);//between [0,1000] MeV
	
}
