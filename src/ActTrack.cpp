#include "ActTrack.h"

#include "ActParameters.h"
#include "ActHit.h"
#include "ActLine.h"
#include "ActStructs.h"


#include <Rtypes.h>
#include <Math/Point3D.h>
#include <Math/Vector3D.h>
#include <algorithm>
#include <cmath>
#include <cstddef>
#include <iterator>
#include <map>
#include <memory>
#include <numeric>
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

///// PHYSICAL INFORMATION OF TRACK ///////

void ActTrack::SetTrackPhysics()
{
	//copy trackID to our struct
	fTrackPhysics.fTrackID = this->GetTrackID();
	//and now a funtion to each physical parameter
	CalculateThetaTrack(*this);
	CalculatePhiTrack(*this);
	CalculateReactionPoint(*this);
	//CalculateBoundaryPoint(*this);
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
		track.GetTrackPhysics().fIsGood = false;
		fIsGood = false;
		return;
	}
	//even if Z is inside chamber and X not, nothing guarantees that it is not a delta
	if(intersection.X() < 0.)
	{
		track.GetTrackPhysics().fReactionPlace = ActParameters::trackWindow;
		track.GetTrackPhysics().fIsGood = false;
		fIsGood = false;
		return;
	}
	else if(intersection.X() > ActParameters::g_NPADX)
	{
		track.GetTrackPhysics().fReactionPlace = ActParameters::trackDump;
		track.GetTrackPhysics().fIsGood = false;
		fIsGood = false;
		return;
	}
}

void ActTrack::CalculateSiliconPoint(ActTrack& track)
{
	//computute intersection point with silicons
	std::map<std::string, XYZPoint> siliconsPlacement = { {"S_left", XYZPoint(0., ActParameters::g_NPADY + ActParameters::g_NPADSISIDE, 0.)},
														  {"S_right", XYZPoint(0., -ActParameters::g_NPADSISIDE, 0.)},
														  {"F", XYZPoint(ActParameters::g_NPADX + ActParameters::g_NPADSSIFRONT, 0., 0.)}};

	//get correct direction taking into account reaction point and gravity center
	auto direction { track.GetLine().GetPoint() -
					 track.GetTrackPhysics().fReactionPoint};
	//compute intersection with Silicon walls according to direction of track
	std::string label {""};
	//SIDES
	XYZPoint intersectionY {};
	if(direction.Y() >= 0)//left silicons
	{
		label = ActParameters::trackHitsSiliconSideLeft;
		intersectionY = IntersectionTrackPlane(siliconsPlacement["S_left"], XYZVector(0., 1., 0.), track);
		
	}
	else
	{
		label = ActParameters::trackHitsSiliconSideRight;
		intersectionY = IntersectionTrackPlane(siliconsPlacement["S_right"], XYZVector(0., 1., 0.), track);
	}
	if(IsInSiliconPlane(intersectionY, "S"))
	{
		track.GetTrackPhysics().fSiliconPlace = label;
		track.GetTrackPhysics().fSiliconPoint = intersectionY;
	}
	else
	{
		//check FRONT
		XYZPoint intersectionX {};
		if(direction.X() >= 0)
		{
			label = ActParameters::trackHitsSiliconFront;
			intersectionX = IntersectionTrackPlane(siliconsPlacement["F"], XYZVector(1., 0., 0.), track);
		}
		else
		{
			intersectionX = XYZPoint(-999, -999, -999);
		}
		if(IsInSiliconPlane(intersectionX, "F"))
		{
			track.GetTrackPhysics().fSiliconPlace = label;
			track.GetTrackPhysics().fSiliconPoint = intersectionX;
		}
		else
		{
			track.GetTrackPhysics().fSiliconPlace = ActParameters::trackHitsSiliconOutside;
			fIsGood = false;
			track.GetTrackPhysics().fIsGood = false;
		}
	}

	//std::cout<<"Silicon hit: "<<track.GetTrackPhysics().fSiliconPlace<<'\n';
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
