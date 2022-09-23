#include "ActAnalyze.h"

#include <TTree.h>

#include <iostream>

ActAnalyze::ActAnalyze(TTree* tree)
	: fTree(tree)
{
}

void ActAnalyze::ReadTree()
{
	fTree->SetBranchAddress("tracks", &fTracks);
	fTree->SetBranchAddress("silicons", &fSilicons);
	//number of entries
	long long nEntries { fTree->GetEntries()};
	for(long long i = 0; i < nEntries; i++)
	{
		fTree->GetEntry(i);
		//and here run functions
		ProcessTrackPhysics();
	}
}

void ActAnalyze::ProcessTrackPhysics()
{
	if(fTracks->size() > 0)
		std::cout<<"Track ID at 0: "<<fTracks->at(0).fTrackID<<'\n';
}
