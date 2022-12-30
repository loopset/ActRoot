#include "ActTrack.h"

#include "ActCalibrations.h"
#include "ActParameters.h"
#include "ActHit.h"
#include "ActLine.h"
#include "ActStructs.h"
#include "TMathBase.h"
#include "TUrl.h"


#include <Rtypes.h>
#include <Math/Point3D.h>
#include <Math/Vector3D.h>
#include <TMath.h>
#include <algorithm>
#include <cmath>
#include <cstddef>
#include <iterator>
#include <map>
#include <memory>
#include <numeric>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

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
