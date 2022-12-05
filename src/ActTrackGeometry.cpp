#include "ActTrackGeometry.h"
#include "ActEvent.h"
#include "ActParameters.h"
#include "ActTrack.h"
#include "SimGeometry.h"
#include "TMath.h"
#include "TMathBase.h"
#include "TUrl.h"
#include <cmath>
#include <ios>
#include <memory>
#include <stdexcept>
#include <utility>

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
    auto satMatrixEvent { event.GetConstSaturationMatrix()};
    for(const auto& hit : track.GetHitArrayConst())
    {
        auto position {hit.GetPosition()};
        auto charge   {hit.GetCharge()};
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
    fSiliconPointMM = ScalePoint(calibrations, fSiliconPoint);
    //boundary point
    fBoundaryPointMM = ScalePoint(calibrations, fBoundaryPoint);
    //unitary direction
    fUnitaryDirectionMM = ScalePoint(calibrations, fUnitaryDirection).Unit();
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
