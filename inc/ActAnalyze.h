#ifndef ACTANALYZE_H
#define ACTANALYZE_H

#include "ActStructs.h"

#include <TTree.h>
#include <TCanvas.h>

#include <vector>

class ActAnalyze
{
private:
	TTree* fTree { nullptr};//do not modify TTree inside class (pointer could be changed)
	Silicons* fSilicons {nullptr};
	TriggersAndGates* fTriggers {nullptr};
	std::vector<TrackPhysics>* fTracks {nullptr};

public:
	ActAnalyze(TTree* tree);
	~ActAnalyze() = default;

	//read TTree and set branches to our private members
	void ReadTree();

private:
	void ProcessTrackPhysics();
};

#endif //ACTANALIZE_H
