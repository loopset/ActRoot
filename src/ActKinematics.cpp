#include "ActKinematics.h"
#include <cmath>
#include <stdexcept>
#include <string>
#include <vector>
#include <iostream>

ActKinematics::ActKinematics(std::string beam, std::string target,
							 double beamKinetic, double targetKinetic)
{
	if(!(isKnown(beam) && isKnown(target)))
	{
		throw std::runtime_error("Could not find particle in list of know masses!");
	}
	fBeamParticle = beam;
	SetParticle("beam", beam);
	SetParticle("target", target);
	SetBeamKineticEnergy(beamKinetic);
	SetTargetKineticEnergy(targetKinetic);
}

void ActKinematics::SetBeamKineticEnergy(double energy)
{
	fKinetics["beam"] = energy;
	fEnergies["beam"] = fKinetics["beam"] + fMasses["beam"];
}

void ActKinematics::SetTargetKineticEnergy(double energy)
{
	fKinetics["target"] = energy;
	fEnergies["target"] = fKinetics["target"] + fMasses["target"];
}

void ActKinematics::SetEjectileAndRecoil(std::string particle)
{
	if(!isKnown(particle))
	{
		throw std::runtime_error("Cannot get particle mass for ejectile");
	}
	fMasses["ejectile"] = fKnownMasses[particle];
	fMasses["recoil"] = fKnownMasses[fRecoilAssociatedToEjectile[particle]];
}

void ActKinematics::SetEjectileKineticEnergy(double energy)
{
	fKinetics["ejectile"] = energy;
	fEnergies["ejectile"] = fKinetics["ejectile"] + fMasses["ejectile"];
}

//WARNING: Assuming target at rest right now!
void ActKinematics::ComputeEnergyAtCM()
{
	fEnergyAtCM = std::sqrt(2.*fEnergies["beam"]*fMasses["target"] + std::pow(fMasses["beam"], 2)
							+ std::pow(fMasses["target"], 2)); 
}

void ActKinematics::ComputeGammaAndDelta()
{
	fGamma = (fEnergies["beam"] + fEnergies["target"]) / fEnergyAtCM;
	fBeta = std::sqrt(1. - 1. / std::pow(fGamma, 2));
}

double ActKinematics::GetRecoilInvariantMass()
{
	//require first the execution of ComputeEnergyAtCM
	ComputeEnergyAtCM();
	//then, gamma and beta
	ComputeGammaAndDelta();
	//then compute p_ejectile momentum
	double p3 { std::sqrt(std::pow(fEnergies["ejectile"], 2) - std::pow(fMasses["ejectile"], 2))};
	//finally, invariant mass
	double invariantMass {
		std::pow(fEnergyAtCM, 2) + std::pow(fMasses["ejectile"], 2) -
		2.*fEnergyAtCM*(fGamma*(fEnergies["ejectile"] - fBeta*p3*std::cos(fThetas["ejectile"])))
	};
	return invariantMass;
}

double ActKinematics::GetTheoreticalRecoilEnergy(std::string branch)
{
	//require again computation of CM energy and gamma and beta
	ComputeEnergyAtCM();
	ComputeGammaAndDelta();
	double A { (std::pow(fEnergyAtCM, 2) + std::pow(fMasses["ejectile"], 2) - std::pow(fMasses["recoil"], 2)) / (2*fEnergyAtCM*fGamma)};
	double B { fBeta * std::cos(fThetas["ejectile"])};
	double Delta { A*A * B*B - B*B*std::pow(fMasses["ejectile"], 2) * (1. - B*B)};
	if(Delta < 0)
		return -1.;
	double denom { 1. - B*B};
	double solPos { (A + std::sqrt(Delta)) / denom - fMasses["ejectile"]};
	double solNeg { (A - std::sqrt(Delta)) / denom - fMasses["ejectile"]};
	if(branch == "positive")
		return solPos;
	else
	{
		return solNeg;
	}

}
