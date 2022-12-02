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

///// PHYSICAL INFORMATION OF TRACK ///////

//1st, in pad and time bucket units!
void ActTrack::SetMinimalTrackPhysics()
{
	fTrackPhysics.fTrackID = fTrackID;
	CalculateTrackTotalCharge();
	
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
		intersectionY = IntersectionTrackPlane(ActParameters::siliconsPlacement.at("left"), XYZVector(0., 1., 0.), *this);
		
	}
	else
	{
		label = ActParameters::trackHitsSiliconSideRight;
		intersectionY = IntersectionTrackPlane(ActParameters::siliconsPlacement.at("right"), XYZVector(0., 1., 0.), *this);
	}
	if(IsInSiliconPlane(intersectionY, "S"))
	{
		fTrackPhysics.fSiliconPlace = label;
		fTrackPhysics.fSiliconPoint = intersectionY;
		fSPInArray = true;
		fTrackPhysics.fSPInArray = true;
	}
	else//no front silicons in NFS
	{
		// //check FRONT
		// XYZPoint intersectionX {};
		// if(direction.X() >= 0)
		// {
		// 	label = ActParameters::trackHitsSiliconFront;
		// 	intersectionX = IntersectionTrackPlane(siliconsPlacement["F"], XYZVector(1., 0., 0.), *this);
		// }
		// else
		// {
		// 	intersectionX = XYZPoint(-999, -999, -999);
		// }
		// if(IsInSiliconPlane(intersectionX, "F"))
		// {
		// 	fTrackPhysics.fSiliconPlace = label;
		// 	fTrackPhysics.fSiliconPoint = intersectionX;
		// 	fSPInArray = true;
		// 	fTrackPhysics.fSPInArray = true;
			
		// }
		// else
		// {
        fTrackPhysics.fSiliconPlace = ActParameters::trackHitsSiliconOutside;
        // }
	}
}

void ActTrack::CalculateSiliconPointRawUnits(const std::string& side, int silIndex)
{
     auto point {fLine.GetPoint()};
    XYZPoint goodSigns {
            ActParameters::siliconDirection.at(side).at(silIndex).first - point.X(),
            ActParameters::siliconsPlacement.at(side).Y()               - point.Y(),
            ActParameters::siliconDirection.at(side).at(silIndex).second- point.Z()
        };

    auto oldDirection {fLine.GetDirection()};
    XYZVector newDirection {
        TMath::Sign(oldDirection.X(), goodSigns.X()),
        TMath::Sign(oldDirection.Y(), goodSigns.Y()),
        TMath::Sign(oldDirection.Z(), goodSigns.Z()),
    };

    //write correct value to struct, since we dont have the VERTEX
    fTrackPhysics.fGravityPoint     = fLine.GetPoint();
    fTrackPhysics.fUnitaryDirection = newDirection.Unit();
    //finally compute intersection with this new direction
	//SIDES
	XYZPoint intersection {};
    intersection = IntersectionTrackPlane(ActParameters::siliconsPlacement.at(side), XYZVector(0., 1., 0.), fTrackPhysics.fGravityPoint, fTrackPhysics.fUnitaryDirection);
   
    fTrackPhysics.fSiliconPlace = side;
    fTrackPhysics.fSiliconPoint = intersection;
    fTrackPhysics.fSiliconIndex = silIndex; 
}

void ActTrack::CalculateBoundaryPointRawUnits()
{
    auto lambda = [&](const XYZPoint& Pp, const XYZVector& vp)
    {
        auto Pt { fTrackPhysics.fGravityPoint};
        auto vt { fTrackPhysics.fUnitaryDirection};
        auto interesection { Pt + (((Pp - Pt).Dot(vp)) / (vt.Dot(vp))) * vt};
		return interesection;
    };

    if(fTrackPhysics.fSiliconPlace == ActParameters::trackHitsSiliconSideLeft)
	{
		XYZPoint planePoint { 0., ActParameters::g_NPADY, 0.};
		XYZVector normalVector {0., 1., 0.};
		fTrackPhysics.fBoundaryPoint = lambda(planePoint, normalVector);
	}
	else
	{
		XYZPoint planePoint { 0., 0., 0.};
		XYZVector normalVector {0., -1., 0.};
		fTrackPhysics.fBoundaryPoint = lambda(planePoint, normalVector);
	}
    if(IsInChamber(fTrackPhysics.fBoundaryPoint))
    {
        fTrackPhysics.fBPInChamber = true;
    }

}

void ActTrack::ComputeChargeAndLengthInRegion(double yWidth,
                                              const std::vector<std::vector<double>>& pad,
                                              double& length,
                                              double& charge)
{
    auto lambda = [&](const XYZPoint& Pp, const XYZVector& vp)
    {
        auto Pt { fTrackPhysics.fGravityPoint};
        auto vt { fTrackPhysics.fUnitaryDirection};
        auto interesection { Pt + (((Pp - Pt).Dot(vp)) / (vt.Dot(vp))) * vt};
		return interesection;
    };
    
    auto side { fTrackPhysics.fSiliconPlace};
    int yBP {};
    int yThreshold {};
    if(side == ActParameters::trackHitsSiliconSideLeft)
    {
        yBP        = ActParameters::g_NPADY;
        yThreshold = yBP - yWidth;
    }
    else
    {
        yBP         = 0;
        yThreshold = yBP + yWidth;
    }
    XYZPoint planePoint { 0., yThreshold, 0.};
    XYZVector normalVector {0., 1., 0.};
    XYZPoint innerPoint { lambda(planePoint, normalVector)};
    double   lengthInRegion { TMath::Sqrt((innerPoint - fTrackPhysics.fBoundaryPoint).Mag2())};
    double chargeInRegion {};
    for(int y = 0; y < ActParameters::g_NPADY; y++)
    {
        if(std::abs(y - yBP) > yWidth)
            continue;
        for(int x = 0; x < ActParameters::g_NPADX; x++)
        {
            chargeInRegion += pad[x][y];
        }
    }
    length = lengthInRegion;
    charge = chargeInRegion;
}

void ActTrack::CalculateTrackTotalCharge()
{
	auto totalCharge = std::accumulate(fHitArray.begin(),
									   fHitArray.end(),
									   0.,
									   [](double sum, const ActHit& hit){return hit.GetCharge() + sum;});

	fTrackPhysics.fTotalCharge            = totalCharge;
    fTrackPhysics.fAverageChargeInChamber = totalCharge / fHitArray.size();
}

void ActTrack::CalculateNumberOfSaturatedPads(const std::vector<std::vector<bool>> saturationMatrix)
{
    int counter {0};
    for(const auto& hit : fHitArray)
    {
        auto position { hit.GetPosition()};
        if(saturationMatrix.at(static_cast<int>(position.X())).at(static_cast<int>(position.Y())))
            counter++;
    }
    fTrackPhysics.fSaturatedPads = counter;
}

//2nd, set physical values
//IMPORTANT: depends on ActCalibrations bc we need to convert to
//physical lengths using coefficients set there!
// void ActTrack::SetTrackPhysics(ActCalibrations& calibrations)
// {
// 	if(!fRPInChamber)
// 		return;//since RP is not located in chamber, we cannot compute anything
// 	//and now a funtion to each physical parameter
// 	CalculateReactionPoint(calibrations);
// 	if(!fSPInArray)
// 		return;//same but with SP
// 	CalculateSiliconPoint(calibrations);
// 	CalculateTrackLength();
// 	CalculateTrackAverageCharge();
// 	CalculateThetaTrack(calibrations);
// 	CalculatePhiTrack();
// }

// void ActTrack::CalculateThetaTrack(ActCalibrations& calibrations)
// {
// 	XYZVector vector { fTrackPhysics.fSiliconPoint - fTrackPhysics.fReactionPoint};
// 	XYZVector n_x { 1., 0., 0.};//unitary along x in order to get theta
// 	auto dot { n_x.Dot(vector.Unit())};//all unitary vectors
// 	auto theta { TMath::ACos(dot)};
// 	fTrackPhysics.fTheta = TMath::RadToDeg() * theta;
// }

// void ActTrack::CalculatePhiTrack()
// {
// 	XYZVector vector { fTrackPhysics.fSiliconPoint - fTrackPhysics.fReactionPoint};
// 	XYZVector n_z { 0., 0., 1.};
// 	auto dot { n_z.Dot(vector.Unit())};
// 	auto phi { TMath::ACos(dot)};
// 	fTrackPhysics.fPhi = TMath::RadToDeg() * phi;
// }

// void ActTrack::CalculateReactionPoint(ActCalibrations& calibrations)
// {
// 	//once reaction point is computed in raw units, we have to convert it to physical units
// 	//here, we convert it to mm
// 	auto oldPoint { fTrackPhysics.fReactionPoint};
// 	XYZPoint newPoint;
// 	newPoint.SetX(oldPoint.X() * calibrations.GetXYToLengthUnitsCoef());
// 	newPoint.SetY(oldPoint.Y() * calibrations.GetXYToLengthUnitsCoef());
// 	newPoint.SetZ(oldPoint.Z() * calibrations.GetZToLengthUnitsCoef());

// 	fTrackPhysics.fReactionPoint = newPoint;
// }

// void ActTrack::CalculateSiliconPoint(ActCalibrations& calibrations)
// {
// 	auto oldPoint { fTrackPhysics.fSiliconPoint};
// 	XYZPoint newPoint;
// 	newPoint.SetX(oldPoint.X() * calibrations.GetXYToLengthUnitsCoef());
// 	newPoint.SetY(oldPoint.Y() * calibrations.GetXYToLengthUnitsCoef());
// 	newPoint.SetZ(oldPoint.Z() * calibrations.GetZToLengthUnitsCoef());

// 	fTrackPhysics.fSiliconPoint = newPoint;
// }

// void ActTrack::CalculateTrackLength()
// {
// 	//MUST BE RUN AFTER SILICON AND REACTION POINT CALCULATIONS
// 	auto rp { fTrackPhysics.fReactionPoint};
// 	auto sp { fTrackPhysics.fSiliconPoint};

// 	double dist2 { (rp - sp).Mag2()};
// 	fTrackPhysics.fTrackLength = TMath::Sqrt(dist2);
// }

// void ActTrack::CalculateTrackAverageCharge()
// {
// 	fTrackPhysics.fAverageCharge = fTrackPhysics.fTotalCharge / fTrackPhysics.fTrackLength;
// }
