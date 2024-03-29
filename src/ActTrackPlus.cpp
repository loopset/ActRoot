#include "ActTrackPlus.h"
#include "ActEventPlus.h"
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
#include <utility>
#include <vector>
#include <set>

ActTrackPlus::ActTrackPlus(unsigned int run, unsigned int eve, unsigned int entry,
                           ActEventPlus* eveplus,
                           const ActTrack& track, const SiliconsPlus& silicons)
    : fRunID(run), fEventID(eve), fEntryID(entry), fTrackID(track.GetTrackID()),
      fGravityPoint(track.GetLine().GetPoint())
{
    FillPadMatrix(eveplus, track);
    CalculateSiliconPoint(track, silicons);
    CalculateBoundaryPoint();
}

void ActTrackPlus::FillPadMatrix(ActEventPlus*& eveplus, const ActTrack& track)
{
    auto originalPad {eveplus->GetPadMatrix()};
    std::set<std::pair<int, int>> posToFill {};
    double totalQ {}; unsigned int satPads {};
    for(const auto& hit : track.GetHitArrayConst())
    {
        const auto& pos {hit.GetPosition()};
        auto it = posToFill.insert({pos.X(), pos.Y()});
        if(it.second)//insertion took place: key not already present
        {//*it.first returns key inserted
            fPadMatrix[*it.first] = originalPad.at(*it.first);
            totalQ += fPadMatrix.at(*it.first).first;
            if(fPadMatrix.at(*it.first).second)
                satPads++;
        }
    }
    // double totalQ {}; unsigned int satPads {};
    // for(const auto& hit : track.GetHitArrayConst())
    // {
    //     const auto& pos { hit.GetPosition()};
    //     auto charge { hit.GetCharge()};
    //     auto satFlag { hit.GetIsSaturated()};
    //     fPadMatrix[{pos.X(), pos.Y()}].first += charge;
    //     if(satFlag)
    //     {
    //         fPadMatrix.at({pos.X(), pos.Y()}).second = true;
    //         satPads++;
    //     }
    //     totalQ += charge;
    // }
    // for(const auto& [pos, vals] : fPadMatrix)
    // {
    //     if(vals.second)
    //         std::cout<<"Sat at X = "<<pos.first<<" Y = "<<pos.second<<" with Q = "<<vals.first<<'\n';
    // }
    // std::cout<<"NSat = "<<satPads<<'\n';
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

// void ActTrackPlus::ComputeChargeInRegion(int yPads, const ActCalibrations &calibrations)
// {
//     //CALCULATE INNER POINT
//     fRegionWidth = yPads;

//     //auxiliar pad values
//     int yBP {}; int yThreshold {};
//     std::vector<int> yValsToBeChecked;
//     if(fSiliconSide == ActParameters::trackHitsSiliconSideLeft)
//     {
//         yBP        = ActParameters::g_NPADY - 1;
//         yThreshold = yBP - (fRegionWidth - 1);
//         for(auto y = yThreshold; y <= yBP; y++)
//             yValsToBeChecked.push_back(y);
//     }
//     else if(fSiliconSide == ActParameters::trackHitsSiliconSideRight)
//     {
//         yBP        = 0;
//         yThreshold = yBP + (fRegionWidth - 1);
//         for(auto y = yBP; y <= yThreshold; y++)
//             yValsToBeChecked.push_back(y);
//     }
//     else
//     {
//         throw std::runtime_error("Wrong fSiliconSide passed to ComputeChargeInRegion");
//     }
//     XYZPoint planePoint { 0., static_cast<double>(yThreshold), 0.};
//     planePoint = ScalePointOrVector(calibrations, planePoint);
    
//     XYZVector normalVector {0., 1., 0.};
//     auto unitaryDirection {(fSiliconPoint - fGravityPoint).Unit()};
//     auto innerPoint { IntersectionTrackPlane(planePoint, normalVector,
//                                              fGravityPoint, unitaryDirection)};
//     //innerPoint = ScalePointOrVector(calibrations, innerPoint);

//     //AND NOW COMPUTE CHARGE IN REGION
//     double chargeInRegion {};
//     int countPadsInRegion {};
//     bool anyPadBeyondRegion {};
//     std::set<int> yValsFilled;
//     for(const auto& [pos, vals] : fPadMatrix)
//     {
//         const auto& [x,y] = pos;
//         //check if tracks in longer than region
//         if(fSiliconSide == ActParameters::trackHitsSiliconSideLeft)
//         {
//             if(y < yThreshold)
//                 anyPadBeyondRegion = true;
//         }
//         else if(fSiliconSide == ActParameters::trackHitsSiliconSideRight)
//         {
//             if(y > yThreshold)
//                 anyPadBeyondRegion = true;
//         }
//         if(std::abs(y - yBP) > fRegionWidth)
//             continue;
//         chargeInRegion += vals.first;
//         countPadsInRegion++;
//         yValsFilled.insert(y);
//     }
//     //CHECK THAT REGION HAS ALL BINS EMPTY BINS (IN Y)
//     bool regionIsEmpty {};
//     for(const auto& y : yValsToBeChecked)
//     {
//         const bool isInSet {yValsFilled.find(y) != yValsFilled.end()};
//         if(!isInSet)
//             regionIsEmpty = true;
//     }
//     if(anyPadBeyondRegion && !regionIsEmpty)
//     {
//         //COMPUTE LENGTH IN REGION IN MM
//         fChargeInRegion = chargeInRegion;
//         fLengthInRegion = TMath::Sqrt((fBoundaryPoint - innerPoint).Mag2());
//         fPIDInRegion    = fChargeInRegion / fLengthInRegion;
//     }
//     else
//     {
//         fChargeInRegion = -11;
//         fLengthInRegion = -11;
//         fPIDInRegion    = -11;
//     }
// }

void ActTrackPlus::GetChargeProfile(const ActTrack& cluster,
                                    const ActCalibrations& calibrations,
                                    TH1D*& histProfile,
                                    bool in2D)
{
    auto getDistanceProjToBP = [](const XYZVector& lineVector,
                                  const XYZVector& binVector)
    {
        auto dotProduct { (binVector.Dot(lineVector) / TMath::Sqrt(lineVector.Mag2()))};
        return dotProduct;
    };

    //set reference point and fine granularity for XY plane
    XYZPoint reference {fBoundaryPoint};
    if(in2D)
        reference.SetZ(0.0);
    //gravity point
    XYZPoint gravity {fGravityPoint};
    if(in2D)
        gravity.SetZ(0.0);
    auto lineVector { (gravity - reference).Unit()};
    auto xyOffset {calibrations.GetXYToLengthUnitsCoef() / 3};//pad +0.5 three times
    auto zOffset  {(calibrations.GetZToLengthUnitsCoef() * ActParameters::g_REBINZ) / 3};
    for(const auto& hit : cluster.GetHitArrayConst())
    {
        //MUST BE CONVERTED TO MM
        auto pos { ScalePointOrVector(calibrations, hit.GetPosition())};
        if(in2D)
            pos.SetZ(0.0);
        auto ch  { hit.GetCharge()};
        //we divide each pad in 3 * 3 * 1 subpads (Z is already too granular)
        for(int kx = -1; kx < 2; kx++)
        {
            for(int ky = -1; ky < 2; ky++)
            {
                int zCounter {1};
                for(int kz = -1; kz < 2; kz++)
                {
                    if(in2D && (zCounter > 1))
                        break;
                    XYZPoint bin {pos.X() + kx * xyOffset,
                        pos.Y() + ky * xyOffset,
                        pos.Z() + kz * zOffset};

                    auto binVector { bin - reference};
                    auto dist { getDistanceProjToBP(lineVector, binVector)};
                    double qFactor {(in2D) ? 9.0 : 27.0};
                    histProfile->Fill(dist, ch / qFactor);
                    zCounter++;
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
    //and backpropagate from BP to obtain reaction point (done always in 3D!)
    fReactionPoint = (fGravityPoint - fBoundaryPoint).Unit() * fLengthInChamber + fBoundaryPoint;
    //finally, compute full track length
    fTrackLength = TMath::Sqrt((fSiliconPoint - fReactionPoint).Mag2());
}

void ActTrackPlus::ComputeReactionPointFromChargeProfile(const ActTrack& cluster,
                                                         const ActCalibrations& calibrations,
                                                         bool in2D,
                                                         TCanvas* canv)
{
    auto* histProfile { new TH1D("histProfile", "Charge profile;Distance to BP [mm];Charge [au]", 90, -5.0, 145.)};//maximum length of a track in the chamber
    GetChargeProfile(cluster, calibrations, histProfile, in2D);
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

std::pair<double, bool> ActTrackPlus::ComputeRMSInChargeProfile(double threshold, double safeDistance)
{
    //if RMS is above threshold, there is high likelihood that the tracks is of a carbon reaction
    auto* graph = new TGraph();
    auto xMin { fHistProfile.GetBinCenter(fHistProfile.FindFirstBinAbove(0.0))};
    auto xMax { fHistProfile.GetBinCenter(fHistProfile.FindLastBinAbove(0.0))};
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
    fProfileRMS = yRMS;
    if(std::abs(yRMS) > threshold)
        return {yRMS, true};
    else
        return {yRMS, false};
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

std::pair<double, bool> ActTrackPlus::ComputeBraggPeakPosition(double slopeThreshold, double xThreshold)
{
    //Find maximum and locate boundaries
    auto maxBin {fHistProfile.GetMaximumBin()};
    auto maxContent {fHistProfile.GetBinContent(maxBin)};
    auto xMin {fHistProfile.FindFirstBinAbove(xThreshold * maxContent)};
    auto xMax {fHistProfile.FindLastBinAbove(xThreshold * maxContent)};

    std::vector<double> contentsInRange {};
    for(int bin = xMin; bin <= xMax; bin++)
    {
        contentsInRange.push_back(fHistProfile.GetBinContent(bin));
    }

    //Theil-Sen lambda
    //method based on
    //  https://github.com/mmhs013/pyMannKendall/blob/c2be737a199a694e481d98677e0e2c2c5d21b89d/pymannkendall/pymannkendall.py#L148
    auto theil_sen = [](const std::vector<double>& x)
    {
        auto n { x.size()};
        std::vector<double> slopes {};
        //get every slope using two points without repeating them
        for(int i = 0; i < n-1; i++)
        {
            for(int j = i+1; j < n; j++)
            {
                slopes.push_back((x[j] - x[i]) / (j - i));
            }
        }
        auto slope {TMath::Median(slopes.size(), &(slopes[0]))};
        auto intercept {TMath::Median(n, &(x[0]))
            - (static_cast<double>(n - 1) / 2) * slope};
        return std::make_pair(intercept, slope);
    };
    auto [intercept, slope] = theil_sen(contentsInRange);
    fTheilSenSlope = slope;
    if(slope <= slopeThreshold)
        return {slope, true};//if negative, bragg peak is towards silicons
    else
        return {slope, false};//else, it is towards chamber
}

void ActTrackPlus::ComputeChargeCloseToRP(double padRadius)
{
    double chargeInRadius {};
    //we have to recover fReactionPoint (x,y) coordinates to pads! -> use ActParameters::padSideLength
    XYZPoint xyRP {fReactionPoint.X() / ActParameters::padSideLength,
        fReactionPoint.Y() / ActParameters::padSideLength,
        0.0};
    for(const auto& [pos, vals] : fPadMatrix)
    {
        XYZPoint xyPad {(double)pos.first, (double)pos.second, 0.0};
        double dist {TMath::Sqrt((xyPad - xyRP).Mag2())};
        if(dist <= padRadius)
            chargeInRadius += vals.first;
    }
    fChargeCloseToRP = chargeInRadius;    
}

void ActTrackPlus::ComputeEnergyAtVertexWithSRIM(SimSRIM *srim, const std::string& srimString)
{
    double RAtSil { srim->EvalDirect(srimString, fSilEnergy)};
    fRPEnergy = srim->EvalInverse(srimString, RAtSil + fTrackLength);
}

void ActTrackPlus::ReconstructBeamEnergyFromLAB(SimKinematics *kinematics)
{
    fReconstructedBeamEnergy = kinematics->ReconstructBeamEnergyFromLabKinematics(fRPEnergy, TMath::DegToRad() * fTheta);
    if(fReconstructedBeamEnergy < 0.0 || fReconstructedBeamEnergy > 100.0)
        fReconstructedBeamEnergy = -11;
}

// double ActTrackPlus::CorrectPIDInRegion(TF1 *funCorr) const
// {
//     //corrects PID in region by flattening the Q/L distribution along Z_{Sil}, to avoid dependence on drift along Z
//     return fPIDInRegion + funCorr->Eval(fSiliconPoint.Z());
//     //fIsPIDCorrected = true;
// }

double ActTrackPlus::CorrectPIDInChamber(TF1*& funCorr) const
{
    return fTotalCharge / fLengthInChamber + funCorr->Eval(fSiliconPoint.Z());
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
    // std::cout<<" Charge in region defined by BP - "<<fRegionWidth<<" pads of"<<'\n';
    // std::cout<<"  Q: "<<fChargeInRegion<<" along a length of "<<fLengthInRegion<<" mm"<<'\n';
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
