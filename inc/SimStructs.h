#ifndef SIMSTRUCTS_H
#define SIMSTRUCTS_H

#include "Math/Vector3D.h"
#include "Math/Point3D.h"
#include "SimGeometry.h"

#include "TH1D.h"

#include <iostream>
#include <map>
#include <string>
#include <utility>
#include <vector>

class SimBeam;
class SimCrossSection;

struct IterationInfo
{
    using XYZPoint  = ROOT::Math::XYZPoint;
    using XYZVector = ROOT::Math::XYZVector;

    int energyIndex {};
    
    XYZPoint vertex {};
    XYZVector direction {};
    
    bool useGivenRandomAngles {false};
    int whichAnglesAreRandom {};
    int thetaCMRandomSet {};
    double thetaCMRads {};
    int phiCMRandomSet {};
    double phiCMRads   {};
    
    double theta3CM {};
    double theta4CM {};
    double phi3CM {};
    double phi4CM {};
    double theta3Lab {};
    double theta4Lab {};
    double phi3Lab {};
    double phi4Lab {};
    double T3Lab  {};
    double T4Lab  {};

    double distance {};
    bool l1Trigger {};
    bool suffersPunchthrough {};
    int assemblyIndex {};
    int silType {};
    int silIndex {};

    //and now for SRIM
    double R3Ini {};
    double R3Left {};
    double distanceStragg {};
    double T3EnteringSil {};
    double T3AfterSil    {};
    double RIniSil {};
    double RAfterSil {};
    double silELoss {};

    //reconstructed Tn
    double reconstructedTn {};
};

struct ExperimentInfo
{
    double gasMass {};//g / mol
    double gasPressure {};//mb
    double gasTemp {};//K
    double gasDensity {};//g/cm3
    double duration {};//seconds
    double beamRadius {};
    double Nt {};//protons per cm2
    double rate {};//rate of experiment
    double urate{};//uncertainty

    ExperimentInfo() = default;
    ExperimentInfo(double beamRadiusCm, double durationInDays,
                   double gasMass, double gasPressure, double gasTemp = 293.15);
    ExperimentInfo(const DriftInfo& drift, double beamRadiusCm, double durationInDays,
                   double gasMass, double gasPressure, double gasTemp = 293.15);
    ExperimentInfo(double beamRadiusCm, double lengthXCm, double durationInSec,
                   double gasMass, double gasPressure, double gasTemp = 293.15);
    ~ExperimentInfo() = default;

    void DurationFromDaysToSec(double days);
    void ComputeGasDensity();
    void ComputeNumberOfTargets(const DriftInfo& actar);
    void ComputeNumberOfTargets(double x);
    void ComputeRate(TH1D* const & histYield);
    void Print();
        
};

struct SimulationParameters
{
    //geometry parameters
    SilInfo silicons {};
    DriftInfo actar  {};

    //and now vectors of index, energy and scaling factors
    std::map<int, std::pair<double, double>> fSimuMap {};
    //map for iterations per index
    std::map<int, long long int> fNIterMap {};
    //key = index; pair.first = energy and pair.second = scaling

    SimulationParameters() = default;
    SimulationParameters(SilInfo sil, DriftInfo act)
        : silicons(sil), actar(act)
    {
        
    }
    ~SimulationParameters() = default;

    void ComputeScalingFactorAfterSimulation(ExperimentInfo* exp,
                                             SimBeam* beam,
                                             SimCrossSection* xs);
    int FindIndexOfTn(double Tn);

    
};

#endif
