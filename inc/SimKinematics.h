#ifndef SIMKINEMATICS_H
#define SIMKINEMATICS_H

#include <Math/Vector3Dfwd.h>
#include <Math/Vector4Dfwd.h>
#include <Math/GenVector/BoostX.h>
#include <tuple>
#include <utility>

class SimKinematics
{
public:
	using ThreeVector = ROOT::Math::XYZVector;
	using FourVector  = ROOT::Math::PxPyPzEVector;
	using LorentzBoostX = ROOT::Math::BoostX;
	
private:
	double m1 {};
	double m2 {};
	double m3 {};
	double m4 {};
	double Eex {};
	double T1Lab {};
	double gamma {};
	double beta {};
    double Qvalue {};

	//vectors
	FourVector P1Lab {};
	FourVector P2Lab {};
	FourVector PInitialLab {};
	FourVector PInitialCM  {};
	FourVector P3CM {};
    FourVector P4CM {};
	FourVector P3Lab {};
    FourVector P4Lab {};
	LorentzBoostX BoostTransformation {};
	//auxiliar to avoid calling member functions every time
	double theta3CM {};
    double theta4CM {};
    double phi3CM {};
    double phi4CM {};
	double Ecm {};
	double T3Lab {};
    double T4Lab {};
	double theta3Lab {};
    double theta4Lab {};
    double phi3Lab {};
    double phi4Lab {};

public:
    SimKinematics() = default;
	SimKinematics(double m1, double m2, double m3, double m4,
				  double T1,
				  double Eex = 0.);
	~SimKinematics() = default;

    void ComputeRecoilKinematics(double thetaCMRads, double phiCMRads,
                                 int anglesFrom = 4, bool computeBoth = false);
    double ReconstructBeamEnergyFromLabKinematics(double T3, double theta3LabRads);
    double ReconstructExcitationEnergy(double argT3, double argTheta3LabRads);
    double ComputeTheoreticalT3(double argTheta3LabRads, const std::string& sol = {"pos"});
    
	void Print() const;

	//getters
    double GetT1Lab() const { return T1Lab; }
	double GetT3Lab() const { return T3Lab; }
    double GetT4Lab() const { return T4Lab; }
	double GetTheta3Lab() const { return theta3Lab; }
    double GetTheta4Lab() const { return theta4Lab; }
    double GetPhi3Lab() const { return phi3Lab; }
    double GetPhi4Lab() const { return phi4Lab; }
	double GetTheta3CM() const { return theta3CM; }
    double GetTheta4CM() const { return theta4CM; }
    double GetPhi3CM() const { return phi3CM; }
    double GetPhi4CM() const { return phi4CM; }
	double GetBeta() const { return beta; }
	double GetGamma() const { return gamma; }
    double GetECM() const { return Ecm; }
    double GetEex() const { return Eex; }
    double GetMass(unsigned int index) const;
    std::tuple<double, double, double, double> GetMasses() const;
	
private:
	void SetRecoilsCMKinematicsThrough3(double theta3CMRads, double phi3CMRads);
    void SetRecoilsCMKinematicsThrough4(double theta4CMRads, double phi4CMRads);
	void SetRecoil3LabKinematics();
    void SetRecoil4LabKinematics();
    void ComputeQValue();
    
    double GetPhiFromVector(FourVector vect);
    double GetThetaFromVector(FourVector vect);
};

#endif //SIMKINEMATICS_H
