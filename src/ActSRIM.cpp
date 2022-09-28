#include "ActSRIM.h"

#include <TCanvas.h>
#include <TF1.h>
#include <TSpline.h>
#include <fstream>
#include <memory>
#include <vector>

void ActSRIM::ReadInterpolations(std::string key, std::string fileName)
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
	fInterpolationsDirect[key] = std::make_unique<TF1>(("func_direct_" + key).c_str(),
													   [&](double* x, double* p)
													   {return fSplinesDirect[key]->Eval(x[0]);},
													   0., 1000., 1);//between [0,1000] MeV

	fSplinesInverse[key] = std::make_unique<TSpline3>(("spinverse" + key).c_str(),
											   &(vaux1[0]),//range
											   &(vaux0[0]),//energy
											   vaux0.size(),
											   "b2, e2",
											   0., 0.);
	fInterpolationsInverse[key] = std::make_unique<TF1>(("func_inverse_" + key).c_str(),
													   [&](double* x, double* p)
													   {return fSplinesInverse[key]->Eval(x[0]);},
													   0., 1000., 1);//between [0,1000] MeV
	
}

void ActSRIM::DrawDirect(std::string key)
{
	auto fCanvDirect = std::make_unique<TCanvas>("fCanvDirect", ("Direct spline for " + key).c_str(), 1);
	fCanvDirect->cd();
	fInterpolationsDirect[key]->SetLineColor(kRed);
	fInterpolationsDirect[key]->SetLineWidth(2);
	fInterpolationsDirect[key]->Draw();
	fCanvDirect->Update();
	fCanvDirect->WaitPrimitive();
	fCanvDirect->Close();
	//unique_ptr automatically manages deletion of pointer
}

void ActSRIM::DrawInverse(std::string key)
{
	auto fCanvInverse = std::make_unique<TCanvas>("fCanvInverse", ("Inverse spline for " + key).c_str(), 1);
	fCanvInverse->cd();
	fInterpolationsInverse[key]->SetLineColor(kBlue);
	fInterpolationsInverse[key]->SetLineWidth(2);
	fInterpolationsInverse[key]->Draw();
	fCanvInverse->Update();
	fCanvInverse->WaitPrimitive();
	fCanvInverse->Close();
	//unique_ptr automatically manages deletion of pointer
}
