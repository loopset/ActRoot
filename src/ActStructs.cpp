#include "ActStructs.h"

#include "ActParameters.h"
#include "ActCalibrations.h"

#include <TMath.h>
#include <iostream>
#include <string>

Silicons::Silicons()
	: fSi0(ActParameters::NrowsSi01, 0.), fSi0_cal(ActParameters::NrowsSi01, 0.),
	  fSi1(ActParameters::NrowsSi01, 0.), fSi1_cal(ActParameters::NrowsSi01, 0.),
	  fSiS(ActParameters::NrowsSiS, 0.),  fSiS_cal(ActParameters::NrowsSiS, 0.),
	  fSiF(ActParameters::NrowsSi01, 0.), fSiF_cal(ActParameters::NrowsSi01, 0.),
	  fSiBeam(ActParameters::NrowsSiBeam, 0.), fSiBeam_cal(ActParameters::NrowsSiBeam, 0.)
{
}

void Silicons::Print()
{
	std::cout<<BOLDCYAN<<"=========== Event Silicons =========="<<RESET<<'\n';
	std::cout<<"Side-> M: "<<fData["S"]["M"]<<" P: "<<fData["S"]["P"]<<" ES: "<<fData["S"]["ES"]<<'\n';
	std::cout<<"Front-> M: "<<fData["01F"]["M"]<<" P: "<<fData["01F"]["P"]<<" E0: "<<fData["01F"]["E0"]<<" E1: "<<fData["01F"]["E1"]<<'\n';
	std::cout<<"=========="<<RESET<<'\n';
}

void TrackPhysics::Print(std::string mode)
{
	std::cout<<BOLDGREEN<<"===== Track "<<fTrackID<<" ====="<<RESET<<'\n';
	std::cout<<" Total charge: "<<fTotalCharge<<'\n';
	if(mode == "full")
	{
	std::cout<<" Reaction point in mm at X: "<<fReactionPoint.X()<<" Y: "<<fReactionPoint.Y()<<" Z: "<<fReactionPoint.Z()<<'\n';
	std::cout<<" Boundary point in mm at X: "<<fBoundaryPoint.X()<< " Y: "<<fBoundaryPoint.Y()<<" Z: "<<fBoundaryPoint.Z()<<'\n';
	std::cout<<" Silicon point "<<fSiliconPlace<<" with coordinates in mm"<<'\n';
	std::cout<<"  X: "<<fSiliconPoint.X()<<" Y: "<<fSiliconPoint.Y()<<" Z: "<<fSiliconPoint.Z()<<'\n';
	std::cout<<" Track length: "<<fTrackLength<< " mm and average charge: "<<fAverageCharge<<" / mm"<<'\n';
	std::cout<<" Theta: "<<fTheta<<" degrees and phi: "<<fPhi<<" degrees"<<'\n';
	}
	else if (mode == "preliminary")
	{
		std::cout<<" RP in pads/time buckets at X: "<<fReactionPoint.X()<<" Y: "<<fReactionPoint.Y()<<" Z: "<<fReactionPoint.Z()<<'\n';
		std::cout<<" SP at "<<fSiliconPlace<<" with coordinates in pads/time buckets"<<'\n';
		std::cout<<"  X: "<<fSiliconPoint.X()<<" Y: "<<fSiliconPoint.Y()<<" Z: "<<fSiliconPoint.Z()<<'\n';
	}
	else
	{
		std::cout<<BOLDRED<<"Wrong string passed to TrackPhysics::Print -> Should be full or preliminary!"<<RESET<<'\n';
	}
	std::cout<<BOLDGREEN<<"============"<<RESET<<'\n'; 
}

void TrackPhysics::SetTrackFullPhysics(ActCalibrations& calibrations)
{
	if(!fRPInChamber)
		return;//since RP is not located in chamber, we cannot compute anything
	//and now a funtion to each physical parameter
	CalculateReactionPoint(calibrations);
	if(!fSPInArray)
		return;//same but with SP
	CalculateSiliconPoint(calibrations);
	CalculateBoundaryPoint(calibrations);
	CalculateTrackLength();
	CalculateTrackAverageCharge();
	//total charge already computed in ActTrack::SetMinimalTrackPhysics
	CalculateThetaTrack();
	CalculatePhiTrack();
}

void TrackPhysics::CalculateThetaTrack()
{
	XYZVector vector { fSiliconPoint - fReactionPoint};
	XYZVector n_x { 1., 0., 0.};//unitary along x in order to get theta
	auto dot { n_x.Dot(vector.Unit())};//all unitary vectors
	auto theta { TMath::ACos(dot)};
	fTheta = TMath::RadToDeg() * theta;
}

void TrackPhysics::CalculatePhiTrack()
{
	XYZVector vector { fSiliconPoint - fReactionPoint};
	XYZVector n_z { 0., 0., 1.};
	auto dot { n_z.Dot(vector.Unit())};
	auto phi { TMath::ACos(dot)};
	fPhi = TMath::RadToDeg() * phi;
}

void TrackPhysics::CalculateReactionPoint(ActCalibrations& calibrations)
{
	//once reaction point is computed in raw units, we have to convert it to physical units
	//here, we convert it to mm
	auto oldPoint { fReactionPoint};
	XYZPoint newPoint;
	newPoint.SetX(oldPoint.X() * calibrations.GetXYToLengthUnitsCoef());
	newPoint.SetY(oldPoint.Y() * calibrations.GetXYToLengthUnitsCoef());
	newPoint.SetZ(oldPoint.Z() * calibrations.GetZToLengthUnitsCoef());

	fReactionPoint = newPoint;
}

void TrackPhysics::CalculateSiliconPoint(ActCalibrations& calibrations)
{
	auto oldPoint { fSiliconPoint};
	XYZPoint newPoint;
	newPoint.SetX(oldPoint.X() * calibrations.GetXYToLengthUnitsCoef());
	newPoint.SetY(oldPoint.Y() * calibrations.GetXYToLengthUnitsCoef());
	newPoint.SetZ(oldPoint.Z() * calibrations.GetZToLengthUnitsCoef());

	fSiliconPoint = newPoint;
}

void TrackPhysics::CalculateBoundaryPoint(ActCalibrations& calibrations)
{
	if(fSiliconPlace == ActParameters::trackHitsSiliconSideLeft)
	{
		XYZPoint planePoint { 0., ActParameters::g_NPADY * calibrations.GetXYToLengthUnitsCoef(), 0.};
		XYZVector normalVector {0., 1., 0.};
		fBoundaryPoint = IntersectionTrackPlane(planePoint, normalVector);
	}
	else if(fSiliconPlace == ActParameters::trackHitsSiliconSideRight)
	{
		XYZPoint planePoint { 0., 0., 0.};
		XYZVector normalVector {0., -1., 0.};
		fBoundaryPoint = IntersectionTrackPlane(planePoint, normalVector);
	}
	else if(fSiliconPlace == ActParameters::trackHitsSiliconFront)
	{
		XYZPoint planePoint { ActParameters::g_NPADX * calibrations.GetXYToLengthUnitsCoef(), 0., 0.};
		XYZVector normalVector {1., 0., 0.};
		fBoundaryPoint = IntersectionTrackPlane(planePoint, normalVector);
	}
	else
	{
		;//do nothing
	}
}

void TrackPhysics::CalculateTrackLength()
{
	//MUST BE RUN AFTER SILICON AND REACTION POINT CALCULATIONS
	auto rp { fReactionPoint};
	auto bp { fBoundaryPoint};
	auto sp { fSiliconPoint};
	
	double dist2 { (rp - sp).Mag2()};
	double dist2Gas { (rp - bp).Mag2()};
	
	fTrackLength = TMath::Sqrt(dist2);
	fTrackLengthInGas = TMath::Sqrt(dist2Gas);
}

void TrackPhysics::CalculateTrackAverageCharge()
{
	fAverageCharge = fTotalCharge / fTrackLengthInGas;
}
