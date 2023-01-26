#include "SimBeam.h"
#include "Rtypes.h"
#include "TCanvas.h"
#include "TMath.h"
#include "TRandom.h"
#include "TRandom3.h"
#include "TSpline.h"
#include <memory>
#include <vector>

SimBeam::SimBeam(double radius, std::vector<std::pair<double, double>> points)
    : beamRadius(radius)
{ 
}

void SimBeam::SetBeamEnergyPDF(std::vector<std::pair<double, double>> points)
{
    if(points.size() != 0)
    {
        fPoints = points;
    }
    else
    {
        //obtained for NFS at TOF = 20 m from
        //https://www.ganil-spiral2.eu/scientists/ganil-spiral-2-facilities/experimental-areas/nfs/
        fPoints = {{1.0, 1.1E4}, {2.0, 4.0E4}, {3.0, 6.0E4},
                   {4.0, 9.0E4}, {5.0, 1.2E5}, {6.0, 2.0E5},
                   {7.0, 3.4E5}, {8.0, 4.0E5}, {9.0, 5.0E5},
                   {10.0, 7.0E5}, {12.0, 1.0E6}, {15.0, 1.2E6},
                   {20.0, 8.0E5}, {22.0, 5.0E5}, {26.0, 3E5},{30.0, 1.5E5}};
    }
    std::vector<double> vE, vF, vFE;
    for(const auto& point : fPoints)
    {
        vE.push_back(point.first);
        vF.push_back(point.second);//cm^-2s^-1
        vFE.push_back(point.second / point.first);//cm^-2s^-1MeV^-1
    }
    
    splineBeam = std::make_unique<TSpline3>("splineBeam", &(vE[0]), &(vF[0]), vE.size(), "b2,e2", 0.0, 0.0);
    funcBeam   = std::make_unique<TF1>("funcBeam;T_{n} [MeV];E#frac{d#phi}{dE} [cm^{-2}s^{-1}]",
                                       [this](double* x, double* p){return splineBeam->Eval(x[0]);}, vE.front(), vE.back(), 1);
    splineFluxEnergy = std::make_unique<TSpline3>("splineFluxEnergy", &(vE[0]), &(vFE[0]), vE.size(), "b2,e2", 0.0, 0.0);
    funcFluxEnergy   = std::make_unique<TF1>("funcFluxEnergy;T_{n} [MeV];#frac{d#phi}{dE} [cm^{-2}s^{-1}MeV^{-1}]",
                                       [this](double* x, double* p){return splineFluxEnergy->Eval(x[0]);}, vE.front(), vE.back(), 1);
}

double SimBeam::SampleBeamEnergy(TRandom3* generator) const
{
    return funcBeam->GetRandom((generator) ? generator : gRandom);
}

double SimBeam::GetIntegratedFlux(bool withRadius) const
{
    double min {};
    double max {};
    funcFluxEnergy->GetRange(min, max);
    auto eval { funcFluxEnergy->Integral(min, max)};
    if(!withRadius)
        return eval;
    else
        return  eval * TMath::Pi() * TMath::Power(beamRadius, 2);
}

double SimBeam::GetFluxAtEnergy(double Tn, bool withRadius)
{
    auto fluxPerCm2 {funcBeam->Eval(Tn)};
    if(!withRadius)
        return fluxPerCm2;
    else
        return fluxPerCm2 * TMath::Pi() * TMath::Power(beamRadius, 2);
}

void SimBeam::GetBeamRange(double &minT, double &maxT) const
{
    funcBeam->GetRange(minT, maxT);
}

double SimBeam::ComputeScalingFactor(ExperimentInfo* &experiment,
                                     double Tn, long long iter,
                                     double scatteringXSAtTn)
{
    double NpAtTn {GetFluxAtEnergy(Tn) * experiment->duration};
    double xs { scatteringXSAtTn * 1.0E-24}; //b to cm2
    return (experiment->Nt * NpAtTn * xs) / iter;
}

SimBeam::XYZPoint SimBeam::SampleVertex(TRandom3* generator,
                                        const DriftInfo& actar)
{
    //Refered to actar drift chamber center
    double y {0.0};
    double z {0.0};
    do
    {
        y = generator->Gaus(0.0, beamRadius);
        z = generator->Gaus(0.0, beamRadius);
    }
    while( (std::fabs(y) >= actar.Y) || (std::fabs(z) >= actar.Z) );
    double x { generator->Uniform(-1., 1.) * actar.X};
    return { x, y, z};
}

void SimBeam::Draw()
{
    auto canvas = std::make_unique<TCanvas>("canvas", "Beam energy distribution", 1);
    canvas->cd();
    funcBeam->SetLineColor(kBlue);
    funcBeam->SetLineWidth(2);
    funcBeam->Draw();
    funcFluxEnergy->SetLineColor(kRed);
    funcFluxEnergy->SetLineWidth(2);
    funcFluxEnergy->Draw("same");
    canvas->Update();
    canvas->WaitPrimitive("lat", "");
    canvas->Close();
}
