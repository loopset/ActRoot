#ifndef ACTKINEMATICS_H
#define ACTKINEMATICS_H

//Class for managing kinematics of a (binary) reaction
#include <cstring>
#include <map>
#include <string>
#include <utility>
#include <vector>

#include <TMath.h>

class ActKinematics
{
private:
	//store masses for multiple particles
	double fUToMeV { 931.494};
	double fElectron { 0.511};//MeV
	std::map<std::string, double> fKnownMasses {{"p", 1.0078250319 * fUToMeV - fElectron},
												{"d", 2.0141017778 * fUToMeV - fElectron},
												{"t", 3.0160492813 * fUToMeV - fElectron},
												{"20O", 20.004075 * fUToMeV - 8 * fElectron},
												{"19O", 19.003578 * fUToMeV - 8 * fElectron},
												{"18O", 17.999159612 * fUToMeV - 8 * fElectron}};
	std::map<std::string, std::pair<std::string, std::string>> fReactions {};
	// list of known reactions!
	// -> key is ejectile (particle identified in ID)
	// -> pair containts:
	//           - first: target particle
	//           - second: recoil particle
	std::map<std::string, std::pair<std::string, std::string>> fElasticInelasticReactions {{"p", std::make_pair("p", "20O")},
																						   {"d", std::make_pair("d", "20O")},
																						   {"t", std::make_pair("t", "20O")}};
	std::map<std::string, std::pair<std::string, std::string>> fTransferReactions {{"p", std::make_pair("p", "20O")},//include elastic one bc is feasible
																				   {"d", std::make_pair("p", "19O")},
																				   {"t", std::make_pair("d", "19O")}};
	
	std::map<std::string, double> fMasses {};
	std::map<std::string, double> fEnergies {};//total energies at lab
	std::map<std::string, double> fKinetics {};//kinetic energies at lab
	std::map<std::string, double> fThetas {};//internally stored in rads at lab
	std::string fBeamParticle {""};
	double fGamma {};
	double fBeta {};
	double fEnergyAtCM {};//energy available at CM
	//auxiliar variables to store energies in constructor
	double fInitialBeamKineticEnergy {};
	

public:
	ActKinematics(std::string beam, std::string target, double beamKinetic, double targetKinetic, std::string reactionType);
	~ActKinematics() = default;

	void SetBeamKineticEnergy(double energy);
	void SetTargetKineticEnergy(double energy);
	void SetEjectileKineticEnergy(double energy);
	void SetEjectileAngle(double angle){ fThetas["ejectile"] = TMath::DegToRad() * angle; }
	void SetParticle(std::string type, std::string particle){fMasses[type] = fKnownMasses[particle]; }
	void SetEjectileAndRecoil(std::string ejectile);
	void SetReactionType(std::string reactionType);
	void ResetBeamEnergy();

	double GetBeamKineticEnergy() { return fKinetics["beam"]; }
	std::string GetBeamParticle() const { return fBeamParticle; }
	double GetRecoilInvariantMass();
	double GetBeta() const { return fBeta; }
	double GetGamma() const { return fGamma; }
	double GetEnergyAtCM() const { return fEnergyAtCM; }
	double GetMass(std::string type) { return fMasses[type]; }

	//auxiliar funtion to compute recoil energy knowing theta and Etot
	double GetTheoreticalRecoilEnergy(std::string branch = "positive");

private:
	void ComputeGammaAndDelta();
	void ComputeEnergyAtCM();

	inline bool isKnown(std::string val)
	{
		if(fKnownMasses.find(val) == fKnownMasses.end())
			return false;
		else
			return true;
	}
	
};

#endif //ACTKINEMATICS_H
