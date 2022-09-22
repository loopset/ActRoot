#include "ActTrack.h"

#include "ActParameters.h"
#include "ActHit.h"
#include "ActLine.h"
#include "Silicons.h"
#include "TrackPhysics.h"


#include <Rtypes.h>
#include <Math/Point3D.h>
#include <Math/Vector3D.h>
#include <algorithm>
#include <cstddef>
#include <memory>
#include <numeric>
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

///// PHYSICAL INFORMATION OF TRACK ///////

void ActTrack::SetTrackPhysics()
{
	//copy trackID to our struct
	fTrackPhysics.fTrackID = this->GetTrackID();
	//and now a funtion to each physical parameter
	CalculateThetaTrack(*this);
	CalculatePhiTrack(*this);
	CalculateReactionPoint(*this);
	CalculateBoundaryPoint(*this);
	if(!fIsGood)
		return;
	// do not compute other parameters if reaction point is outside chamber
	CalculateSiliconPoint(*this);
	CalculateTrackLength(*this);
	CalculateTrackCharge(*this);
}

void ActTrack::CalculateThetaTrack(ActTrack& track)
{
	auto line { track.GetLine()};
	XYZVector n_z { 0., 0., 1.};
	auto dot { n_z.Dot(line.GetDirection().Unit())};//all unitary vectors
	auto theta { TMath::ACos(dot)};
	track.GetTrackPhysics().fTheta = theta;
}

void ActTrack::CalculatePhiTrack(ActTrack& track)
{
	auto line { track.GetLine()};
	XYZVector n_y { 0., 1., 0.};
	auto dot { n_y.Dot(line.GetDirection().Unit())};
	auto phi { TMath::ACos(dot)};
	track.GetTrackPhysics().fPhi = phi;
}

void ActTrack::CalculateReactionPoint(ActTrack& track)
{
	
	XYZPoint pointPlane { 0., ActParameters::beamPlaneY, 0.};
	XYZVector normalUnitaryPlane { 0., 1., 0.};

	auto intersection { IntersectionTrackPlane(pointPlane, normalUnitaryPlane, track)};
	//and now check if RP is inside chamber boundaries (if not, it is highly likely it is a reaction in beam window/dump)
	//also, if Z is outside 0 < z < NPADZ, it is probably a delta electron
	if(IsInChamber(intersection))
	{
		track.GetTrackPhysics().fReactionPlace = ActParameters::trackChamber;
		track.GetTrackPhysics().fReactionPoint = intersection;
		return;
	}
	
	if((intersection.Z() < 0) || (intersection.Z() > ActParameters::g_NPADZ))
	{
		track.GetTrackPhysics().fReactionPlace = ActParameters::trackDelta;
		fIsGood = false;
		return;
	}
	//even if Z is inside chamber and X not, nothing guarantees that it is not a delta
	if(intersection.X() < 0.)
	{
		track.GetTrackPhysics().fReactionPlace = ActParameters::trackWindow;
		fIsGood = false;
		return;
	}
	else if(intersection.X() > ActParameters::g_NPADX)
	{
		track.GetTrackPhysics().fReactionPlace = ActParameters::trackDump;
		fIsGood = false;
		return;
	}
}

void ActTrack::CalculateBoundaryPoint(ActTrack& track)
{
	if(!fIsGood)
		return;
	//get sign of direction in y of track
	auto slopeY { track.GetLine().GetDirection().Y()};
	XYZPoint planeY { 0., (slopeY >= 0. ) ? ActParameters::g_NPADY : 0., 0.};
	XYZVector vectorY { 0., 1., 0.};
	auto intersectionY { IntersectionTrackPlane(planeY, vectorY, track)};

	XYZPoint planeX { ActParameters::g_NPADX, 0., 0.};
	XYZVector vectorX { 1., 0., 0.};
	auto intersectionX { IntersectionTrackPlane(planeX, vectorX, track)};
	// std::cout<<"Intersection Y: "<<'\n';
	// std::cout<<'\t'<<"X: "<<intersectionY.X()<<" Y: "<<intersectionY.Y()<< " Z: "<<intersectionY.Z()<<'\n';
	// std::cout<<"Intersection X: "<<'\n';
	// std::cout<<'\t'<<"X: "<<intersectionX.X()<<" Y: "<<intersectionX.Y()<< " Z: "<<intersectionX.Z()<<'\n';
	if(IsInChamber(intersectionY))
	{
		track.GetTrackPhysics().fSiliconPlace = ActParameters::trackHitsSiliconSide;
	}
	else if (IsInChamber(intersectionX))
	{
		track.GetTrackPhysics().fSiliconPlace = ActParameters::trackHitsSiliconFront;
	}
	else
	{
		//again, if intersection point in boundary is not in chamber
		//we are not interested in this track
		track.GetTrackPhysics().fSiliconPlace = ActParameters::trackHitsSiliconOutside;
		fIsGood = false;
	}
}

void ActTrack::CalculateSiliconPoint(ActTrack& track)
{
	XYZPoint pointPlane;
	XYZVector vectorPlane;
	if(track.GetTrackPhysics().fSiliconPlace == ActParameters::trackHitsSiliconSide)
	{
		pointPlane = { 0., ActParameters::g_NPADY + ActParameters::g_NPADSISIDE, 0.};
		vectorPlane = { 0., 1., 0.};
	}
	else if(track.GetTrackPhysics().fSiliconPlace == ActParameters::trackHitsSiliconFront)
	{
		pointPlane = { ActParameters::g_NPADX + ActParameters::g_NPADSSIFRONT, 0., 0.};
		vectorPlane = { 1., 0., 0.};
	}
	else
	{
			std::cout<<BOLDRED<<"ActTrack::CalculateSiliconPoint has received a wrong boundary intersection point -> Not computing silicon point!"<<RESET<<'\n';
			return;
	}

	auto intersection { IntersectionTrackPlane(pointPlane, vectorPlane, track)};
	track.GetTrackPhysics().fSiliconPoint = intersection;
}

void ActTrack::CalculateTrackLength(ActTrack& track)
{
	//MUST BE RUN AFTER SILICON AND REACTION POINT CALCULATIONS
	auto rp { track.GetTrackPhysics().fReactionPoint};
	auto sp { track.GetTrackPhysics().fSiliconPoint};

	double dist2 { (rp - sp).Mag2()};
	track.GetTrackPhysics().fTrackLength = TMath::Sqrt(dist2);
}

void ActTrack::CalculateTrackCharge(ActTrack& track)
{
	auto totalCharge = std::accumulate(track.GetHitArray().begin(),
									   track.GetHitArray().end(),
									   0.,
									   [](double sum, const ActHit& hit){return hit.GetCharge() + sum;});

	auto averageCharge { totalCharge / track.GetTrackPhysics().fTrackLength};

	track.GetTrackPhysics().fTotalCharge = totalCharge;
	track.GetTrackPhysics().fAverageCharge = averageCharge;
}
