#ifndef ACTCLUSTERINGRESULTS_H
#define ACTCLUSTERINGRESULTS_H

//This is just a collection of ActTracks, result of the different algorithms clustering processes.

//class ActHit;
//class ActTrack;
#include "ActHit.h"
#include "ActTrack.h"

//Include TNamed as ATTPCROOT
#include <Rtypes.h>
#include <TNamed.h>
#include <algorithm>
#include <utility>
#include <vector>

class ActClusteringResults : public TNamed
{
	private:
	std::vector<ActTrack> fTrackCandidates;
	std::vector<ActHit> fNoise;

	public:
	ActClusteringResults();
	~ActClusteringResults() = default;

	//setters should be included in .cpp bc otherwise we have problems with partial definitions when modifying the std::vector(idk why since we include the header file...)
	void SetTrackCandidates(std::vector<ActTrack> tracks);
	void AddTrack(const ActTrack& track);
	void AddTrack(ActTrack&& track);
	void AddNoise(ActHit hit);

	//return by ref if we want to modify clusters after
	const std::vector<ActHit>& GetNoise() { return fNoise; }
	std::vector<ActTrack>& GetTrackCandidates() { return fTrackCandidates; }

	ClassDef(ActClusteringResults, 1)
};

#endif //ACTCLUSTERINGRESULTS_H
