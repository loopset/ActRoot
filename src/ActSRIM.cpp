#include "ActSRIM.h"

#include <TCanvas.h>
#include <TF1.h>
#include <TSpline.h>
#include <TLegend.h>

#include <fstream>
#include <memory>
#include <vector>

void ActSRIM::ReadInterpolations(std::string key, std::string fileName)
{
	std::ifstream streamer(fileName.c_str());
	double aux0, aux1;
	while(streamer >> aux0 >> aux1)
	{
		fXAxis[key].push_back(aux0);
		fYAxis[key].push_back(aux1);
	}
	streamer.close();

	fSplinesDirect[key] = std::make_unique<TSpline3>(("spdirect" + key).c_str(),
													 &(fXAxis[key][0]),//energy
													 &(fYAxis[key][0]),//range
													 fXAxis[key].size(),
													 "b2, e2",
													 0., 0.);
	fInterpolationsDirect[key] = std::make_unique<TF1>(("func_direct_" + key).c_str(),
													   [key, this](double* x, double* p)
													   {return fSplinesDirect[key]->Eval(x[0]);},
													   0., 1000., 1);//between [0,1000] MeV

	fSplinesInverse[key] = std::make_unique<TSpline3>(("spinverse" + key).c_str(),
													  &(fYAxis[key][0]),//range
													  &(fXAxis[key][0]),//energy
													  fYAxis[key].size(),
													  "b2, e2",
													  0., 0.);
	fInterpolationsInverse[key] = std::make_unique<TF1>(("func_inverse_" + key).c_str(),
														[key, this](double* x, double* p)
													   {return fSplinesInverse[key]->Eval(x[0]);},
													   0., 1000., 1);//between [0,1000] MeV
}

void ActSRIM::DrawDirect(std::vector<std::string> keys)
{
	auto fCanvDirect = std::make_unique<TCanvas>("fCanvDirect", "Direct splines", 1);
	auto legend = std::make_unique<TLegend>(0.7, 0.7, 0.9, 0.9);
	legend->SetFillStyle(0);
	fCanvDirect->cd();
	int counter {1};
	for(auto& key : keys)
	{
		fInterpolationsDirect[key]->SetLineColor(counter);
		fInterpolationsDirect[key]->SetLineWidth(2);
		fInterpolationsDirect[key]->Draw((counter > 1) ? "same" : "l");
		legend->AddEntry(fInterpolationsDirect[key].get(), key.c_str(), "lp");
		counter++;
	}
	legend->Draw();
	fCanvDirect->Update();
	fCanvDirect->WaitPrimitive("lat", "");
	fCanvDirect->Close();
	legend.reset();
	//unique_ptr automatically manages deletion of pointer
}

void ActSRIM::DrawInverse(std::vector<std::string> keys)
{
	auto fCanvInverse = std::make_unique<TCanvas>("fCanvInverse", "Inverse splines", 1);
	auto legend = std::make_unique<TLegend>(0.7, 0.7, 0.9, 0.9);
	legend->SetFillStyle(0);
	fCanvInverse->cd();
	int counter {1};
	for(auto& key : keys)
	{
		std::cout<<key<<'\n';
		fInterpolationsInverse[key]->SetLineColor(counter);
		fInterpolationsInverse[key]->SetLineWidth(2);
		fInterpolationsInverse[key]->Draw((counter > 1) ? "same" : "");
		legend->AddEntry(fInterpolationsInverse[key].get(), key.c_str(), "l");
		counter++;
	}
	legend->Draw();
	fCanvInverse->Update();
	fCanvInverse->WaitPrimitive("lat", "");
	fCanvInverse->Close();
	legend.reset();
	//unique_ptr automatically manages deletion of pointer
}
