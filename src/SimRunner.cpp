#include "SimRunner.h"

#include "SimGeometry.h"
#include "SimSRIM.h"
#include "SimStructs.h"
#include "SimBeam.h"

#include "TCanvas.h"
#include "TH1.h"
#include "TMath.h"
#include "TRandom3.h"
#include "TString.h"
#include "TTree.h"
#include <cmath>
#include <iostream>
#include <memory>
#include <set>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

SimRunner::SimRunner(TRandom3* gen, SimGeometry* geo, SimSRIM* sri, SimBeam* const beam, TTree* &tr)
    : generator(gen), geometry(geo), srim(sri),
      silicons(geo->GetSilParameters()), actar(geo->GetDriftParameters()),
      beam(beam),
      tree(tr)
{
    tree->Branch("iter", &iteration);
    //initiaze function for energy straggling in silicons
    funcSilStragg = std::make_unique<TF1>("funcSilStragg",[this](double* x, double* p){return (0.0213 / 2.35) * TMath::Sqrt(x[0]);}, 0.0, 100.0, 1);
}

void SimRunner::Reset()
{
    iteration = {};
}

void SimRunner::RunKinematicsAndGeometry(bool useGivenAngles, int whichAngles, bool debug)
{
    //set flags
    iteration.useGivenRandomAngles = useGivenAngles;
    iteration.whichAnglesAreRandom = whichAngles;
    //run functions
    SampleVertex();
    CallComputeKinematics();
    if(debug) kinematics.Print();
    PropagateTrackToSilicons(debug);
}

void SimRunner::SampleVertex()
{
    //Refered to actar drift chamber center
    double y {0.0};
    double z {0.0};
    do
    {
        y = generator->Gaus(0.0, beam->GetBeamRadius());
        z = generator->Gaus(0.0, beam->GetBeamRadius());
    }
    while( (std::fabs(y) >= actar.Y) || (std::fabs(z) >= actar.Z) );
    double x { generator->Uniform(-1., 1.) * actar.X};
    iteration.vertex = { x, y, z};
}

void SimRunner::CallComputeKinematics()
{
    if(iteration.useGivenRandomAngles)
    {
        //check that values have been previously set by user
        if(iteration.thetaCMRandomSet != 1 || iteration.phiCMRandomSet != 1)
            throw std::runtime_error("You do want to use custom random angles but you havent't set it before running iteration!");
    }
    else
    {
        double costhetaCM { generator->Uniform(-1.0, 1.0)};
        iteration.thetaCMRads = TMath::ACos(costhetaCM);
        iteration.phiCMRads   = generator->Uniform(0.0, 2.0 * TMath::Pi());
    }
    ComputeKinematics();
}

void SimRunner::ComputeKinematics()
{
    switch (iteration.whichAnglesAreRandom)
    {
    case 3:
        iteration.theta3CM = iteration.thetaCMRads;
        iteration.phi3CM   = iteration.phiCMRads;
        kinematics.ComputeRecoilKinematics(iteration.thetaCMRads, iteration.phiCMRads, 3);
        iteration.theta4CM = kinematics.GetTheta4CM();
        iteration.phi4CM   = kinematics.GetPhi4CM();
        break;
    case 4:
        iteration.theta4CM = iteration.thetaCMRads;
        iteration.phi4CM   = iteration.phiCMRads;
        kinematics.ComputeRecoilKinematics(iteration.thetaCMRads, iteration.phiCMRads, 4);
        iteration.theta3CM = kinematics.GetTheta3CM();
        iteration.phi3CM   = kinematics.GetPhi3CM();
        break;
    default:
        throw std::runtime_error("Wrong int passed to SimKinematics as whichAnglesAreRandom: only 3 or 4 values are accepted!");
        break;
    }
    //get values in LAB
    iteration.T3Lab     = kinematics.GetT3Lab();
    iteration.theta3Lab = kinematics.GetTheta3Lab();
    iteration.phi3Lab   = kinematics.GetPhi3Lab();
    iteration.T4Lab     = kinematics.GetT4Lab();
    iteration.theta4Lab = kinematics.GetTheta4Lab();
    iteration.phi4Lab   = kinematics.GetPhi4Lab();
}

void SimRunner::PropagateTrackToSilicons(bool debug)
{
    //direction is calculated in ACTAR reference frame
    //only following 3rd particle
    iteration.direction = {TMath::Cos(iteration.theta3Lab),
        TMath::Sin(iteration.theta3Lab) * TMath::Sin(iteration.phi3Lab),
        TMath::Sin(iteration.theta3Lab) * TMath::Cos(iteration.phi3Lab)};

    //propagate
    geometry->PropagateTrackToSilicons(iteration.vertex, iteration.direction, iteration.distance, iteration.assemblyIndex, iteration.silType, iteration.silIndex, debug);
}

void SimRunner::UseSRIMInGas(const std::string& stringGas)
{
    iteration.R3Ini = srim->EvalDirect(stringGas, iteration.T3Lab);
    iteration.R3Left= iteration.R3Ini - iteration.distance * 10.0;//cm to mm
    if(iteration.R3Left <= 0.0)
    {
        //particle stopped in actar chamber bf reaching silicons
        iteration.silELoss = std::nan("-1");
        return;
    }
    //compute straggling in distance
    if(enableGasStraggling)
    {
        double straggR3Ini { srim->EvalLongStraggling(stringGas, iteration.R3Ini)};
        double straggR3Left{ srim->EvalLongStraggling(stringGas, iteration.R3Left)};
        double straggDist  { TMath::Sqrt(TMath::Power(straggR3Ini, 2) - TMath::Power(straggR3Left, 2))};
        //avoid nans
        if(straggDist < 0.0)
            straggDist = 0.0;
        //this trick to avoid negative values of R3LeftWithStraggling after randomize distance!
        double R3LeftWithStraggling {};
        do
        {
            iteration.distanceStragg = generator->Gaus(iteration.distance * 10.0, straggDist);//already in cm!
            R3LeftWithStraggling     = iteration.R3Ini - iteration.distanceStragg;
        }
        while(R3LeftWithStraggling < 0.0);
        iteration.T3EnteringSil = srim->EvalInverse(stringGas, R3LeftWithStraggling);
    }
    else
        iteration.T3EnteringSil = srim->EvalInverse(stringGas, iteration.R3Left);
}

void SimRunner::UseSRIMInSilicons(const std::string& stringSil)
{
    //now at silicons
    iteration.RIniSil       = srim->EvalDirect(stringSil, iteration.T3EnteringSil);
    //2* to convert to full-length and 10* to cm to mm
    iteration.RAfterSil     = iteration.RIniSil - silicons.unitX.at(iteration.silType) * 2 * 10.0;
    if(iteration.RAfterSil <= 0.0)
    {
        //particle stopped in silicon
        double ELossWithouStragg { iteration.T3EnteringSil};
        //punchthrough must be computed before straggling!
        iteration.suffersPunchthrough = !CompareEnergies(ELossWithouStragg, iteration.T3EnteringSil);
        iteration.silELoss = (enableSilStraggling) ? generator->Gaus(ELossWithouStragg, funcSilStragg->Eval(ELossWithouStragg)) : ELossWithouStragg;
        //implement energy threshold for silicons
        if(enableSilEThreshold)
            if(iteration.silELoss < silEThreshold)
                iteration.silELoss  = std::nan("threshold");
        iteration.T3AfterSil = 0.0;
        return;
    }
    iteration.T3AfterSil = srim->EvalInverse(stringSil, iteration.RAfterSil);
    double ELossWithoutStragg { iteration.T3EnteringSil - iteration.T3AfterSil};
    iteration.suffersPunchthrough = !CompareEnergies(ELossWithoutStragg, iteration.T3EnteringSil);
    iteration.silELoss   = (enableSilStraggling) ? generator->Gaus(ELossWithoutStragg, funcSilStragg->Eval(ELossWithoutStragg)) :ELossWithoutStragg;
    if(enableSilEThreshold)
        if(iteration.silELoss < silEThreshold)
            iteration.silELoss  = std::nan("threshold");
}

void SimRunner::UseSRIMToObtainELoss(std::string srimInGas, std::string srimInSil)
{
    throw std::runtime_error("Method not working now: Splitted into two: one for gas and other for silicons"); 
}

void SimRunner::ComputeL1Trigger(bool debug)
{
    bool isGeometricallyInside {false};
    geometry->CheckIfStepIsInsideDriftChamber(iteration.vertex, iteration.direction, fL1TriggerThreshold / 10.0, isGeometricallyInside, debug);
    //and if its geometrically feasible, check if given recoil energy it is also
    if(iteration.R3Ini >= fL1TriggerThreshold && isGeometricallyInside)
    {
        iteration.l1Trigger = true;
    }
}

void SimRunner::WriteIterationToTree()
{
    tree->Fill();
}


bool SimRunner::CompareEnergies(double ELossAtSil, double EEnteringSil, double epsilon)
{
    return (std::fabs(ELossAtSil - EEnteringSil) < epsilon);
}

void SimRunner::ReconstructBeamEnergy(const std::string& stringGas)
{
    //for this, we need again SRIM
    //punchtrough MUST BE AVOIDED. we work only over SRIM inside GAS TABLES
    double R3AtSilEntrance {srim->EvalDirect(stringGas, iteration.silELoss)};
    double RAtVertex {R3AtSilEntrance + iteration.distanceStragg};
    //eval again to obtain energy at vertex after all uncertainties
    double EAtRP { srim->EvalInverse(stringGas, RAtVertex)};
    //angular resolution according to Juan's measurements in previous experiments
    double theta3LabWithStragg { (enableManualAngularStraggling) ? generator->Gaus(iteration.theta3Lab, manualAngularStragg) : iteration.theta3Lab};
    iteration.reconstructedTn = kinematics.ReconstructBeamEnergyFromLabKinematics(EAtRP, theta3LabWithStragg);
}
