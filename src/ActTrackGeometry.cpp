#include "ActTrackGeometry.h"
#include "ActCalibrations.h"
#include "ActEvent.h"
#include "ActParameters.h"
#include "ActTrack.h"
#include "SimGeometry.h"
#include "TMath.h"
#include "TMathBase.h"
#include "TUrl.h"
#include <algorithm>
#include <cmath>
#include <ios>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <utility>
#include <vector>

ActTrackGeometry::ActTrackGeometry(const ActEvent& event,
                                   const ActTrack& track, const std::string& side, int silIndex)
    : fSiliconPlace(side), fSilIndex(silIndex)
{
    fTrackID = track.GetTrackID();
    fGravityPoint = track.GetLine().GetPoint();
    FillMatrices(event, track);
    CalculateChargesAndSaturatedPads();
    CalculateUnitaryDirectionAndSiliconPoint(track);
    CalculateBoundaryPoint();
}

void ActTrackGeometry::CalculateChargesAndSaturatedPads()
{
    double totalCharge {}; int nSaturated {};
    for(const auto& [pair, charge] : fPad)
    {
        totalCharge += charge;
        if(fSatMatrix.at(pair))
            nSaturated++;
    }
    fNSaturatedPads            = nSaturated;
    fTotalCharge               = totalCharge;
    fAverageChargeAlongPads    = totalCharge / fPad.size();
}

void ActTrackGeometry::FillMatrices(const ActEvent& event, const ActTrack& track)
{
    const auto& satMatrixEvent { event.GetConstSaturationMatrix()};
    for(const auto& hit : track.GetHitArrayConst())
    {
        const auto& position {hit.GetPosition()};
        const auto& charge   {hit.GetCharge()};
        fHits.push_back(hit);
        fPad[{static_cast<int>(position.X()), static_cast<int>(position.Y())}] += charge;
        fSatMatrix[{static_cast<int>(position.X()), static_cast<int>(position.Y())}] =
            satMatrixEvent.at(static_cast<int>(position.X())).at(static_cast<int>(position.Y()));
    }
}

void ActTrackGeometry::CalculateUnitaryDirectionAndSiliconPoint(const ActTrack& track)
{
    //Compute intersection with plane according to silTrigger
    fSiliconPoint = IntersectionTrackPlane(ActParameters::siliconsPlacement.at(fSiliconPlace), XYZVector(0.0, 1.0, 0.0),
                                           fGravityPoint, track.GetLine().GetDirection());
    fUnitaryDirection = (fSiliconPoint - fGravityPoint).Unit();
    
    // auto gravityCenter {track.GetLine().GetPoint()};
    // XYZPoint goodSigns {
    //     ActParameters::siliconDirection.at(fSiliconPlace).at(fSilIndex).first - gravityCenter.X(),
    //     ActParameters::siliconsPlacement.at(fSiliconPlace).Y()               - gravityCenter.Y(),
    //     ActParameters::siliconDirection.at(fSiliconPlace).at(fSilIndex).second- gravityCenter.Z()
    // };

    // auto oldDirection {track.GetLine().GetDirection()};
    // XYZVector newDirection {
    //     TMath::Sign(oldDirection.X(), goodSigns.X()),
    //     TMath::Sign(oldDirection.Y(), goodSigns.Y()),
    //     TMath::Sign(oldDirection.Z(), goodSigns.Z()),
    // };

    // //write correct value to class, since we dont have the VERTEX
    // fGravityPoint     = gravityCenter;
    // fUnitaryDirection = newDirection.Unit();
    // //finally compute intersection with this new direction
    // fSiliconPoint = IntersectionTrackPlane(ActParameters::siliconsPlacement.at(fSiliconPlace), XYZVector(0., 1., 0.),
    //                                        fGravityPoint, fUnitaryDirection);
}

void ActTrackGeometry::CalculateBoundaryPoint()
{
    if(fSiliconPlace == ActParameters::trackHitsSiliconSideLeft)
    {
        XYZPoint planePoint {0.0, ActParameters::g_NPADY, 0.0};
        XYZVector normalVector { 0.0, 1.0, 0.0};
        fBoundaryPoint = IntersectionTrackPlane(planePoint, normalVector,
                                                fGravityPoint, fUnitaryDirection);
    }
    else if(fSiliconPlace == ActParameters::trackHitsSiliconSideRight)
    {
        XYZPoint planePoint {0.0, 0.0, 0.0};
        XYZVector normalVector { 0.0, -1.0, 0.0};
        fBoundaryPoint = IntersectionTrackPlane(planePoint, normalVector,
                                                fGravityPoint, fUnitaryDirection);
    }
    else
    {
        throw std::runtime_error("Wrong side passed to CalculateBoundaryPoint: correct values are set in ActParameters");
    }
    //and finally check if point is in chamber
    if(IsInChamber(fBoundaryPoint))
        fBPInChamber = true;
}
template<typename T>
T ActTrackGeometry::ScalePoint(const ActCalibrations& calibrations, const T& oldPoint)
{
    T newPoint;
	newPoint.SetX(oldPoint.X() * calibrations.GetXYToLengthUnitsCoef());
	newPoint.SetY(oldPoint.Y() * calibrations.GetXYToLengthUnitsCoef());
	newPoint.SetZ(oldPoint.Z() * calibrations.GetZToLengthUnitsCoef());

	return newPoint;
}

void ActTrackGeometry::ConvertToLengthUnits(const ActCalibrations &calibrations)
{
    //silicon points
    fSiliconPointMM     = ScalePoint(calibrations, fSiliconPoint);
    //boundary point
    fBoundaryPointMM    = ScalePoint(calibrations, fBoundaryPoint);
    //unitary direction
    fUnitaryDirectionMM = ScalePoint(calibrations, fUnitaryDirection).Unit();
    //gravity point
    fGravityPointMM     = ScalePoint(calibrations, fGravityPoint);
}

void ActTrackGeometry::Print(std::string mode) const
{
    std::cout<<BOLDGREEN<<"===== Track "<<fTrackID<<" ====="<<RESET<<'\n';
    if(mode == "pad")
    {
        std::cout<<" Number of saturated pads along track : "<<fNSaturatedPads<<'\n';
        std::cout<<"  of a total of "<<fPad.size()<<" pads filled"<<'\n';
        std::cout<<" Total charge of "<<fTotalCharge<<" and averaged over pads "<<fAverageChargeAlongPads<<'\n';
		std::cout<<" SP at "<<fSiliconPlace<<" index "<<fSilIndex<<" with coordinates in pads/time buckets"<<'\n';
		std::cout<<"  X: "<<fSiliconPoint.X()<<" Y: "<<fSiliconPoint.Y()<<" Z: "<<fSiliconPoint.Z()<<'\n';
        std::cout<<" BP at "<<fSiliconPlace<<" with coordinates in pads/time buckets"<<'\n';
		std::cout<<"  X: "<<fBoundaryPoint.X()<<" Y: "<<fBoundaryPoint.Y()<<" Z: "<<fBoundaryPoint.Z()<<'\n';
        std::cout<<"  and is in chamber ? "<<std::boolalpha<<fBPInChamber<<'\n';
        
    }
    else if(mode == "mm")
    {
        std::cout<<" Number of saturated pads along track : "<<fNSaturatedPads<<'\n';
        std::cout<<"  of a total of "<<fPad.size()<<" pads filled"<<'\n';
        std::cout<<" Total charge of "<<fTotalCharge<<" and averaged over pads "<<fAverageChargeAlongPads<<'\n';
		std::cout<<" SP at "<<fSiliconPlace<<" index "<<fSilIndex<<" with coordinates in mm"<<'\n';
		std::cout<<"  X: "<<fSiliconPointMM.X()<<" Y: "<<fSiliconPointMM.Y()<<" Z: "<<fSiliconPointMM.Z()<<'\n';
        std::cout<<" BP at "<<fSiliconPlace<<" with coordinates in mm"<<'\n';
		std::cout<<"  X: "<<fBoundaryPointMM.X()<<" Y: "<<fBoundaryPointMM.Y()<<" Z: "<<fBoundaryPointMM.Z()<<'\n';
        std::cout<<"  and is in chamber ? "<<std::boolalpha<<fBPInChamber<<'\n';
        std::cout<<" IP at "<<fSiliconPlace<<" with coordinates in mm"<<'\n';
		std::cout<<"  X: "<<fInnerPointMM.X()<<" Y: "<<fInnerPointMM.Y()<<" Z: "<<fInnerPointMM.Z()<<'\n';
        std::cout<<" And charge in region defined by (BP - IP) of"<<'\n';
        std::cout<<"  Q: "<<fChargeInRegion<<" along a length of "<<fLengthInRegion<<" mm"<<'\n';
        std::cout<<" Theta: "<<fTheta<<" degrees and phi: "<<fPhi<<" degrees"<<'\n';
        std::cout<<" RP at with coordinates in mm"<<'\n';
		std::cout<<"  X: "<<fReactionPointMM.X()<<" Y: "<<fReactionPointMM.Y()<<" Z: "<<fReactionPointMM.Z()<<'\n';
    }
    std::cout<<BOLDGREEN<<"=================="<<RESET<<'\n';
}

std::pair<double, double> ActTrackGeometry::ComputeChargeInRegion(int yPads, const ActCalibrations &calibrations)
{
    //CALCULATE INNER POINT
    fYWidthInPads = yPads;

    //auxiliar pad values
    int yBP {}; int yThreshold {};
    if(fSiliconPlace == ActParameters::trackHitsSiliconSideLeft)
    {
        yBP        = static_cast<int>(fBoundaryPoint.Y());
        yThreshold = yBP - fYWidthInPads;
    }
    else if(fSiliconPlace == ActParameters::trackHitsSiliconSideRight)
    {
        yBP        = static_cast<int>(fBoundaryPoint.Y());
        yThreshold = yBP + fYWidthInPads;
    }
    else
    {
        throw std::runtime_error("Wrong fSiliconSide passed to ComputeChargeInRegion");
    }
    XYZPoint planePoint { 0., static_cast<double>(yThreshold), 0.};
    XYZVector normalVector {0., 1., 0.};
    fInnerPoint = IntersectionTrackPlane(planePoint, normalVector,
                                         fGravityPoint, fUnitaryDirection);
    fInnerPointMM = ScalePoint(calibrations, fInnerPoint);

    //AND NOW COMPUTE CHARGE IN REGION
    double chargeInRegion {};
    int countPadsInRegion {0};
    for(const auto& [pair, charge] : fPad)
    {
        auto [x,y] = pair;
        if(std::abs(y - yBP) > fYWidthInPads)
            continue;
        chargeInRegion += charge;
        countPadsInRegion++;
    }
    if(countPadsInRegion < fPad.size())
    {
        fTrackAlsoOutside = true;
        //COMPUTE LENGTH IN REGION IN MM
        fChargeInRegion = chargeInRegion;
        fLengthInRegion = TMath::Sqrt((fBoundaryPointMM - fInnerPointMM).Mag2());
        return {fChargeInRegion, fLengthInRegion};
    }
    else
    {
        return {-1.0, -1.0};
    }
}

void ActTrackGeometry::ComputeAngles()
{
    //THETA
	XYZVector n_x { 1., 0., 0.};//unitary along x in order to get theta
	auto dot { n_x.Dot(fUnitaryDirectionMM.Unit())};//all unitary vectors
	auto theta { TMath::ACos(dot)};
	fTheta = TMath::RadToDeg() * theta;
    //PHI
    XYZVector n_z { 0., 0., 1.};
	auto dot2 { n_z.Dot(fUnitaryDirectionMM.Unit())};
	auto phi { TMath::ACos(dot2)};
	fPhi = TMath::RadToDeg() * phi;
}

void ActTrackGeometry::ComputeRPFromChargeProfile(const ActCalibrations &calibrations, TH1D*& histProfile)
{
    auto getDistanceProjToBP = [](const XYZVector& lineVector,
                                  const XYZVector& binVector)
    {
        auto dotProduct { (binVector.Dot(lineVector) / TMath::Sqrt(lineVector.Mag2()))};
            
        return dotProduct;
    };

    //set reference point and fine granularity for XY plane
    const XYZPoint& reference {fBoundaryPointMM};
    auto xyOffset {calibrations.GetXYToLengthUnitsCoef() / 3};//pad +0.5 three times
    for(const auto& hit : fHits)
    {
        auto pos { ScalePoint(calibrations, hit.GetPosition())};//HIT MUST BE CONVERTED TO MM
        auto ch  { hit.GetCharge()};
        //we divide each pad in 3 * 3 * 1 subpads (Z is already too granular)
        for(int kx = -1; kx < 2; kx++)
        {
            for(int ky = -1; ky < 2; ky++)
            {
                XYZPoint bin {pos.X() + kx * xyOffset,
                    pos.Y() + ky * xyOffset,
                    pos.Z()};

                auto binVector { bin - reference};
                //ensure correct signs for direction
                auto lineVector { (fGravityPointMM - fBoundaryPointMM).Unit()};
                auto dist { getDistanceProjToBP(lineVector, binVector)};
                histProfile->Fill(dist, ch / 9);
            }
        }
    }
    //smooth
    histProfile->Smooth();
    //find range according to last bin filled in histProfile
    for(int i = 1; i <= histProfile->GetNbinsX(); i++)
    {
        auto content {histProfile->GetBinContent(i)};
        auto center  {histProfile->GetBinCenter(i)};
        if(content > 0.0)
            fRangeInChamber = center;
    }
    //and backpropagate from BP to obtain reaction point
    fReactionPointMM = (fGravityPointMM - fBoundaryPointMM).Unit() * fRangeInChamber + fBoundaryPointMM;
}

void ActTrackGeometry::ComputeRP(const ActCalibrations &calibrations)
{
    auto* histProfile { new TH1D("histProfile", "Charge profile;Distance [mm];Charge [au]", 70, 0.0, 100.)};
    ComputeRPFromChargeProfile(calibrations, histProfile);
    delete histProfile;
}

TH1D* ActTrackGeometry::ComputeRPAndReturnProfile(const ActCalibrations &calibrations)
{
    auto* histProfile { new TH1D("histProfile", "Charge profile;Distance [mm];Charge [au]", 70, 0.0, 100.)};
    ComputeRPFromChargeProfile(calibrations, histProfile);
    return std::move(histProfile);
}

void ActTrackGeometry::GetRangeProfile(const ActCalibrations& calibrations, TH1D*& histProfile, TH1D*& histDer)
{
    auto getDistance = [](const XYZVector& lineVector,
                          const XYZVector& binVector)
    {
        auto projectionVector { (binVector.Dot(lineVector) / TMath::Sqrt(lineVector.Mag2()))};

        return projectionVector;
    };
    
    const XYZPoint& reference {fBoundaryPointMM};
    auto xyOffset {calibrations.GetXYToLengthUnitsCoef() / 4};
    std::vector<double> distance {};
    std::vector<double> charge {};
    for(const auto& hit : fHits)
    {
        auto pos { ScalePoint(calibrations, hit.GetPosition())};
        auto ch  { hit.GetCharge()};
        //we divide each pad in 3 * 3 * 3 subpads
        for(int kx = -1; kx < 2; kx++)
        {
            for(int ky = -1; ky < 2; ky++)
            {
                for(int kz = -1; kz < 0; kz++)//no loop on Z by now
                {
                    XYZPoint bin {pos.X() + kx * xyOffset,
                        pos.Y() + ky * xyOffset,
                        pos.Z()};

                    auto binVector { bin - reference};
                    //ensure correct signs for direction
                    auto lineVector { (fGravityPointMM - fBoundaryPointMM).Unit()};
                    distance.push_back(getDistance(lineVector, binVector));
                    charge.push_back(ch / 9);
                    histProfile->Fill(distance.back(), charge.back());
                    //cout
                    std::cout<<"============================="<<'\n';
                    std::cout<<"Reference X: "<<reference.x()<<" Y: "<<reference.y()<<" Z: "<<reference.z()<<" mm\n";
                    std::cout<<"Gravity  X: "<<fGravityPointMM.x()<<" Y: "<<fGravityPointMM.y()<<" Z: "<<fGravityPointMM.z()<<" mm\n";
                    std::cout<<"Bin X: "<<bin.x()<<" Y: "<<bin.y()<<" Z: "<<bin.z()<<" mm\n";
                    std::cout<<"BinVector X: "<<binVector.x()<<" Y: "<<binVector.y()<<" Z: "<<binVector.z()<<" mm\n";
                    std::cout<<"lineVector X: "<<lineVector.x()<<" Y: "<<lineVector.y()<<" Z: "<<lineVector.z()<<" mm\n";
                    auto proj {reference + lineVector*distance.back()};
                    std::cout<<"Proj X: "<<proj.x()<<" Y: "<<proj.y()<<" Z: "<<proj.z()<<" mm\n";
                    std::cout<<"Distance: "<<distance.back()<<" mm\n";
                }
            }
        }
    }
    //smooth
    histProfile->Smooth();

    //compute derivative
    for(int i = 1; i < histProfile->GetNbinsX(); i++)
    {
        histDer->SetBinContent(i, histProfile->GetBinContent(i+1) -
                               histProfile->GetBinContent(i));
    }
    //smooth
    histDer->Smooth();
    //find range according to last bin filled in histProfile
    for(int i = 1; i <= histProfile->GetNbinsX(); i++)
    {
        auto content {histProfile->GetBinContent(i)};
        auto center  {histProfile->GetBinCenter(i)};
        if(content > 0.0)
            fRangeInChamber = center;
    }
    std::cout<<"Range value: "<<fRangeInChamber<<'\n';
}

void ActTrackGeometry::ComputeRPDistance(const ActCalibrations &calibrations)
{
    double range {};
    const auto& reference { fBoundaryPointMM};
    for(const auto& hit : fHits)
    {
        auto pos { ScalePoint(calibrations, hit.GetPosition())};
        double aux {TMath::Sqrt((pos - reference).Mag2())};
        if(aux > range)
            range = aux;
    }
    std::cout<<"Range with distance: "<<range<<'\n';
}
