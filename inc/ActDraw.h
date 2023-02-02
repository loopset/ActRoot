#ifndef ACTDRAW_H
#define ACTDRAW_H

#include "ActRoot.h"
#include "ActCalibrations.h"
#include "ActHit.h"
#include "ActClusteringResults.h"
#include "ActTrack.h"
#include "ActStructs.h"

#include <Math/Point3Dfwd.h>
#include <Math/Point3D.h>
#include <Math/Vector3Dfwd.h>
#include <Math/Vector3D.h>
#include <RtypesCore.h>
#include <TCanvas.h>
#include <TH1.h>
#include <TH1I.h>
#include <TH1D.h>
#include <TH2.h>
#include <TH2I.h>
#include <TH2D.h>
#include <TH3D.h>
#include <TH3I.h>
#include <TPolyLine.h>
#include <TPolyLine3D.h>
#include <TString.h>

#include <algorithm>
#include <vector>
#include <memory>

class ActDraw
{
	public:
	using XYZPoint = ROOT::Math::XYZPoint;
	using XYZVector = ROOT::Math::XYZVector;

	protected:
	//only hits
	std::unique_ptr<TCanvas> fCanvPrecluster;
	std::unique_ptr<TH2D>    fHistPad;
	std::unique_ptr<TH2D>    fHistFront;
	std::unique_ptr<TH2D>    fHistProfile;

	//canvas for poster 
	std::unique_ptr<TCanvas> fCanvPoster;

	//both
	std::unique_ptr<TCanvas> fCanvAllcluster;
	std::unique_ptr<TH2I>    fHistPadAllcluster;
	std::unique_ptr<TH2I>    fHistFrontAllcluster;
	std::unique_ptr<TH2I>    fHistProfileAllcluster;

	//3D drawings
	std::unique_ptr<TCanvas> fCanv3DResults;
	std::unique_ptr<TH3D>    fHist3DPrecluster;
	std::unique_ptr<TH3I>    fHist3DAftercluster;

	//physical tracks for visual analysis
	std::unique_ptr<TCanvas> fCanvVisual;
	std::unique_ptr<TH2D> fHistVisualPad;
	std::unique_ptr<TH2D> fHistVisualFront;
	std::unique_ptr<TH2D> fHistVisualProfile;

	//binning and size of histograms
	Int_t fNbinsX { ActRoot::GetChamber().fNPADSX};
	Int_t fNbinsY { ActRoot::GetChamber().fNPADSY};
	Int_t fNbinsZ { ActRoot::GetChamber().fNBINSZ};//new! to admit rebinning in Z!

	Double_t fMinX { 0.};
	Double_t fMaxX { static_cast<Double_t>(ActRoot::GetChamber().fNPADSX)};
	Double_t fMinY { 0};
	Double_t fMaxY { static_cast<Double_t>(ActRoot::GetChamber().fNPADSY)};
	Double_t fMinZ { 0.};
	Double_t fMaxZ { static_cast<Double_t>(ActRoot::GetChamber().fNPADSZ)};

	//Plotting options
	//Double_t fOverrideHits { 15.};//value for a hit assigned to multiple tracks

public:
	ActDraw();
	~ActDraw();

	//getters
	std::unique_ptr<TCanvas> GetCanvPrecluster() { return std::move(fCanvPrecluster); }
	std::unique_ptr<TCanvas> GetCanvResults() { return std::move(fCanvAllcluster); }
	//initialize canvas
	void Init();
	//reset HISTOGRAMS only
	void Reset();
	
	//and draw event
	void DrawEvent(std::vector<ActHit>& hitArray);
	//draw both
	void DrawResults(std::vector<ActHit>& hitArray, ActClusteringResults& results);
	//draw results for publications (just less TPads)
	void DrawResultsPublication(const std::vector<ActHit>& hitArray, ActClusteringResults& results);
	//draw results in 3D!!
	void DrawResults3D(std::vector<ActHit>& hitArray, ActClusteringResults& results);
	
	//set bin and max values on histograms
	void SetMaxZ(double max) { fMaxZ = max; }
	void SetNbinsZ(int nbins) { fNbinsZ = nbins; }
	void SetMaxY(double max){ fMaxY = max; }
	void SetNbinsY(int nbins){ fNbinsY = nbins; }
	void SetMaxX(double max){ fMaxX = max; }
	void SetNbinsX(int nbins){ fNbinsX = nbins; }
	
protected:
	inline void OverrideHistContent(std::unique_ptr<TH2I>& hist,
                                    const Double_t& maxVal,
                                    const Double_t& val1,
                                    const Double_t& val2, const Double_t& val3)
	{
		if(hist == nullptr) return ;
		Int_t Bin1 { hist->GetXaxis()->FindFixBin(val1) };
		Int_t Bin2 { hist->GetYaxis()->FindFixBin(val2) };
		Double_t content { hist->GetBinContent(Bin1, Bin2) };
		//simply override; do not consider extra max value
		hist->SetBinContent(Bin1, Bin2, val3);
		//if(content > 0) hist->SetBinContent(Bin1, Bin2, maxVal + 1);
		//else hist->SetBinContent(Bin1, Bin2, val3);
	}

	inline bool isInInterval(double val, double min, double max)
	{
		return (val >= min) && (val <= max);
	}
	std::unique_ptr<TPolyLine> GetPolyLine(const ActTrack& track, TString projection);
	std::unique_ptr<TPolyLine3D>  GetPolyLine3D(const ActTrack& track);
	
};

#endif //ACTDRAW_H
