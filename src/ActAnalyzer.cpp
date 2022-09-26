#include "ActAnalyzer.h"

#include "ActParameters.h"

#include <TCanvas.h>
#include <TH2.h>
#include <TTree.h>

#include <algorithm>
#include <iostream>
#include <memory>

ActAnalyzer::ActAnalyzer(TTree* tree, std::unique_ptr<TH2D> histTrackID)
	: fTree(tree),
	  //fCanvTrackID(std::make_unique<TCanvas>("fCanvTrackID", "Track ID ", 1)),
	  fHistTrackID(std::move(histTrackID))
{
}

void ActAnalyzer::DrawCanvas()
{
	fCanvTrackID = std::make_unique<TCanvas>("fCanvTrackID", "Track ID ", 1);
	fCanvTrackID->cd();
	fHistTrackID->Draw("colz");
	fCanvTrackID->Update();
	fCanvTrackID->WaitPrimitive();
}

void ActAnalyzer::ReadTree()
{
	fTree->SetBranchAddress("tracks", &fTracks);
	fTree->SetBranchAddress("silicons", &fSilicons);
	fTree->SetBranchAddress("triggers", &fTriggers);
	//number of entries
	long long nEntries { fTree->GetEntries()};
	for(long long i = 0; i < nEntries; i++)
	{
		fTree->GetEntry(i);
		//and here run functions
		ProcessTrackID();
	}
	
}

void ActAnalyzer::ProcessTrackID()
{
	//check if we have tracks
	if(fTracks->size() == 0)
		return;

	for(auto& track : *fTracks)
	{
		if(!(track.fIsGood))//if track is not good, continue!
			continue;
		//get silicon place
		auto siliconSide { track.fSiliconPlace};
		double energySi {};
		if(siliconSide == ActParameters::trackHitsSiliconSideLeft)//side_left
		{
		    energySi = fSilicons->fData["S"]["ES"];
		}
		else if(siliconSide == ActParameters::trackHitsSiliconFront)//front
		{
			auto energySi0 { fSilicons->fData["01F"]["E0"]};
			auto energySi1 { fSilicons->fData["01F"]["E1"]};
			energySi = (energySi1 > 0.) ? energySi1 : energySi0;
		}
		else//track doesnt hit silicons
		{
			continue;
		}
		auto chargeAveraged { track.fAverageCharge};
		//however, check if energy at Si is greater than 0.!
		if(energySi <= 0.)
			continue;

		fHistTrackID->Fill(energySi, chargeAveraged);
	}	
}
