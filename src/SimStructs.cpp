#include "SimStructs.h"
#include "SimCrossSection.h"
#include "SimBeam.h"
#include "SimGeometry.h"

#include <cstdio>
#include <iostream>
#include <stdexcept>

ExperimentInfo::ExperimentInfo(double beamRadiusCm,
                               double durationInDays,
                               double gasMass,
                               double gasPressure,
                               double gasTemp)
    : beamRadius(beamRadiusCm), gasMass(gasMass),
      gasPressure(gasPressure), gasTemp(gasTemp)
{
    DurationToDays(durationInDays);
    ComputeGasDensity();
    Print();
}

ExperimentInfo::ExperimentInfo(const DriftInfo& drift, double beamRadiusCm,
                               double durationInDays,
                               double gasMass,
                               double gasPressure,
                               double gasTemp)
    : beamRadius(beamRadiusCm), gasMass(gasMass),
      gasPressure(gasPressure), gasTemp(gasTemp)
{
    DurationToDays(durationInDays);
    ComputeGasDensity();
    ComputeNumberOfTargets(drift);
    Print();
}

void ExperimentInfo::DurationToDays(double days)
{
    duration = days * 24 * 3600;//convert from days to seconds
}

void ExperimentInfo::ComputeGasDensity()
{
    //g / mol to kg / mol
    //mb to Pa
    //kg / m3 to g /cm3
    gasDensity = ((gasMass * 1.0E-3) * (gasPressure * 100.0) /
                  ((8.314) * (gasTemp))) * 1.0E-3;
}
void ExperimentInfo::ComputeNumberOfTargets(const DriftInfo &actar)
{
    if(gasDensity == 0)
        throw std::runtime_error("You must compute density first N_{targets}");

    //assuming beam along X axis
    Nt = (6.022E23 * gasDensity) / gasMass * actar.X;
}

void ExperimentInfo::Print()
{
    std::cout<<"===== EXPERIMENT ====="<<'\n';
    std::cout<<"With duration of : "<<duration / (24 * 3600)<<" effective days"<<'\n';
    std::cout<<"Gas mass         : "<<gasMass<<" g / mol"<<'\n';
    std::cout<<"Gas pressure     : "<<gasPressure<<" mb"<<'\n';
    std::cout<<"Gas temperature  : "<<gasTemp<<" K"<<'\n';
    std::cout<<"Gas density      : "<<gasDensity<<" g / cm3"<<'\n';
    std::cout<<"N targets        : "<<Nt<<" particles / cm2"<<'\n';
    std::cout<<"Beam radius      : "<<beamRadius<<" cm"<<'\n';
    std::cout<<"======================"<<'\n';
}

void SimulationParameters::ComputeScalingFactorAfterSimulation(ExperimentInfo* exp,
                                                               SimBeam* beam,
                                                               SimCrossSection* xs)
{
    for(auto& [index, pair] : fSimuMap)
    {
        auto Tn {pair.first};
        auto iterations {fNIterMap[index]};
        auto scatteringXSAtTn { xs->EvalScatteringXS(Tn)};
        auto scalingFactor { beam->ComputeScalingFactor(exp, Tn, iterations, scatteringXSAtTn)};
        pair.second = scalingFactor;
    }
}

int SimulationParameters::FindIndexOfTn(double Tn)
{
    int val {-1};
    for(const auto& [index, pair] : fSimuMap)
    {
        if(Tn == pair.first)
        {
            val = index;
            break;
        }
    }
    return val;
}

void ExperimentInfo::ComputeRate(TH1D* const & histYield)
{
    //compute integral after integrating yield histogram
    double error {};
    double integral { histYield->IntegralAndError(1, histYield->GetNbinsX(), error)};
    rate = integral / duration;// counts / s
    urate= error / duration;
    std::cout<<"RATE for this setup : "<<rate<<" +/- "<<urate<<" Hz"<<'\n';
}
