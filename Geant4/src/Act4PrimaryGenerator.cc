#include "Act4PrimaryGenerator.hh"

#include "ActInputParser.h"
#include "ActKinematicGenerator.h"
#include "ActOptions.h"
#include "ActParticle.h"

#include "Randomize.hh"

#include "TLorentzVector.h"

#include "G4Box.hh"
#include "G4EmCalculator.hh"
#include "G4Event.hh"
#include "G4IonTable.hh"
#include "G4LogicalVolume.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4RunManager.hh"
#include "G4SystemOfUnits.hh"

#include <G4Exception.hh>
#include <G4ExceptionSeverity.hh>
#include <G4ThreeVector.hh>

#include <stdexcept>


ActPrimaryGenerator::ActPrimaryGenerator()
{
    // Particle gun with only one particle at each vertex
    fParticleGun = new G4ParticleGun {1};

    // Kinematics generator, initialised from config file
    auto config {ActRoot::Options::GetInstance()->GetConfigDir() + "/geant.conf"};
    ActRoot::InputParser parser {config};
    // Kinematics
    auto kin {parser.GetBlock("Kinematics")};
    auto reac {kin->GetStringVector("Reaction")};
    if(reac.size() != 4)
        G4Exception("ActPrimaryGenerator::ActPrimaryGenerator()", "ActPrimaryGenerator001", FatalErrorInArgument,
                    "Parsed Reaction from geant.conf does not contain 4 particles");
    auto ps {kin->GetDoubleVector("PS")};
    auto ex {kin->GetDouble("Ex")};
    // Beam parameters
    auto beam {parser.GetBlock("Beam")};
    fBeamCentreZ = beam->GetDouble("CentreZ");
    fBeamSigmaXY = beam->GetDouble("SigmaXY");
    fBeamSigmaZ = beam->GetDouble("SigmaZ");
    fEBeam = beam->GetDouble("Energy");

    // Initialise generator
    fKinGen = new ActSim::KinematicGenerator {reac[0], reac[1], reac[2], reac[3], (int)ps[0], (int)ps[1]};
    fKinGen->SetBeamAndExEnergies(fEBeam, ex);
    // And particle definitions
    for(int i = 0; i < reac.size(); i++)
    {
        const ActPhysics::Particle* part {};
        double excitation {};
        // Beam + target -> light: extracted from binary kinematics
        if(i < 3)
            part = &(fKinGen->GetBinaryKinematics()->GetParticle(i + 1));
        // computed from PS calculator for generality. Here explicitily add Ex
        else
        {
            part = &(fKinGen->GetHeavyPart());
            excitation = ex * MeV;
        }
        auto Z {part->GetZ()};
        auto A {part->GetA()};
        std::cout << "Z : " << Z << " A: " << A << '\n';
        auto def {G4IonTable::GetIonTable()->GetIon(Z, A, excitation)};
        if(!def)
            throw std::runtime_error("Cannot create part def for " + part->GetName());
        fPartDefs.push_back(def);
    }
}

ActPrimaryGenerator::~ActPrimaryGenerator()
{
    delete fParticleGun;
    delete fKinGen;
}

void ActPrimaryGenerator::GeneratePrimaries(G4Event* event)
{
    // Sample points
    auto* driftLog {G4LogicalVolumeStore::GetInstance()->GetVolume("driftLog")};
    G4Box* driftBox {};
    if(driftLog)
        driftBox = dynamic_cast<G4Box*>(driftLog->GetSolid());
    else
    {
        G4Exception("PrimaryGeneratorAction::GeneratePrimaries()", "MyCode0002", FatalException,
                    "Cannot retrieve driftLog solid volume");
    }
    // Get X half length
    auto xHalfLength {driftBox->GetXHalfLength()};

    // 1-> Entrance point
    G4ThreeVector entrance {0, G4RandGauss::shoot(0, fBeamSigmaXY), // assuming beam perfectly centred in Y
                            G4RandGauss::shoot(fBeamCentreZ, fBeamSigmaZ)};

    // 2-> Vertex
    // Sampling again gaussian.... not realistic bc real beam has a given emittance
    G4ThreeVector vertex {G4RandFlat::shoot(-xHalfLength, +xHalfLength),
                          G4RandGauss::shoot(0, fBeamSigmaXY), // assuming beam perfectly centred in Y
                          G4RandGauss::shoot(fBeamCentreZ, fBeamSigmaZ)};
    // 3-> Beam direction
    auto beamDir {vertex - entrance};

    // Shoot beam
    // WARNING: need to define particles before, create a separate method for this

    // fParticleGun->SetParticleDefinition(fPartDefs[0]);
    // fParticleGun->SetParticlePosition(entrance);
    // fParticleGun->SetParticleMomentumDirection(beamDir);
    // fParticleGun->SetParticleEnergy(fEBeam);
    // fParticleGun->GeneratePrimaryVertex(event);

    // // Shoot recoils
    // auto d {(vertex - entrance).r()};
    // auto EBeamAtVertex {SlowDownBeam(fPartDefs[0], fKinGen->GetEBeam(), d, driftLog->GetMaterial())};
    // fKinGen->SetBeamEnergy(EBeamAtVertex);
    // auto weight = fKinGen->Generate();
    // // Light
    // auto plight {fKinGen->GetLorentzVector(0)};
    // auto T3 {plight->E() - fKinGen->GetBinaryKinematics()->GetParticle(3).GetMass()};
    // auto theta3 {plight->Theta()};
    // auto phi3 {plight->Phi()};
    // G4ThreeVector dir3 {std::sin(theta3) * std::cos(phi3), std::sin(theta3) * std::sin(phi3), std::cos(theta3)};
    // fParticleGun->SetParticleDefinition(fPartDefs[2]);
    // fParticleGun->SetParticlePosition(vertex);
    // fParticleGun->SetParticleMomentumDirection(dir3);
    // fParticleGun->SetParticleEnergy(T3);
    // fParticleGun->GeneratePrimaryVertex(event);
}

double ActPrimaryGenerator::SlowDownBeam(G4ParticleDefinition* part, double E, double d, G4Material* mat)
{
    if(part->GetParticleName() == "neutron")
        return E;
    double dEdx {};
    double dE {};
    int nsteps {100};

    G4EmCalculator calc {};

    for(int i = 0; i < nsteps; i++)
    {
        dEdx = calc.ComputeTotalDEDX(E, part, mat);
        dE = dEdx * d / nsteps;
        E -= dE;
        if(E < 0)
        {
            E = 0;
            break;
        }
    }
    return E;
}
