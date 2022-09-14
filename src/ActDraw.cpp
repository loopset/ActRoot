#include "ActDraw.h"

#include "ActParameters.h"
#include "ActHit.h"
#include "ActLine.h"
#include <algorithm>
#include "ActTrack.h"

#include <RtypesCore.h>
#include <TCanvas.h>
#include <TH2.h>
#include <TPolyLine.h>
#include <TStyle.h>
#include <TString.h>
#include <TH3.h>
#include <TPolyLine3D.h>

#include <cmath>
#include <map>
#include <stdexcept>
#include <vector>
#include <memory>
#include <iostream>

ActDraw::ActDraw()
	: fCanvPrecluster(nullptr),
	  fHistPad(nullptr), fHistProfile(nullptr),
	  fHistFront(nullptr),
	  ///////////////////
	  fCanvAllcluster(nullptr), fHistPadAllcluster(nullptr),
	  fHistFrontAllcluster(nullptr), fHistProfileAllcluster(nullptr),
	  fCanv3DResults(nullptr), fHist3DPrecluster(nullptr), fHist3DAftercluster(nullptr)
{
}

ActDraw::~ActDraw() = default;

void ActDraw::Init()
{
	//initialize canvas
	fCanvPrecluster = std::make_unique<TCanvas>("fCanvPrecluster", "Event before clustering", 1);
	fCanvPrecluster->Divide(3, 1);
	
	//initialize histos
	fHistPad = std::make_unique<TH2D>("fHistPad", "Pad: XY", fNbinsX, fMinX, fMaxX, fNbinsY, fMinY, fMaxY);
	fHistProfile = std::make_unique<TH2D>("fHistProfile", "Profile: XZ", fNbinsX, fMinX, fMaxX, fNbinsZ, fMinZ, fMaxZ);
	fHistFront = std::make_unique<TH2D>("fHistFront", "Front: YZ", fNbinsY, fMinY, fMaxY, fNbinsZ, fMinZ, fMaxZ);

	///////////////////////////////////////////
	fCanvAllcluster = std::make_unique<TCanvas>("fCanvAllcluster", "Canvas with full info", 1);
	fCanvAllcluster->Divide(3, 2);
	
	fHistPadAllcluster = std::make_unique<TH2I>("fHistPadAllcluster", "Clusters in pad", fNbinsX, fMinX, fMaxX, fNbinsY, fMinY, fMaxY);
	fHistFrontAllcluster = std::make_unique<TH2I>("fHistFrontAllcluster", "Clusters in front", fNbinsY, fMinY, fMaxY, fNbinsZ, fMinZ, fMaxZ);
	fHistProfileAllcluster = std::make_unique<TH2I>("fHistProfileAllcluster", "Clusters in profile", fNbinsX, fMinX, fMaxX, fNbinsZ, fMinZ, fMaxZ);


	////////3D results
	fCanv3DResults = std::make_unique<TCanvas>("fCanv3DResults", "Canvas with full info", 1);
	fCanv3DResults->Divide(2, 1);
	fHist3DPrecluster = std::make_unique<TH3D>("fHist3DPrecluster", "ACTAR hits",
											   fNbinsX, fMinX, fMaxX,
											   fNbinsY, fMinY, fMaxY,
											   fNbinsZ, fMinZ, fMaxZ);
	fHist3DAftercluster = std::make_unique<TH3I>("fHist3DAftercluster", "Reconstructed tracks",
												 fNbinsX, fMinX, fMaxX,
											   fNbinsY, fMinY, fMaxY,
											   fNbinsZ, fMinZ, fMaxZ);
	
	//options to gStyle
	gStyle->SetOptStat(0);
	gStyle->SetCanvasPreferGL(true);
}

void ActDraw::Reset()
{
	fCanvPrecluster->Clear("D"); //fCanvPrecluster->Update();//"D" option does not delete TPads
	fCanvAllcluster->Clear("D"); //fCanvAllcluster->Update();
	fCanv3DResults->Clear("D");
	
	fHistFront->Reset();
	fHistProfile->Reset();
	fHistPad->Reset();
	

	fHistProfileAllcluster->Reset();
	fHistFrontAllcluster->Reset();
	fHistPadAllcluster->Reset();

	fHist3DPrecluster->Reset();
	fHist3DAftercluster->Reset();
	
	fCanvPrecluster->Update();
	fCanvAllcluster->Update();
	fCanv3DResults->Update();
}

void ActDraw::DrawEvent(std::vector<ActHit> &hitArray)
{
	fCanvAllcluster->Close();
	fCanv3DResults->Close();
	Reset();

	//read each event and file Pad, Profile and Front hits
	for(const auto& hit : hitArray)
	{
		XYZPoint position { hit.GetPosition()};
		Double_t charge   { hit.GetCharge()};

		//pad
		fHistPad->Fill(position.X(), position.Y(), charge);
		//profile
		fHistProfile->Fill(position.X(), position.Z(), charge);
		//front
		fHistFront->Fill(position.Y(), position.Z(), charge);
	}

	if(fCanvPrecluster)
	{	
		fCanvPrecluster->cd(1); fHistPad->Draw("colz");
		fCanvPrecluster->cd(2); fHistProfile->Draw("colz");
		fCanvPrecluster->cd(3); fHistFront->Draw("colz");

		fCanvPrecluster->Update();
		fCanvPrecluster->cd();
		fCanvPrecluster->WaitPrimitive();
		
	}
	
}

void ActDraw::DrawResults(std::vector<ActHit> &hitArray, ActClusteringResults &results)
{
	fCanvPrecluster->Close();
	fCanv3DResults->Close();
	Reset();

	//first, events
	//read each event and file Pad, Profile and Front hits
	for(const auto& hit : hitArray)
	{
		XYZPoint position { hit.GetPosition()};
		Double_t charge   { hit.GetCharge()};

		//pad
		fHistPad->Fill(position.X(), position.Y(), charge);
		//profile
		fHistProfile->Fill(position.X(), position.Z(), charge);
		//front
		fHistFront->Fill(position.Y(), position.Z(), charge);
	}

	//now, clusters and lines
	std::vector<TString> projections { "XY", "XZ", "YZ" };
	std::map<TString, std::vector<std::unique_ptr<TPolyLine>>> polylines;

	auto tracks = results.GetTrackCandidates();
	for(const auto& track : tracks)
	{
		auto hits = track.GetHitArrayConst();
		for(const auto& hit : hits)
		{
			XYZPoint position { hit.GetPosition()};
			//pad
			OverrideHistContent(fHistPadAllcluster, tracks.size(), position.X(), position.Y(), track.GetTrackID() + 1);
			//front
			OverrideHistContent(fHistFrontAllcluster, tracks.size(), position.Y(), position.Z(), track.GetTrackID() + 1);
			//profile
			OverrideHistContent(fHistProfileAllcluster, tracks.size(), position.X(), position.Z(), track.GetTrackID() + 1);
		}
		//add TPolyLines
		for(auto& proj : projections)
		{
			auto polyline = GetPolyLine(track, proj);
			polyline->SetLineColor(track.GetTrackID() + 1);
			polyline->SetLineWidth(2);
			polylines[proj].push_back(std::move(polyline));
		}
	}

	//and finally draw
	if(fCanvAllcluster)
	{
		fCanvAllcluster->cd(1); fHistPad->Draw("colz");
		fCanvAllcluster->cd(2); fHistProfile->Draw("colz");
		fCanvAllcluster->cd(3); fHistFront->Draw("colz");

		fCanvAllcluster->cd(4); fHistPadAllcluster->Draw("colz");
		for(auto& poly : polylines["XY"]) poly->Draw("same");
		fCanvAllcluster->cd(5); fHistProfileAllcluster->Draw("colz");
		for(auto& poly : polylines["XZ"]) poly->Draw("same");
		fCanvAllcluster->cd(6); fHistFrontAllcluster->Draw("colz");
		for(auto& poly : polylines["YZ"]) poly->Draw("same");

		fCanvAllcluster->Update();
		fCanvAllcluster->cd();
		fCanvAllcluster->WaitPrimitive();
	}
}

void ActDraw::DrawResults3D(std::vector<ActHit> &hitArray, ActClusteringResults &results)
{
	fCanvPrecluster->Close();
	fCanvAllcluster->Close();
	Reset();

	//first, events
	//read each event and file Pad, Profile and Front hits
	for(const auto& hit : hitArray)
	{
		XYZPoint position { hit.GetPosition()};
		Double_t charge   { hit.GetCharge()};

		fHist3DPrecluster->Fill(position.X(), position.Y(), position.Z(), charge);
	}

	//now, clusters and lines
	std::vector<std::unique_ptr<TPolyLine3D>> polylines;
	auto tracks = results.GetTrackCandidates();
	for(const auto& track : tracks)
	{
		auto hits = track.GetHitArrayConst();
		for(const auto& hit : hits)
		{
			XYZPoint position { hit.GetPosition()};
			fHist3DAftercluster->Fill(position.X(), position.Y(), position.Z(), track.GetTrackID() + 1);
		}
		auto polyline = GetPolyLine3D(track);
		polyline->SetLineColor(track.GetTrackID() + 1);
		polyline->SetLineWidth(2);
		polylines.push_back(std::move(polyline));
	}

	//and finally draw
	if(fCanv3DResults)
	{
		fCanv3DResults->cd(1); fHist3DPrecluster->Draw("box2z");

		fCanv3DResults->cd(2); fHist3DAftercluster->Draw("box2z");
		for(auto& poly : polylines) poly->Draw("same");
		
		fCanv3DResults->Update();
		fCanv3DResults->cd();
		fCanv3DResults->WaitPrimitive();
	}
}

std::unique_ptr<TPolyLine> ActDraw::GetPolyLine(const ActTrack& track, TString projection)
{
	auto line = track.GetLine();
	auto position = line.GetPoint();
	auto direction = line.GetDirection();

	//we force slope X to 1! -> line parametric in X 
	double slope3DXY { direction.Y() / direction.X() };
	double slope3DXZ { direction.Z() / direction.X() };
	//x direction can be 0. if we have pad saturation: we must return empty TPolyLine then
	if(std::isnan(slope3DXY) || std::isnan(slope3DXZ))
	{
		std::cout<<BOLDMAGENTA<<"Warning: TPolyLine has slope in X null due to pad saturation, thus, it isn't drawn!"<<RESET<<'\n';
		std::unique_ptr<TPolyLine> emptyPolyline;
		emptyPolyline = std::make_unique<TPolyLine>();
		return emptyPolyline;
	}
	double offsetXY { position.Y() - slope3DXY * position.X() };
	double slopeXY  { slope3DXY };

	double offsetXZ { position.Z() - slope3DXZ * position.X() };
	double slopeXZ  { slope3DXZ };

	int Npoints { 500 };
	std::vector<double> vecX, vecY, vecZ;
	double x0 { fMinX};
	double dx { 1. * static_cast<double>(ActParameters::NPADX) / Npoints };
	for(int r = 0; r < Npoints; r++ )
	{
		double yval { offsetXY + slopeXY * x0 };
		double zval { offsetXZ + slopeXZ * x0 };
		//projection-dependent TPolyLine filling
		if(projection.Contains("XY"))
		{
			if(isInInterval(yval, fMinY, fMaxY))
			{
				vecX.push_back(x0);
				vecY.push_back(yval);
				
			}
		}
		else if(projection.Contains("XZ"))
		{
			if(isInInterval(zval, fMinZ, fMaxZ))
			{
				vecX.push_back(x0);
				vecZ.push_back(zval);
			}
		}
		else if(projection.Contains("YZ"))
		{
			if(isInInterval(yval, fMinY, fMaxY) &&
			   isInInterval(zval, fMinZ, fMaxZ))
			{
				vecY.push_back(yval);
				vecZ.push_back(zval);
			}
		}
		else { throw std::runtime_error("Wrong string passed to GetPolyline()"); }
		
		x0 += dx;
	}

	std::unique_ptr<TPolyLine> polyline;
	if(projection.Contains("XY")) polyline = std::make_unique<TPolyLine>(vecX.size(), &(vecX[0]), &(vecY[0]));
	else if (projection.Contains("XZ")) polyline = std::make_unique<TPolyLine>(vecX.size(), &(vecX[0]), &(vecZ[0]));
	else if (projection.Contains("YZ")) polyline = std::make_unique<TPolyLine>(vecY.size(), &(vecY[0]), &(vecZ[0]));
	else { throw std::runtime_error("Wrong string passed to GetPolyline()"); }

	return std::move(polyline);
}

std::unique_ptr<TPolyLine3D> ActDraw::GetPolyLine3D(const ActTrack& track)
{
	auto line = track.GetLine();
	auto position = line.GetPoint();
	auto direction = line.GetDirection();

	//we force slope X to 1! -> line parametric in X 
	double slope3DXY { direction.Y() / direction.X() };
	double slope3DXZ { direction.Z() / direction.X() };
	//x direction can be 0. if we have pad saturation: we must return empty TPolyLine then
	if(std::isnan(slope3DXY) || std::isnan(slope3DXZ))
	{
		std::cout<<BOLDMAGENTA<<"Warning: TPolyLine has slope in X null due to pad saturation, thus, it isn't drawn!"<<RESET<<'\n';
		std::unique_ptr<TPolyLine3D> emptyPolyline;
		emptyPolyline = std::make_unique<TPolyLine3D>();
		return emptyPolyline;
	}

	double offsetXY { position.Y() - slope3DXY * position.X() };
	double slopeXY  { slope3DXY };

	double offsetXZ { position.Z() - slope3DXZ * position.X() };
	double slopeXZ  { slope3DXZ };

	int Npoints { 50 };
	std::vector<double> vecX, vecY, vecZ;
	double x0 { 0. };
	double dx { 1. * ActParameters::NPADX / Npoints };
	for(int r = 0; r < Npoints; r++ )
	{
		double yval { offsetXY + slopeXY * x0 };
		double zval { offsetXZ + slopeXZ * x0 };
		if(isInInterval(yval, fMinY, fMaxY) && isInInterval(zval, fMinZ, fMaxZ))
		{
			vecX.push_back(x0);
			vecY.push_back(yval);
			vecZ.push_back(zval);
		}
		x0 += dx;
	}

	std::unique_ptr<TPolyLine3D> polyline;
	polyline = std::make_unique<TPolyLine3D>(vecX.size(), &(vecX[0]),
										  &(vecY[0]), &(vecZ[0]));
	return std::move(polyline);
}
