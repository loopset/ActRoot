#include "ActStructs.h"

#include "ActParameters.h"
#include "ActCalibrations.h"

#include <TMath.h>
#include <iostream>
#include <string>
#include <utility>

Silicons::Silicons()
	: fSilLeft0(ActParameters::NSilLeft0), fSilLeft0_cal(ActParameters::NSilLeft0),
      fSilLeft1(ActParameters::NSilLeft1), fSilLeft1_cal(ActParameters::NSilLeft1),
      fSilRight0(ActParameters::NSilRight0), fSilRight0_cal(ActParameters::NSilRight0),
      fSilRight1(ActParameters::NSilRight1), fSilRight1_cal(ActParameters::NSilRight1),
      fSilFront0(ActParameters::NSilFront0), fSilFront0_cal(ActParameters::NSilFront0),
      fSilFront1(ActParameters::NSilFront1), fSilFront1_cal(ActParameters::NSilFront1),
      fSiBeam(ActParameters::NrowsSiBeam, 0.), fSiBeam_cal(ActParameters::NrowsSiBeam, 0.),
      fSilSide0({{"left", std::vector<double>(ActParameters::NSilLeft0)}, {"right", std::vector<double>(ActParameters::NSilRight0)}}),
      fSilSide0_cal({{"left", std::vector<double>(ActParameters::NSilLeft0)}, {"right", std::vector<double>(ActParameters::NSilRight0)}})
{
}

void Silicons::Print() const
{
	std::cout<<BOLDCYAN<<"=========== Event Silicons =========="<<RESET<<'\n';
	// std::cout<<"Side-> M: "<<fData["S"]["M"]<<" P: "<<fData["S"]["P"]<<" ES: "<<fData["S"]["ES"]<<'\n';
	// std::cout<<"Front-> M: "<<fData["01F"]["M"]<<" P: "<<fData["01F"]["P"]<<" E0: "<<fData["01F"]["E0"]<<" E1: "<<fData["01F"]["E1"]<<'\n';
    for(const auto& [side, vecRaw] : fSilSide0)
    {
        int silIndex {0};
        for(const auto& val : vecRaw)
        {
            std::cout<<"\tSide: "<<side<<" sil: "<<silIndex + 1<<" RAW: "<<val<<" Channel CAL: "<<fSilSide0_cal.at(side).at(silIndex)<<" MeV"<<'\n';

            silIndex++;
        }
    }
    std::cout<<"------------------------------------------------"<<'\n';
    std::cout<<"Left-> M: "<<fData.at("left").at("M")<<" P: "<<fData.at("left").at("P")<<" E: "<<fData.at("left").at("E")<<'\n';
	std::cout<<"Right-> M: "<<fData.at("right").at("M")<<" P: "<<fData.at("right").at("P")<<" E: "<<fData.at("right").at("E")<<'\n';
	std::cout<<"================================================"<<RESET<<'\n';
}

std::pair<std::string, int> Silicons::GetSilSideAndIndex() const
{
    //return first side with M = 1 and its index
    int silIndex {-1};
    std::string side {"none"};
    for(const auto& [key, innerMap] : fData)
    {
        if(innerMap.at("M") == 1)
        {
            side     = key;
            silIndex = innerMap.at("P");
            break;
        }
    }

    return {side, silIndex};
}

void TrackPhysics::Print(std::string mode) const
{
	std::cout<<BOLDGREEN<<"===== Track "<<fTrackID<<" ====="<<RESET<<'\n';
	std::cout<<" Total charge: "<<fTotalCharge<<'\n';
	if(mode == "full")
	{
	std::cout<<" Reaction point in mm at X: "<<fReactionPoint.X()<<" Y: "<<fReactionPoint.Y()<<" Z: "<<fReactionPoint.Z()<<'\n';
	std::cout<<" Boundary point in mm at X: "<<fBoundaryPoint.X()<< " Y: "<<fBoundaryPoint.Y()<<" Z: "<<fBoundaryPoint.Z()<<'\n';
	std::cout<<" Silicon point "<<fSiliconPlace<<" with coordinates in mm"<<'\n';
	std::cout<<"  X: "<<fSiliconPoint.X()<<" Y: "<<fSiliconPoint.Y()<<" Z: "<<fSiliconPoint.Z()<<'\n';
    std::cout<<" Inner point in mm at X   : "<<fInnerPoint.X()<<" Y: "<<fInnerPoint.Y()<<" Z: "<<fInnerPoint.Z()<<'\n';
    std::cout<<" Charge in region         : "<<fChargeInRegion<<" and length in region: "<<fTrackLengthInRegion<<" mm"<<'\n';
	std::cout<<" Track length: "<<fTrackLength<< " mm and average charge: "<<fAverageCharge<<" / mm"<<'\n';
	std::cout<<" Theta: "<<fTheta<<" degrees and phi: "<<fPhi<<" degrees"<<'\n';
	}
	else if (mode == "preliminary")
	{
        std::cout<<" Number of saturated pads along track : "<<fSaturatedPads<<'\n';
		std::cout<<" RP in pads/time buckets at X: "<<fReactionPoint.X()<<" Y: "<<fReactionPoint.Y()<<" Z: "<<fReactionPoint.Z()<<'\n';
		std::cout<<" SP at "<<fSiliconPlace<<" with coordinates in pads/time buckets"<<'\n';
		std::cout<<"  X: "<<fSiliconPoint.X()<<" Y: "<<fSiliconPoint.Y()<<" Z: "<<fSiliconPoint.Z()<<'\n';
        std::cout<<" BP at "<<fSiliconPlace<<" with coordinates in pads/time buckets"<<'\n';
		std::cout<<"  X: "<<fBoundaryPoint.X()<<" Y: "<<fBoundaryPoint.Y()<<" Z: "<<fBoundaryPoint.Z()<<'\n';
        std::cout<<"  and is in chamber ? "<<fBPInChamber<<'\n';
        std::cout<<" IP at "<<fSiliconPlace<<" with coordinates in pads/time buckets"<<'\n';
		std::cout<<"  X: "<<fInnerPoint.X()<<" Y: "<<fInnerPoint.Y()<<" Z: "<<fInnerPoint.Z()<<'\n';
        std::cout<<"  with charge in region (BP - IP): "<<fChargeInRegion<<" au"<<'\n';
	}
	else
	{
		std::cout<<BOLDRED<<"Wrong string passed to TrackPhysics::Print -> Should be full or preliminary!"<<RESET<<'\n';
	}
	std::cout<<BOLDGREEN<<"============"<<RESET<<'\n'; 
}

void EventInfo::Print() const
{
    std::cout<<BOLDMAGENTA<<"===== Auxiliar Event Info ====="<<'\n';
    std::cout<<" Number of saturated pads in EVENT : "<<fSaturatedPadsEvent<<'\n';
    std::cout<<" Charge averaged over pads in EVENT: "<<fAverageChargeEvent<<'\n';
    for(int i = 0; i < fTrackID.size(); i++)
    {
        std::cout<<" For track ID: "<<fTrackID.at(i);
        std::cout<<"  Number of saturated pads  : "<<fSaturatedPads.at(i)<<'\n';
        std::cout<<"  Charge averaged over pads : "<<fAverageChargeAlongPads.at(i)<<'\n';
    }
    std::cout<<BOLDMAGENTA<<"==============================="<<RESET<<'\n';
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

TrackPhysics::XYZPoint TrackPhysics::ScalePoint(const ActCalibrations& calibrations, const XYZPoint& oldPoint)
{
	XYZPoint newPoint;
	newPoint.SetX(oldPoint.X() * calibrations.GetXYToLengthUnitsCoef());
	newPoint.SetY(oldPoint.Y() * calibrations.GetXYToLengthUnitsCoef());
	newPoint.SetZ(oldPoint.Z() * calibrations.GetZToLengthUnitsCoef());

	return newPoint;
}

void TrackPhysics::SetTrackPhysicsForNFS(const ActCalibrations &calibrations)
{
    //rescale points
    fSiliconPoint  = ScalePoint(calibrations, fSiliconPoint);
    fBoundaryPoint = ScalePoint(calibrations, fBoundaryPoint);
    fInnerPoint    = ScalePoint(calibrations, fInnerPoint);
    //we have to scale direction too!

    //and compute inner region distance correctly
    CalculateLengthInRegion();
}

void TrackPhysics::CalculateLengthInRegion()
{
    fTrackLengthInRegion = TMath::Sqrt((fBoundaryPoint - fInnerPoint).Mag2());//now in mm!
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

void TrackPhysics::CalculateReactionPoint(const ActCalibrations& calibrations)
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

void TrackPhysics::CalculateSiliconPoint(const ActCalibrations& calibrations)
{
	auto oldPoint { fSiliconPoint};
	XYZPoint newPoint;
	newPoint.SetX(oldPoint.X() * calibrations.GetXYToLengthUnitsCoef());
	newPoint.SetY(oldPoint.Y() * calibrations.GetXYToLengthUnitsCoef());
	newPoint.SetZ(oldPoint.Z() * calibrations.GetZToLengthUnitsCoef());

	fSiliconPoint = newPoint;
}

void TrackPhysics::CalculateBoundaryPoint(const ActCalibrations& calibrations)
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
