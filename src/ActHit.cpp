#include "ActHit.h"

#include <Rtypes.h>
#include <RtypesCore.h>
#include <cmath>
#include <utility>

ClassImp(ActHit);

//constructor initialization
//ActHit::ActHit(Int_t hitID) : ActHit(hitID, XYZPoint(0, 0, -1000), -1) {}

ActHit::ActHit(Int_t hitID, XYZPoint location, Double_t charge, bool hasSaturated)
	: fHitID(hitID), fPosition(location), fCharge(charge), fIsSaturated(hasSaturated)
{
}

std::unique_ptr<ActHit> ActHit::Clone()
{
	return std::make_unique<ActHit>(*this);
}
