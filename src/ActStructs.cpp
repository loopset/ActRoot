#include "ActStructs.h"

#include "ActParameters.h"
#include "ActCalibrations.h"

#include <TMath.h>

Silicons::Silicons()
	: fSi0(ActParameters::NrowsSi01, 0.), fSi0_cal(ActParameters::NrowsSi01, 0.),
	  fSi1(ActParameters::NrowsSi01, 0.), fSi1_cal(ActParameters::NrowsSi01, 0.),
	  fSiS(ActParameters::NrowsSiS, 0.),  fSiS_cal(ActParameters::NrowsSiS, 0.),
	  fSiF(ActParameters::NrowsSi01, 0.), fSiF_cal(ActParameters::NrowsSi01, 0.),
	  fSiBeam(ActParameters::NrowsSiBeam, 0.), fSiBeam_cal(ActParameters::NrowsSiBeam, 0.)
{
}

void TrackPhysics::Print()
{
	std::cout<<BOLDGREEN<<"===== Track "<<fTrackID<<" ====="<<RESET<<'\n';
	std::cout<<" Total charge: "<<fTotalCharge<<'\n';
	std::cout<<" Reaction point in mm at X: "<<fReactionPoint.X()<<" Y: "<<fReactionPoint.Y()<<" Z: "<<fReactionPoint.Z()<<'\n';
	std::cout<<" Silicon point "<<fSiliconPlace<<" with coordinates in mm"<<'\n';
	std::cout<<"  X: "<<fSiliconPoint.X()<<" Y: "<<fSiliconPoint.Y()<<" Z: "<<fSiliconPoint.Z()<<'\n';
	std::cout<<" Track length: "<<fTrackLength<< " mm and average charge: "<<fAverageCharge<<" / mm"<<'\n';
	std::cout<<" Theta: "<<fTheta<<" degrees and phi: "<<fPhi<<" degrees"<<'\n';
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

void TrackPhysics::CalculateTrackLength()
{
	//MUST BE RUN AFTER SILICON AND REACTION POINT CALCULATIONS
	auto rp { fReactionPoint};
	auto sp { fSiliconPoint};

	double dist2 { (rp - sp).Mag2()};
	fTrackLength = TMath::Sqrt(dist2);
}

void TrackPhysics::CalculateTrackAverageCharge()
{
	fAverageCharge = fTotalCharge / fTrackLength;
}
