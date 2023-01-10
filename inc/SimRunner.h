#ifndef SIMRUNNER_H
#define SIMRUNNER_H

#include "SimCrossSection.h"
#include "SimGeometry.h"
#include "SimKinematics.h"
#include "SimSRIM.h"
#include "SimStructs.h"

#include "TH1D.h"
#include "TMath.h"
#include "TRandom3.h"
#include "TTree.h"
#include "TVirtualPad.h"

#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include <utility>


class SimRunner
{
 public:
    using XYZPoint  = ROOT::Math::XYZPoint;
    using XYZVector = ROOT::Math::XYZVector;
 private:
    TRandom3* generator {nullptr};
    SimGeometry* geometry {nullptr};
    SimSRIM* srim {nullptr};
    SimKinematics  kinematics {};
    TTree* tree {nullptr};
    //data members
    //const through all iterations
    const SilInfo silicons;
    const DriftInfo actar;
    SimBeam* const beam;
    
    //must be reset every iteration
    IterationInfo iteration {};

    //scaling factor for each beam energy
    double scalingFactor {};

    //L1 trigger
    double fL1TriggerThreshold {14.0};//mm

    //bool to enable stragglins
    bool enableGasStraggling {true};
    bool enableSilStraggling {true};
    //silicon energy threshold
    bool enableSilEThreshold {true};
    double silEThreshold {0.5};//MeV
    //function to silicon energy straggling
    std::unique_ptr<TF1> funcSilStragg {};
    //manual angular straggling
    bool enableManualAngularStraggling {true};
    double manualAngularStragg {1.0 * TMath::DegToRad()};
    
 public:
    SimRunner(TRandom3* gen, SimGeometry* geo, SimSRIM* sri, SimBeam* const beam, TTree*& tr);
    ~SimRunner() = default;

    //scaling factor for each global iteration
    void ComputeScalingFactor(double Tn, SimCrossSection& xs, double fluxAtTn, int iterations);
    //Kinematics by copy bc we want it fresh every iteration
    void SetKinematics(SimKinematics kin){ kinematics = kin; }
    void Reset();
    
    void RunKinematicsAndGeometry(bool useGivenAngles = false, int whichAngles = 4, bool debug = false);
    void UseSRIMToObtainELoss(std::string srimInGas, std::string srimInSil);
    void UseSRIMInGas(const std::string& stringGas);
    void UseSRIMInSilicons(const std::string& stringSil);
    void ComputeL1Trigger(bool debug = false);
    void ReconstructBeamEnergy(const std::string& stringGas);
    void WriteIterationToTree();
    
    //getters of info
    double GetTheta3CM() const { return iteration.theta3CM; }
    double GetPhi3CM()   const { return iteration.phi3CM; }
    double GetTheta3Lab() const { return iteration.theta3Lab; }
    double GetPhi3Lab()  const { return iteration.phi3Lab; }
    double GetT3Lab() const { return iteration.T3Lab; }
    double GetDistanceToSil() const { return iteration.distance; }
    double GetSilEnerlyLoss() const { return iteration.silELoss; }
    double GetT3EnteringSil() const { return iteration.T3EnteringSil; }
    double GetScalingFactor() const { return scalingFactor; }
    double GetR3Left() const { return iteration.R3Left; }
    int GetAssemblyIndex() const { return iteration.assemblyIndex; }
    int GetSilType() const { return iteration.silType; }
    int GetSilIndex() const { return iteration.silIndex; }
    bool GetPunchthroughBool() const { return iteration.suffersPunchthrough; }
    IterationInfo GetIterationInfo() const { return iteration; }

    //setters
    void SetRandomThetaCM(double val){ iteration.thetaCMRads = val; iteration.thetaCMRandomSet += 1; }
    void SetRandomPhiCM(double val){ iteration.phiCMRads = val; iteration.phiCMRandomSet += 1; }
    void SetIterationEnergyIndex(int ind) { iteration.energyIndex = ind; }
    void SetEnableStragglingInGas(bool val){ enableGasStraggling = val; }
    void SetEnableStragglingInSil(bool val){ enableSilStraggling = val; }
    void SetEnableSilEnergyThreshold(bool val) { enableSilEThreshold = val; }
    void SetSilEnergyThreshold(double val){ silEThreshold = val; }
    void SetEnableManualAngularStraggling(bool val){ enableManualAngularStraggling = val; }
    void SetManualAngularStraggling(double val){ manualAngularStragg = val; }

 private:
    void SampleVertex();
    void CallComputeKinematics();
    void ComputeKinematics();
    void PropagateTrackToSilicons(bool debug = false);
    bool CompareEnergies(double ELossAtSil, double EEnteringSil, double epsilon = 1.0E-3);
};

#endif //SIMRUNNER_H
