#include "ActTrackPlus.h"
#include "ActParameters.h"
#include "ActStructs.h"
#include "ActTrack.h"
#include "RtypesCore.h"
#include "TCanvas.h"
#include "TGraph.h"
#include "TMath.h"
#include <cmath>
#include <ios>
#include <iostream>
#include <string>
#include <vector>

ActTrackPlus::ActTrackPlus(unsigned int run, unsigned int eve, unsigned int entry,
                           const ActTrack& track, const SiliconsPlus& silicons)
    : fRunID(run), fEventID(eve), fEntryID(entry), fTrackID(track.GetTrackID()),
      fGravityPoint(track.GetLine().GetPoint())
{
    FillPadMatrix(track);
    CalculateSiliconPoint(track, silicons);
    CalculateBoundaryPoint();
}

void ActTrackPlus::FillPadMatrix(const ActTrack& track)
{
    double totalQ {}; unsigned int satPads {};
    for(const auto& hit : track.GetHitArrayConst())
    {
        const auto& pos { hit.GetPosition()};
        const auto& charge { hit.GetCharge()};
        const auto& satFlag { hit.GetIsSaturated()};
        fPadMatrix[{pos.X(), pos.Y()}].first += charge;
        if(satFlag)
        {
            fPadMatrix.at({pos.X(), pos.Y()}).second = true;
            satPads++;
        }
        totalQ += charge;
    }
    fNSatPads = satPads;
    fTotalCharge = totalQ;
    fChargePerPad = totalQ / fPadMatrix.size();
}

void ActTrackPlus::CalculateSiliconPoint(const ActTrack& track, const SiliconsPlus& silicons)
{
    auto [silSide, silIndex] = silicons.GetSilSideAndIndex();
    fSilEnergy    = silicons.fData.at(silSide).at("E");
    fSiliconSide  = silSide;
    fSiliconIndex = silIndex;
    
    fSiliconPoint = IntersectionTrackPlane(ActParameters::siliconsPlacement.at(fSiliconSide), XYZVector(0.0, 1.0, 0.0),
                                           fGravityPoint, track.GetLine().GetDirection());
}

void ActTrackPlus::CalculateBoundaryPoint()
{
    auto unitaryDirection {(fSiliconPoint - fGravityPoint).Unit()};
    
    if(fSiliconSide == ActParameters::trackHitsSiliconSideLeft)
    {
        XYZPoint planePoint {0.0, ActParameters::g_NPADY - 1, 0.0};//because pads go from [0,31] (32 excluded)
        XYZVector normalVector { 0.0, 1.0, 0.0};
        fBoundaryPoint = IntersectionTrackPlane(planePoint, normalVector,
                                                fGravityPoint, unitaryDirection);
    }
    else if(fSiliconSide == ActParameters::trackHitsSiliconSideRight)
    {
        XYZPoint planePoint {0.0, 0.0, 0.0};
        XYZVector normalVector { 0.0, -1.0, 0.0};
        fBoundaryPoint = IntersectionTrackPlane(planePoint, normalVector,
                                                fGravityPoint, unitaryDirection);
    }
    else
    {
        throw std::runtime_error("Wrong side passed to CalculateBoundaryPoint: correct values are set in ActParameters");
    }
    //and finally check if point is in chamber
    if(IsInChamber(fBoundaryPoint))
        fBPInChamber = true;
}

void ActTrackPlus::ConvertToLengthUnits(const ActCalibrations &calibrations)
{
    //this method overrides the previously set values to the new ones in cm!
    //silicons
    fSiliconPoint = ScalePointOrVector(calibrations, fSiliconPoint);
    //boundary
    fBoundaryPoint = ScalePointOrVector(calibrations, fBoundaryPoint);
    //gravity point
    fGravityPoint = ScalePointOrVector(calibrations, fGravityPoint);
}

void ActTrackPlus::MatchSPWithRealPlacement()
{
    auto xLims {ActParameters::siliconsXPlacement.at(fSiliconSide).at(fSiliconIndex)};
    if(xLims.first <= fSiliconPoint.X() && fSiliconPoint.X() <= xLims.second)
        fMatchesRealSilicon = true;
}

void ActTrackPlus::ComputeAngles()
{
    auto unitaryDirection {(fSiliconPoint - fGravityPoint).Unit()};
    //THETA
	XYZVector n_x { 1., 0., 0.};//unitary along x in order to get theta
	auto dot { n_x.Dot(unitaryDirection)};//all unitary vectors
	auto theta { TMath::ACos(dot)};
	fTheta = TMath::RadToDeg() * theta;
    //PHI
    XYZVector n_z { 0., 0., 1.};
	auto dot2 { n_z.Dot(unitaryDirection)};
	auto phi { TMath::ACos(dot2)};
	fPhi = TMath::RadToDeg() * phi;
}

void ActTrackPlus::ComputeChargeInRegion(int yPads, const ActCalibrations &calibrations)
{
    //CALCULATE INNER POINT
    fRegionWidth = yPads;

    //auxiliar pad values
    int yBP {}; int yThreshold {};
    std::vector<int> yValsToBeChecked;
    if(fSiliconSide == ActParameters::trackHitsSiliconSideLeft)
    {
        yBP        = ActParameters::g_NPADY - 1;
        yThreshold = yBP - (fRegionWidth - 1);
        for(auto y = yThreshold; y <= yBP; y++)
            yValsToBeChecked.push_back(y);
    }
    else if(fSiliconSide == ActParameters::trackHitsSiliconSideRight)
    {
        yBP        = 0;
        yThreshold = yBP + (fRegionWidth - 1);
        for(auto y = yBP; y <= yThreshold; y++)
            yValsToBeChecked.push_back(y);
    }
    else
    {
        throw std::runtime_error("Wrong fSiliconSide passed to ComputeChargeInRegion");
    }
    XYZPoint planePoint { 0., static_cast<double>(yThreshold), 0.};
    planePoint = ScalePointOrVector(calibrations, planePoint);
    
    XYZVector normalVector {0., 1., 0.};
    auto unitaryDirection {(fSiliconPoint - fGravityPoint).Unit()};
    auto innerPoint { IntersectionTrackPlane(planePoint, normalVector,
                                             fGravityPoint, unitaryDirection)};
    //innerPoint = ScalePointOrVector(calibrations, innerPoint);

    //AND NOW COMPUTE CHARGE IN REGION
    double chargeInRegion {};
    int countPadsInRegion {};
    bool anyPadBeyondRegion {};
    std::vector<int> yValsFilled;
    for(const auto& [pos, vals] : fPadMatrix)
    {
        const auto& [x,y] = pos;
        //check if tracks in longer than region
        if(fSiliconSide == ActParameters::trackHitsSiliconSideLeft)
        {
            if(y < yThreshold)
                anyPadBeyondRegion = true;
        }
        else if(fSiliconSide == ActParameters::trackHitsSiliconSideRight)
        {
            if(y > yThreshold)
                anyPadBeyondRegion = true;
        }
        if(std::abs(y - yBP) > fRegionWidth)
            continue;
        chargeInRegion += vals.first;
        countPadsInRegion++;
        yValsFilled.push_back(y);
    }
    //CHECK THAT REGION HAS ALL BINS EMPTY BINS (IN Y)
    bool regionIsEmpty {};
    for(const auto& y : yValsToBeChecked)
    {
        if(!isInVector(y, yValsFilled))
            regionIsEmpty = true;
    }
    if(anyPadBeyondRegion && !regionIsEmpty)
    {
        //COMPUTE LENGTH IN REGION IN MM
        fChargeInRegion = chargeInRegion;
        fLengthInRegion = TMath::Sqrt((fBoundaryPoint - innerPoint).Mag2());
        fPIDInRegion    = fChargeInRegion / fLengthInRegion;
    }
    else
    {
        fChargeInRegion = -11;
        fLengthInRegion = -11;
        fPIDInRegion    = -11;
    }
}

void ActTrackPlus::GetChargeProfile(const ActTrack& cluster,
                                    const ActCalibrations& calibrations,
                                    TH1D*& histProfile)
{
    auto getDistanceProjToBP = [](const XYZVector& lineVector,
                                  const XYZVector& binVector)
    {
        auto dotProduct { (binVector.Dot(lineVector) / TMath::Sqrt(lineVector.Mag2()))};
        return dotProduct;
    };

    //set reference point and fine granularity for XY plane
    const XYZPoint& reference {fBoundaryPoint};
    //ensure correct signs for line vector
    auto lineVector { (fGravityPoint - fBoundaryPoint).Unit()};
    auto xyOffset {calibrations.GetXYToLengthUnitsCoef() / 3};//pad +0.5 three times
    auto zOffset  {(calibrations.GetZToLengthUnitsCoef() * ActParameters::g_REBINZ) / 3};
    for(const auto& hit : cluster.GetHitArrayConst())
    {
        //MUST BE CONVERTED TO MM
        auto pos { ScalePointOrVector(calibrations, hit.GetPosition())};
        auto ch  { hit.GetCharge()};
        //we divide each pad in 3 * 3 * 1 subpads (Z is already too granular)
        for(int kx = -1; kx < 2; kx++)
        {
            for(int ky = -1; ky < 2; ky++)
            {
                for(int kz = -1; kz < 2; kz++)
                {
                    XYZPoint bin {pos.X() + kx * xyOffset,
                        pos.Y() + ky * xyOffset,
                        pos.Z() + kz * zOffset};

                    auto binVector { bin - reference};
                    auto dist { getDistanceProjToBP(lineVector, binVector)};
                    histProfile->Fill(dist, ch / 27);
                }
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
            fLengthInChamber = center;
    }
    //and backpropagate from BP to obtain reaction point
    fReactionPoint = (fGravityPoint - fBoundaryPoint).Unit() * fLengthInChamber + fBoundaryPoint;
}

void ActTrackPlus::ComputeReactionPointFromChargeProfile(const ActTrack& cluster,
                                                         const ActCalibrations& calibrations,
                                                         TCanvas* canv)
{
    auto* histProfile { new TH1D("histProfile", "Charge profile;Distance to BP [mm];Charge [au]", 90, -5.0, 145.)};//maximum length of a track in the chamber
    GetChargeProfile(cluster, calibrations, histProfile);
    if(canv)
    {
        canv->cd();
        canv->Clear();
        histProfile->Draw("histe");
        canv->Update();
        canv->cd();
        canv->WaitPrimitive();
    }
    //save histo to file
    TH1::AddDirectory(kFALSE);
    histProfile->Copy(fHistProfile);
    delete histProfile;
}

void ActTrackPlus::CountNumberOfPeaksInChargeProfile(double sigma, std::string options, double threshold)
{
    //use TSpectrum to compute number of peaks along charge profile
    //why? carbon reactions deposit a huge amount of charge in a narrow region -> only one peak
    fNPeaksInChargeProfile = fHistProfile.ShowPeaks(sigma,
                                                    (options + "goff nodraw").c_str(),
                                                    threshold);
}

bool ActTrackPlus::ComputeRMSInChargeProfile(double threshold)
{
    //if RMS is above threshold, there is high likelihood that the tracks is of a carbon reaction
    auto* graph = new TGraph();
    auto xMin { fHistProfile.GetBinCenter(fHistProfile.FindFirstBinAbove(0.0))};
    auto xMax { fHistProfile.GetBinCenter(fHistProfile.FindLastBinAbove(0.0))};
    auto safeDistance {4.0};//mm
    for(int i = 1; i <= fHistProfile.GetNbinsX(); i++)
    {
        auto center { fHistProfile.GetBinCenter(i)};
        auto content{ fHistProfile.GetBinContent(i)};
        if(content <= 0.0)
            continue;
        //avoid points at the beginning and end of track! there the profile always goes to 0
        if(!((xMin + safeDistance) <= center && center <= (xMax - safeDistance)))
            continue;
        graph->SetPoint(graph->GetN(), center, content);
    }
    double yRMS {graph->GetRMS(2)};//2 for Y axis
    delete graph;
    if(std::abs(yRMS) > threshold)
        return true;
    else
        return false;
}

bool ActTrackPlus::ComputeBraggPeakInChargeProfile(double threshold)
{
    //it's as simple as getting the maximum
    double xMax {};
    double contentMax {};
    for(int binx = 1; binx <= fHistProfile.GetNbinsX(); binx++)
    {
        auto content { fHistProfile.GetBinContent(binx)};
        if(content > contentMax)
        {
            contentMax = content;
            xMax = fHistProfile.GetBinCenter(binx);
        }
    }
    auto rate { xMax / fLengthInChamber};
    if(rate > threshold)
        return true;
    else
        return false;
}

void ActTrackPlus::ComputeEnergyAtVertexWithSRIM(SimSRIM *srim, const std::string& srimString)
{
    fTrackLength = TMath::Sqrt((fSiliconPoint - fReactionPoint).Mag2());
    double RAtSil { srim->EvalDirect(srimString, fSilEnergy)};
    fRPEnergy = srim->EvalInverse(srimString, RAtSil + fTrackLength);
}

void ActTrackPlus::ReconstructBeamEnergyFromLAB(SimKinematics *kinematics)
{
    fReconstructedBeamEnergy = kinematics->ReconstructBeamEnergyFromLabKinematics(fRPEnergy, TMath::DegToRad() * fTheta);
    if(fReconstructedBeamEnergy < 0.0 || fReconstructedBeamEnergy > 500.0)
        fReconstructedBeamEnergy = -11;
}

double ActTrackPlus::CorrectPIDInRegion(TF1 *funCorr) const
{
    //corrects PID in region by flattening the Q/L distribution along Z_{Sil}, to avoid dependence on drift along Z
    return fPIDInRegion + funCorr->Eval(fSiliconPoint.Z());
    //fIsPIDCorrected = true;
}

void ActTrackPlus::Print() const
{
    std::cout<<BOLDGREEN<<"===== Track "<<fTrackID<<" ====="<<RESET<<'\n';
    std::cout<<BOLDCYAN<<"== Run "<<fRunID<<" Entry "<<fEntryID<<" =="<<RESET<<'\n';
    std::cout<<" Number of saturated pads along track : "<<fNSatPads<<'\n';
    std::cout<<"  of a total of "<<fPadMatrix.size()<<" pads filled"<<'\n';
    std::cout<<" Total charge of "<<fTotalCharge<<" and averaged over pads "<<fChargePerPad<<'\n';
    std::cout<<" SiliconPoint at "<<'\n';
    std::cout<<BOLDGREEN<<"  Side: "<<fSiliconSide<<" with Index: "<<fSiliconIndex<<RESET<<'\n';
    std::cout<<"  Coordinates "<<fSiliconPoint<<" mm and Energy: "<<fSilEnergy<<" MeV"<<'\n';
    std::cout<<BOLDGREEN<<"  And matches real Sil placement ? "<<std::boolalpha<<fMatchesRealSilicon<<RESET<<'\n';
    std::cout<<" BoundaryPoint at "<<fSiliconSide<<'\n';
    std::cout<<"  with coordinates "<<fBoundaryPoint<<" mm"<<'\n';
    std::cout<<"  and is in chamber ? "<<std::boolalpha<<fBPInChamber<<'\n';
    std::cout<<" Charge in region defined by BP - "<<fRegionWidth<<" pads of"<<'\n';
    std::cout<<"  Q: "<<fChargeInRegion<<" along a length of "<<fLengthInRegion<<" mm"<<'\n';
    std::cout<<BOLDGREEN<<" Theta: "<<fTheta<<RESET<<" degrees and phi: "<<fPhi<<" degrees"<<'\n';
    std::cout<<" ReactionPoint with coordinates "<<fReactionPoint<<" mm"<<'\n';
    std::cout<<BOLDGREEN<<"  energy at RP "<<fRPEnergy<<" MeV"<<RESET<<'\n';
    std::cout<<" Total track length of "<<fTrackLength<<" mm"<<'\n';
    std::cout<<" And recoil energy at RP: "<<fRPEnergy<<" MeV"<<'\n';
    std::cout<<" Beam energy reconstructed: "<<fReconstructedBeamEnergy<<" MeV"<<'\n';
    //std::cout<<" Goes over Analysis Cuts ? "<<BOLDGREEN<<std::boolalpha<<fInsideAnalysisCuts<<RESET<<'\n';
    std::cout<<BOLDGREEN<<"=================="<<RESET<<std::endl;
}

void ActTrackPlus::WriteToStreamer(std::ofstream &streamer, const int& anaEntry) const
{
    if(anaEntry != -1)
        streamer<<fRunID<<" "<<fEntryID<<" "<<anaEntry<<'\n';
    else
        streamer<<fRunID<<" "<<fEntryID<<'\n';
}
