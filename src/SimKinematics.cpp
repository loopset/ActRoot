#include "SimKinematics.h"
#include "TMathBase.h"

#include <Math/Vector3D.h>
#include <Math/Vector4D.h>
#include <Math/GenVector/BoostX.h>
#include <TMath.h>
#include <cmath>
#include <iomanip>
#include <stdexcept>
#include <string>
#include <tuple>
#include <utility>

SimKinematics::SimKinematics(double m1, double m2, double m3, double m4,
							 double T1,
							 double Eex)
	: m1(m1), m2(m2), m3(m3), m4(m4),
	  T1Lab(T1),
	  Eex(Eex)
{
    ComputeQValue();
	double E1Lab { T1Lab + m1};
	double p1Lab { TMath::Sqrt(E1Lab * E1Lab - m1 * m1)};
	P1Lab = { p1Lab, 0.0, 0.0, E1Lab};//beam along X axis! ACTAR TPC reference frame!
	P2Lab = { 0., 0., 0., m2};
	PInitialLab = P1Lab + P2Lab;

	//and now let's move to CM! For now on, we assume boost along Z axis only!
	auto betaVector { PInitialLab.BoostToCM()};
	if((betaVector.Y() != 0.) || (betaVector.Z() != 0.))
	{
		throw std::runtime_error("Error! Boost includes non-null Y and Z values -> This class only works with boost along X axis, as ACTAR TPC standard reference frame");
	}
	BoostTransformation.SetBeta(betaVector.X());
	beta = BoostTransformation.Beta();
	gamma = BoostTransformation.Gamma();
	PInitialCM = BoostTransformation(PInitialLab);
	Ecm = PInitialCM.E();
}

double SimKinematics::GetMass(unsigned int index) const
{
    switch(index)
    {
    case 1:
        return m1;
        break;
    case 2:
        return m2;
        break;
    case 3:
        return m3;
        break;
    case 4:
        return m4;
        break;
    default:
        throw std::runtime_error("Index out of range: 1, 2, 3 or 4");
        break;
    }
}

std::tuple<double, double, double, double> SimKinematics::GetMasses() const
{
    return std::make_tuple(m1, m2, m3, m4);
}

void SimKinematics::SetRecoilsCMKinematicsThrough3(double theta3CMRads, double phi3CMRads)
{
	double E3CM { 0.5 * (Ecm * Ecm + m3 * m3 - (m4 + Eex) * (m4 + Eex)) / Ecm};
	double p3CM { TMath::Sqrt(E3CM * E3CM - m3 * m3)};
	P3CM = { p3CM * TMath::Cos(theta3CMRads),
        p3CM * TMath::Sin(theta3CMRads) * TMath::Sin(phi3CMRads),
        p3CM * TMath::Sin(theta3CMRads) * TMath::Cos(phi3CMRads),
        E3CM};
	theta3CM = theta3CMRads;
    phi3CM   = phi3CMRads;

    //for 4th particle
    P4CM = PInitialCM - P3CM;
    theta4CM = GetThetaFromVector(P4CM);
    phi4CM   = GetPhiFromVector(P4CM);
}

void SimKinematics::SetRecoilsCMKinematicsThrough4(double theta4CMRads, double phi4CMRads)
{
    double E4CM { 0.5 * (Ecm * Ecm + (m4 + Eex) * (m4 + Eex) - m3 * m3) / Ecm};
    double p4CM { TMath::Sqrt(E4CM * E4CM - (m4 + Eex) * (m4 + Eex))};
    P4CM = { p4CM * TMath::Cos(theta4CMRads),
        p4CM * TMath::Sin(theta4CMRads) * TMath::Sin(phi4CMRads),
        p4CM * TMath::Sin(theta4CMRads) * TMath::Cos(phi4CMRads),
        E4CM};
    theta4CM = theta4CMRads;
    phi4CM   = phi4CMRads;

    //for 3rd particle
    P3CM = PInitialCM - P4CM;
    theta3CM = GetThetaFromVector(P3CM);
    phi3CM   = GetPhiFromVector(P3CM);
}

void SimKinematics::SetRecoil3LabKinematics()
{
	P3Lab = { BoostTransformation.Inverse()(P3CM)};
	T3Lab = P3Lab.E() - m3;
    theta3Lab = GetThetaFromVector(P3Lab);
    phi3Lab   = GetPhiFromVector(P3Lab);
}

void SimKinematics::SetRecoil4LabKinematics()
{
    P4Lab = { BoostTransformation.Inverse()(P4CM)};
    T4Lab = P4Lab.E() - (m4 + Eex);
    theta4Lab = GetThetaFromVector(P4Lab);
    phi4Lab   = GetPhiFromVector(P4Lab);
}

void SimKinematics::ComputeRecoilKinematics(double thetaCMRads, double phiCMRads,
                                            int anglesFrom, bool computeBoth)
{
    //this function allows to choose which angles are given
    if(Eex < 0.0)
        throw std::runtime_error("Cannot proceed: Eex < 0, probably you dont want to use this function depending on inner Eex!");
    
    switch (anglesFrom)
    {
    case 3:
        SetRecoilsCMKinematicsThrough3(thetaCMRads, phiCMRads);
        break;
    case 4:
        SetRecoilsCMKinematicsThrough4(thetaCMRads, phiCMRads);
        break;
    default:
        throw std::runtime_error("Wrong value passed: only 3 or 4 int values are allowed!");
        break;
    }
    //we are mainly interesed in 3rd particle
    SetRecoil3LabKinematics();
    //but if bool computeBoth passed, compute 4th particle kinematics in LAB
    if(computeBoth)
        SetRecoil4LabKinematics();
    //(in this way we save computation time)
}

void SimKinematics::Print() const
{
	std::cout<<std::fixed<<std::setprecision(2);
	std::cout<<"> Beam with energy: "<<T1Lab<<" MeV\n";
	std::cout<<"----> transforms at CM with gamma: "<<gamma<<" and beta: "<<beta<<'\n';
    std::cout<<"----> transforms at CM with E_{CM}: "<<Ecm<<'\n';
	std::cout<<"--> Recoil 3 with energy: "<<T3Lab<<" at theta: "<<theta3Lab * TMath::RadToDeg()<<" degrees and phi: "<<phi3Lab * TMath::RadToDeg()<<" degrees"<<'\n';
    std::cout<<"--> Recoil 4 with energy: "<<T4Lab<<" at theta: "<<theta4Lab * TMath::RadToDeg()<<" degrees and phi: "<<phi4Lab * TMath::RadToDeg()<<" degrees"<<'\n';
}

double SimKinematics::GetPhiFromVector(FourVector vect)
{
    double phi {};
    //we use the ATan2 function, but converting it to [0., 2pi) range
    phi = TMath::ATan2(vect.Y(), vect.Z());
    if(phi < 0.)
        phi += 2.0 * TMath::Pi();
    
    return phi;
}

double SimKinematics::GetThetaFromVector(FourVector vect)
{
    return TMath::ACos(vect.X() / TMath::Sqrt(vect.Vect().Mag2()));
}

double SimKinematics::ReconstructBeamEnergyFromLabKinematics(double argT3, double argTheta3LabRads)
{
    double a { m2 - argT3 - m3};
    double b { TMath::Sqrt(argT3 * argT3 + 2.0 * argT3 * m3) * TMath::Cos(argTheta3LabRads)};
    double c {0.5 * (m4 * m4 - m3 * m3 - m1 * m1 - m2 * m2) - m1 * (m2 - argT3 - m3) + m2 * (argT3 + m3)};
    double A { b * b - a * a};
    double B { 2.0 * (b * b * m1 + a * c)};
    double C { - c * c};
    double Delta { B * B - 4.0 * A *C };
    //std::cout<<"a: "<<a<<" b: "<<b<<" c: "<<c<<'\n';
    //std::cout<<"A: "<<A<<" B: "<<B<<" C: "<<C<<" Delta: "<<Delta<<'\n';
    if(Delta < 0.0)
        return std::nan("D<0");
    //only positive solution has physical interest
    double solPos { (- B + TMath::Sqrt(Delta)) / (2 * A)};
    //double solNeg { (- B - TMath::Sqrt(Delta)) / (2 * A)};
    return solPos;
}

void SimKinematics::ComputeQValue()
{
    Qvalue = (m1 + m2 - m3 - (m4 + Eex));
    if(Qvalue < 0.0)
    {
        double T1threshold {-Qvalue * (m1 + m2 + m3 + (m4 +Eex)) / (2.0 * m2)};
        if(T1Lab < T1threshold)
        {
            throw std::runtime_error(("Error! Reactionn has threshold energy of " + std::to_string(T1threshold) + " MeV, but given beam has only " + std::to_string(T1Lab) + " MeV!"));
        }
    }
}

double SimKinematics::ReconstructExcitationEnergy(double argT3, double argTheta3LabRads)
{
    //mass code:
    // m1 = beam
    // m2 = target (at rest)
    // m3 = light recoil (ejectile)
    // m4 = heavy recoil
    double p3 { TMath::Sqrt(argT3 * (argT3 + 2.0 * m3))};
    //std::cout<<"p3: "<<p3<<'\n';
    double E3 { argT3 + m3};//TOTAL energy
    //std::cout<<"E3: "<<E3<<'\n';
    double invariant4Mass { TMath::Power(Ecm, 2) + TMath::Power(m3, 2)
        - 2.0 * Ecm * (gamma * (E3 + beta * p3 * TMath::Cos(argTheta3LabRads)))};
    //WATCH OUT!!! in the above formula, usually one has (E3 - beta * p3 * cos())
    //but here, since beta is already negative (since we are using ROOT's lorentz transfromations)
    //we use the general +: - is already included in beta!
    double recEex {TMath::Sqrt(invariant4Mass) - m4};
    return recEex;
}

double SimKinematics::ComputeTheoreticalT3(double argTheta3LabRads, const std::string& sol)
{
    double A { (TMath::Power(Ecm, 2) + m3 * m3 - (m4 + Eex) * (m4 + Eex)) / (2.0 * gamma * Ecm)};
    double B { TMath::Abs(beta) * TMath::Cos(argTheta3LabRads)};
    double Delta { A*A * B*B - B*B * m3*m3 * (1.0 - B*B)};
    if(Delta < 0)
        return -11;
    double denom { 1.0 - B*B};
    if(sol == "pos")
    {
        auto val { (A + TMath::Sqrt(Delta)) / denom - m3};
        return val;
    }
    else if(sol == "neg")
    {
        auto val { (A - TMath::Sqrt(Delta)) / denom - m3};
        return val;
    }
    else
    {
        throw std::runtime_error("sol arg only admits two options: pos or neg!");
    }
}
