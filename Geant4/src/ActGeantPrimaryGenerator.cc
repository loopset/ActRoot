#include "ActGeantPrimaryGenerator.hh"

#include "ActGeantDataHolder.hh"
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

#include <G4AnalysisManager.hh>
#include <G4Exception.hh>
#include <G4ExceptionSeverity.hh>
#include <G4String.hh>
#include <G4ThreeVector.hh>
#include <G4ios.hh>

#include <string>


ActGeant::PrimaryGenerator::PrimaryGenerator()
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
    // Particles are lazily defined in InitialiseParticles, since
    // G4IonTable is not initialised by Geant4 at the moment of calling this constructor
}

ActGeant::PrimaryGenerator::~PrimaryGenerator()
{
    delete fParticleGun;
    delete fKinGen;
}

void ActGeant::PrimaryGenerator::GeneratePrimaries(G4Event* event)
{
    // Initialise particles
    InitialiseParticles();

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

    // Shoot recoils
    auto d {(vertex - entrance).r()};
    auto EBeamAtVertex {SlowDownBeam(fPartDefs[0], fEBeam, d, driftLog->GetMaterial())};
    // G4cout << "Ev : " << event->GetEventID() << " EBeamAtVertex: " << EBeamAtVertex << '\n';
    fKinGen->SetBeamEnergy(EBeamAtVertex);
    auto weight = fKinGen->Generate();

    // Light
    auto plight {fKinGen->GetLorentzVector(0)};
    fKinGen->Print();
    auto T3 {plight->E() - fKinGen->GetBinaryKinematics()->GetParticle(3).GetMass()};
    G4cout << "PLightE : " << plight->E() << " Mass : " << fKinGen->GetBinaryKinematics()->GetParticle(3).GetMass()
           << G4endl;
    auto theta3 {plight->Theta()};
    auto phi3 {plight->Phi()};
    // Kin generator uses ACTAR TPC reference frame from legacy ROOT simulations, so
    // here we have to switch X <-> Z coordinates
    G4ThreeVector dir3 {std::cos(theta3), std::sin(theta3) * std::sin(phi3), std::sin(theta3) * std::cos(phi3)};
    // Reconstruct thetaCM
    auto thetaCM {fKinGen->GetBinaryKinematics()->ReconstructTheta3CMFromLab(T3, theta3)};
    // Shoot
    fParticleGun->SetParticleDefinition(fPartDefs[2]);
    fParticleGun->SetParticlePosition(vertex);
    fParticleGun->SetParticleMomentumDirection(dir3);
    fParticleGun->SetParticleEnergy(T3);
    fParticleGun->GeneratePrimaryVertex(event);

    // Heavy
    auto pheavy {fKinGen->GetLorentzVector(1)};
    auto T4 {pheavy->E() - fKinGen->GetBinaryKinematics()->GetParticle(4).GetMass()};
    auto theta4 {pheavy->Theta()};
    auto phi4 {pheavy->Phi()};
    G4ThreeVector dir4 {std::cos(theta4), std::sin(theta4) * std::sin(phi4), std::sin(theta4) * std::cos(phi4)};
    fParticleGun->SetParticleDefinition(fPartDefs[3]);
    fParticleGun->SetParticlePosition(vertex);
    fParticleGun->SetParticleMomentumDirection(dir4);
    fParticleGun->SetParticleEnergy(T4);
    fParticleGun->GeneratePrimaryVertex(event);

    // Send to data holder
    auto* holder {DataHolder::Instance()};
    auto& info {holder->fVertexInfo};
    info.fRP = DataHolder::PointToVector(vertex);
    info.fEBeam = EBeamAtVertex;
    info.fT3 = T3;
    info.ftheta3 = theta3;
    info.fphi3 = phi3;
    info.fT4 = T4;
    info.ftheta4 = theta4;
    info.fphi4 = phi4;
    info.fthetaCM = thetaCM;
}

double ActGeant::PrimaryGenerator::SlowDownBeam(G4ParticleDefinition* part, double E, double d, G4Material* mat)
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

void ActGeant::PrimaryGenerator::InitialiseParticles()
{
    // Already initialised
    if(fPartDefs.size())
        return;

    for(int i = 1; i <= 4; i++)
    {
        auto& part {fKinGen->GetBinaryKinematics()->GetParticle(i)};
        auto Z {part.GetZ()};
        auto A {part.GetA()};
        auto ex {(i == 4) ? fKinGen->GetEx() : 0}; // explicitily add Ex for heavy fragment
        // std::cout << "Z: " << Z << " A: " << A << " Ex: " << ex << '\n';
        auto* def {G4IonTable::GetIonTable()->GetIon(Z, A, ex)};
        if(!def)
        {
            G4String msg {"Cannot find part def for " + part.GetName()};
            G4Exception("ActPrimaryGenerator::InitialiseParticles", "ActPrimaryGenerator001", FatalException, msg);
        }
        fPartDefs.push_back(def);
    }
}
