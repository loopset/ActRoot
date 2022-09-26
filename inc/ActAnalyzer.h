#ifndef ACTANALYZER_H
#define ACTANALYZER_H

#include "ActStructs.h"

#include <TH2.h>
#include <TTree.h>
#include <TCanvas.h>

#include <memory>
#include <vector>

class ActAnalyzer
{
private:
	//to read ttree
	TTree* fTree { nullptr};
	Silicons* fSilicons {nullptr};
	TriggersAndGates* fTriggers {nullptr};
	std::vector<TrackPhysics>* fTracks {nullptr};

	//canvas and histograms
	std::unique_ptr<TCanvas> fCanvTrackID {nullptr};
	std::unique_ptr<TH2D> fHistTrackID {nullptr};
public:
	ActAnalyzer(TTree* tree, std::unique_ptr<TH2D> histTrackID);
	~ActAnalyzer() = default;

	//read TTree and set branches to our private members
	void ReadTree();
	//draw canvas at the end of execution
	void DrawCanvas();

private:
	void ProcessTrackID();
};

#endif //ACTANALIZER_H
