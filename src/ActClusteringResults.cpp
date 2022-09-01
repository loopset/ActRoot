#include "ActClusteringResults.h"
#include "ActTrack.h"
#include "ActHit.h"

#include <Rtypes.h>
#include <TNamed.h>

ClassImp(ActClusteringResults)

ActClusteringResults::ActClusteringResults() : TNamed("ActClusteringResults", "Results of clustering")
{
	
}

void ActClusteringResults::SetTrackCandidates(std::vector<ActTrack> tracks)
{
	fTrackCandidates = tracks;
}

void ActClusteringResults::AddTrack(const ActTrack& track)
{
	fTrackCandidates.push_back(track);
}

void ActClusteringResults::AddTrack(ActTrack&& track)
{
	fTrackCandidates.push_back(track);
}

void ActClusteringResults::AddNoise(ActHit hit)
{
	fNoise.push_back(std::move(hit));
}
//void ActClusteringResults::AddHit(ActHit hit) { fHits.push_back(hit); }
