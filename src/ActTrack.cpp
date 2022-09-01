#include "ActTrack.h"
#include "ActHit.h"
#include "ActLine.h"

#include <Rtypes.h>
#include <algorithm>
#include <cstddef>
#include <memory>
#include <type_traits>
#include <utility>

ClassImp(ActTrack);

void swap(ActTrack& a, ActTrack& b) noexcept
{
	using std::swap;

	//enable ADL: use by default std::swap
	swap(a.fTrackID, b.fTrackID);
    swap(a.fHitArray, b.fHitArray);
}

void ActTrack::AddHit(const ActHit &hit)
{
	fHitArray.push_back(hit);
}

void ActTrack::AddHit(ActHit&& hit)
{
	fHitArray.push_back(hit);
}

void ActTrack::SetLine(const ActLine& line)
{
	fLine = line;
}
