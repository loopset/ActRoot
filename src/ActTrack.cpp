#include "ActTrack.h"

#include "ActCalibrations.h"
#include "ActParameters.h"
#include "ActHit.h"
#include "ActLine.h"
#include "ActStructs.h"


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

//1st, in pad and time bucket units!
void ActTrack::SetReactionAndSiliconPointsRawUnits()
{
	CalculateReactionPointRawUnits();
	if(!fRPInChamber)
		return;//do not continue if RP is not inside chamber
	CalculateSiliconPointRawUnits();
}

void ActTrack::CalculateReactionPointRawUnits()
{
	XYZPoint pointPlane { 0., ActParameters::beamPlaneY, 0.};
	XYZVector normalUnitaryPlane { 0., 1., 0.};

	auto intersection { IntersectionTrackPlane(pointPlane, normalUnitaryPlane, *this)};
	//and now check if RP is inside chamber boundaries (if not, it is highly likely it is a reaction in beam window/dump)
	//also, if Z is outside 0 < z < NPADZ, it is probably a delta electron
	if(IsInChamber(intersection))
	{
		fTrackPhysics.fReactionPlace = ActParameters::trackChamber;
		fTrackPhysics.fReactionPoint = intersection;
		fTrackPhysics.fRPInChamber = true;
		fRPInChamber = true;
		return;
	}
	
	if((intersection.Z() < 0) || (intersection.Z() > ActParameters::g_NPADZ))
	{
		fTrackPhysics.fReactionPlace = ActParameters::trackDelta;
		return;
	}
	//even if Z is inside chamber and X not, nothing guarantees that it is not a delta
	if(intersection.X() < 0.)
	{
		fTrackPhysics.fReactionPlace = ActParameters::trackWindow;
		return;
	}
	else if(intersection.X() > ActParameters::g_NPADX)
	{
		fTrackPhysics.fReactionPlace = ActParameters::trackDump;
		return;
	}
}

void ActTrack::CalculateSiliconPointRawUnits()
{
	//computute intersection point with silicons
	std::map<std::string, XYZPoint> siliconsPlacement = { {"S_left", XYZPoint(0., ActParameters::g_NPADY + ActParameters::g_NPADSISIDE, 0.)},
														  {"S_right", XYZPoint(0., -ActParameters::g_NPADSISIDE, 0.)},
														  {"F", XYZPoint(ActParameters::g_NPADX + ActParameters::g_NPADSSIFRONT, 0., 0.)}};

	//get correct direction taking into account reaction point and gravity center
	auto direction { fLine.GetPoint() - //fLine.GetPoint is intended to represent gravitypoint
					 fTrackPhysics.fReactionPoint};
	//compute intersection with Silicon walls according to direction of track
	std::string label {""};
	//SIDES
	XYZPoint intersectionY {};
	if(direction.Y() >= 0)//left silicons
	{
		label = ActParameters::trackHitsSiliconSideLeft;
		intersectionY = IntersectionTrackPlane(siliconsPlacement["S_left"], XYZVector(0., 1., 0.), *this);
		
	}
	else
	{
		label = ActParameters::trackHitsSiliconSideRight;
		intersectionY = IntersectionTrackPlane(siliconsPlacement["S_right"], XYZVector(0., 1., 0.), *this);
	}
	if(IsInSiliconPlane(intersectionY, "S"))
	{
		fTrackPhysics.fSiliconPlace = label;
		fTrackPhysics.fSiliconPoint = intersectionY;
		fSPInArray = true;
		fTrackPhysics.fSPInArray = true;
	}
	else
	{
		//check FRONT
		XYZPoint intersectionX {};
		if(direction.X() >= 0)
		{
			label = ActParameters::trackHitsSiliconFront;
			intersectionX = IntersectionTrackPlane(siliconsPlacement["F"], XYZVector(1., 0., 0.), *this);
		}
		else
		{
			intersectionX = XYZPoint(-999, -999, -999);
		}
		if(IsInSiliconPlane(intersectionX, "F"))
		{
			fTrackPhysics.fSiliconPlace = label;
			fTrackPhysics.fSiliconPoint = intersectionX;
			fSPInArray = true;
			fTrackPhysics.fSPInArray = true;
			
		}
		else
		{
			fTrackPhysics.fSiliconPlace = ActParameters::trackHitsSiliconOutside;
		}
	}
}

//2nd, set physical values
//IMPORTANT: depends on ActCalibrations bc we need to convert to
//physical lengths using coefficients set there!
void ActTrack::SetTrackPhysics(ActCalibrations& calibrations)
{
	//copy trackID to our struct
	fTrackPhysics.fTrackID = fTrackID;
	CalculateTrackTotalCharge();
	if(!fRPInChamber)
		return;//since RP is not located in chamber, we cannot compute anything
	//and now a funtion to each physical parameter
	CalculateReactionPoint(calibrations);
	if(!fSPInArray)
		return;//same but with SP
	CalculateSiliconPoint(calibrations);
	CalculateTrackLength();
	CalculateTrackAverageCharge();
	CalculateThetaTrack();
	CalculatePhiTrack();
}

void ActTrack::CalculateThetaTrack()
{
	XYZVector vector { fTrackPhysics.fSiliconPoint - fTrackPhysics.fReactionPoint};
	XYZVector n_z { 0., 0., 1.};//normal to z in order to get theta
	auto dot { n_z.Dot(vector.Unit())};//all unitary vectors
	auto theta { TMath::ACos(dot)};
	fTrackPhysics.fTheta = TMath::RadToDeg() * theta;
}

void ActTrack::CalculatePhiTrack()
{
	XYZVector vector { fTrackPhysics.fSiliconPoint - fTrackPhysics.fReactionPoint};
	XYZVector n_y { 0., 1., 0.};
	auto dot { n_y.Dot(vector.Unit())};
	auto phi { TMath::ACos(dot)};
	fTrackPhysics.fPhi = TMath::RadToDeg() * phi;
}

void ActTrack::CalculateReactionPoint(ActCalibrations& calibrations)
{
	//once reaction point is computed in raw units, we have to convert it to physical units
	//here, we convert it to mm
	auto oldPoint { fTrackPhysics.fReactionPoint};
	XYZPoint newPoint;
	newPoint.SetX(oldPoint.X() * calibrations.GetXYToLengthUnitsCoef());
	newPoint.SetY(oldPoint.Y() * calibrations.GetXYToLengthUnitsCoef());
	newPoint.SetZ(oldPoint.Z() * calibrations.GetZToLengthUnitsCoef());

	fTrackPhysics.fReactionPoint = newPoint;
}

void ActTrack::CalculateSiliconPoint(ActCalibrations& calibrations)
{
	auto oldPoint { fTrackPhysics.fSiliconPoint};
	XYZPoint newPoint;
	newPoint.SetX(oldPoint.X() * calibrations.GetXYToLengthUnitsCoef());
	newPoint.SetY(oldPoint.Y() * calibrations.GetXYToLengthUnitsCoef());
	newPoint.SetZ(oldPoint.Z() * calibrations.GetZToLengthUnitsCoef());

	fTrackPhysics.fSiliconPoint = newPoint;
}

void ActTrack::CalculateTrackLength()
{
	//MUST BE RUN AFTER SILICON AND REACTION POINT CALCULATIONS
	auto rp { fTrackPhysics.fReactionPoint};
	auto sp { fTrackPhysics.fSiliconPoint};

	double dist2 { (rp - sp).Mag2()};
	fTrackPhysics.fTrackLength = TMath::Sqrt(dist2);
}

void ActTrack::CalculateTrackTotalCharge()
{
	auto totalCharge = std::accumulate(fHitArray.begin(),
									   fHitArray.end(),
									   0.,
									   [](double sum, const ActHit& hit){return hit.GetCharge() + sum;});

	fTrackPhysics.fTotalCharge = totalCharge;
}

void ActTrack::CalculateTrackAverageCharge()
{
	fTrackPhysics.fAverageCharge = fTrackPhysics.fTotalCharge / fTrackPhysics.fTrackLength;
}
