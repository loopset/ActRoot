#include "ActGeantPrimaryGenerator.hh"

#include "ActCrossSection.h"
#include "ActGeantDataHolder.hh"
#include "ActInputParser.h"
#include "ActKinematicGenerator.h"
#include "ActKinematics.h"
#include "ActOptions.h"
#include "ActParticle.h"

#include "Randomize.hh"

#include "TLorentzVector.h"
#include "TROOT.h"

#include "Math/GenVector/AxisAngle.h"

#include "G4Box.hh"
#include "G4EmCalculator.hh"
#include "G4Event.hh"
#include "G4IonTable.hh"
#include "G4LogicalVolume.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4PhysicalConstants.hh"
#include "G4RunManager.hh"
#include "G4SystemOfUnits.hh"

#include <G4AnalysisManager.hh>
#include <G4Exception.hh>
#include <G4ExceptionSeverity.hh>
#include <G4String.hh>
#include <G4ThreeVector.hh>
#include <G4ios.hh>

#include <cmath>
#include <string>


ActGeant::PrimaryGenerator::PrimaryGenerator()
{
    // Particle gun with only one particle at each vertex
    fParticleGun = new G4ParticleGun {1};

    // Parse configuration file
    auto config {ActRoot::Options::GetInstance()->GetGeantFile()};
    ActRoot::InputParser parser {config};
    // Kinematics
    auto kin {parser.GetBlock("Kinematics")};
    auto reac {kin->GetStringVector("Reaction")};
    auto aux {reac[0] + "," + reac[1]};
    // Init kinematics
    fKin = new ActPhysics::Kinematics {aux};
    // And get information from it
    fEBeam = fKin->GetT1Lab();
    fEx = fKin->GetEx();
    // Send information to holder
    auto* holder {DataHolder::Instance()};
    holder->fReacInfo.fBeam = fKin->GetParticle(1).GetName();
    holder->fReacInfo.fTarget = fKin->GetParticle(2).GetName();
    holder->fReacInfo.fLight = fKin->GetParticle(3).GetName();
    holder->fReacInfo.fHeavy = fKin->GetParticle(4).GetName();
    holder->fReacInfo.fEBeam = fEBeam;
    holder->fReacInfo.fEx = fEx;

    // Phase spaces
    auto ps {kin->GetDoubleVector("PS")};
    // If any PS, create fKinGen
    if(std::any_of(ps.begin(), ps.end(), [](const auto& v) { return v != 0; }))
    {
        fKinGen = new ActSim::KinematicGenerator {fKin->GetParticle(1), fKin->GetParticle(2), fKin->GetParticle(3),
                                                  fKin->GetParticle(4), (int)ps[0],           (int)ps[1]};
        fKinGen->SetBeamAndExEnergies(fEBeam, fEx);
    }

    // Cross section
    std::string xsfile {};
    if(kin->CheckTokenExists("CrossSection", true))
        xsfile = kin->GetString("CrossSection");
    if(xsfile.size())
    {
        G4cout << "Reading cross section from file: " << xsfile << G4endl;
        ROOT::EnableThreadSafety();
        fCrossSection = new ActSim::CrossSection;
        fCrossSection->ReadUsingTGraph(xsfile);
    }

    // Beam parameters
    auto beam {parser.GetBlock("Beam")};
    fBeamCentreZ = beam->GetDouble("CentreZ");
    fBeamSigmaXY = beam->GetDouble("SigmaXY");
    fBeamSigmaZ = beam->GetDouble("SigmaZ");
    fEBeam = beam->GetDouble("Energy");

    // Particles are lazily defined in InitialiseParticles, since
    // G4IonTable is not initialised by Geant4 at the moment of calling this constructor
}

ActGeant::PrimaryGenerator::~PrimaryGenerator()
{
    delete fKin;
    if(fKinGen)
        delete fKinGen;
    if(fCrossSection)
        delete fCrossSection;
    delete fParticleGun;
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
    G4ThreeVector window {0, G4RandGauss::shoot(0, fBeamSigmaXY), // assuming beam perfectly centred in Y
                          G4RandGauss::shoot(fBeamCentreZ, fBeamSigmaZ)};

    // 2-> Vertex
    // Sampling again gaussian.... not realistic bc real beam has a given emittance
    G4ThreeVector vertex {G4RandFlat::shoot(-xHalfLength, +xHalfLength),
                          G4RandGauss::shoot(0, fBeamSigmaXY), // assuming beam perfectly centred in Y
                          G4RandGauss::shoot(fBeamCentreZ, fBeamSigmaZ)};
    // 3-> Beam direction
    auto beamDir {vertex - window};

    // Shoot beam
    // WARNING: disabled as tracking the beam and then two recoild requires a more complex implementation
    // As in NPTOOL, we simple slow down the beam energy from the entrance to the vertex

    // fParticleGun->SetParticleDefinition(fPartDefs[0]);
    // fParticleGun->SetParticlePosition(entrance);
    // fParticleGun->SetParticleMomentumDirection(beamDir);
    // fParticleGun->SetParticleEnergy(fEBeam);
    // fParticleGun->GeneratePrimaryVertex(event);

    // Slow down beam
    auto d {(vertex - window).r()};
    auto EBeamAtVertex {SlowDownBeam(fPartDefs[0], fEBeam, d, driftLog->GetMaterial())};

    // Generate kinematics
    double weight {};
    double T3 {};
    double theta3 {};
    double phi3 {};
    double T4 {};
    double theta4 {};
    double phi4 {};
    double thetaCM {};
    double phiCM {};
    // NO PS
    if(!fKinGen)
    {
        fKin->SetBeamEnergy(EBeamAtVertex);
        // Sample
        phiCM = G4RandFlat::shoot(0., 2. * pi);
        if(fCrossSection)
            thetaCM = fCrossSection->SampleCDF() * deg;
        else
            thetaCM = std::acos(G4RandFlat::shoot(-1, 1));
        // Compute
        fKin->ComputeRecoilKinematics(thetaCM, phiCM);
        T3 = fKin->GetT3Lab();
        theta3 = fKin->GetTheta3Lab();
        phi3 = fKin->GetPhi3Lab();
        T4 = fKin->GetT4Lab();
        theta4 = fKin->GetTheta4Lab();
        phi4 = fKin->GetPhi4Lab();
    }
    else
    {
        fKinGen->SetBeamEnergy(EBeamAtVertex);
        weight = fKinGen->Generate();
        // Light
        auto plight {fKinGen->GetLorentzVector(0)};
        T3 = plight->E() - fKin->GetParticle(3).GetMass();
        theta3 = plight->Theta();
        phi3 = plight->Phi();
        // Heavy
        auto pheavy {fKinGen->GetLorentzVector(1)};
        T4 = pheavy->E() - fKin->GetParticle(4).GetMass();
        theta4 = pheavy->Theta();
        phi4 = pheavy->Phi();
        // Reconstruct CM
        thetaCM = fKinGen->GetBinaryKinematics()->ReconstructTheta3CMFromLab(T3, theta3);
        phiCM = phi3;
    }

    // This is beam frame, but we must convert to geometry frame
    G4ThreeVector lightDirBeamFrame {std::cos(theta3), std::sin(theta3) * std::sin(phi3),
                                     std::sin(theta3) * std::cos(phi3)};
    G4ThreeVector lightDir {RotateToWorld(lightDirBeamFrame, beamDir)};
    // Shoot
    fParticleGun->SetParticleDefinition(fPartDefs[2]);
    fParticleGun->SetParticlePosition(vertex);
    fParticleGun->SetParticleMomentumDirection(lightDir);
    fParticleGun->SetParticleEnergy(T3);
    fParticleGun->GeneratePrimaryVertex(event);

    // Heavy
    G4ThreeVector heavyDirBeamFrame {std::cos(theta4), std::sin(theta4) * std::sin(phi4),
                                     std::sin(theta4) * std::cos(phi4)};
    G4ThreeVector heavyDir {RotateToWorld(heavyDirBeamFrame, beamDir)};
    // fParticleGun->SetParticleDefinition(fPartDefs[3]);
    // fParticleGun->SetParticlePosition(vertex);
    // fParticleGun->SetParticleMomentumDirection(heavyDir);
    // fParticleGun->SetParticleEnergy(T4);
    // fParticleGun->GeneratePrimaryVertex(event);

    // Send to data holder
    auto* holder {DataHolder::Instance()};
    auto& info {holder->fVertexInfo};
    info.fWP = DataHolder::PointToVector(window);
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
        auto& part {fKin->GetParticle(i)};
        auto Z {part.GetZ()};
        auto A {part.GetA()};
        auto ex {(i == 4) ? fEx : 0}; // explicitily add Ex for heavy fragment
        auto* def {G4IonTable::GetIonTable()->GetIon(Z, A, ex)};
        if(!def)
        {
            G4String msg {"Cannot find part def for " + part.GetName()};
            G4Exception("ActPrimaryGenerator::InitialiseParticles", "ActPrimaryGenerator001", FatalException, msg);
        }
        fPartDefs.push_back(def);
    }
}

G4ThreeVector ActGeant::PrimaryGenerator::RotateToWorld(const G4ThreeVector& vec, const G4ThreeVector& beamDir)
{

    // Usually a sampled reaction returns the angles in the beam frame
    //  But to compute geometrical things (propagate that track to a silicon)
    //  We need to work in the "geometry = world" frame,
    //  where in ACTAR the "beam" goes in {1, 0, 0}
    //  Using XYZPoint and XYZVector is easy to compute it,
    //  as quoted here: https://root-forum.cern.ch/t/get-3x3-rotation-matrix-between-two-tvector3/60070
    //  Using only GenVector classes
    auto originalFrame {beamDir.unit()};
    G4ThreeVector worldFrame {1, 0, 0};
    auto cross {worldFrame.cross(originalFrame)}; // this defines the rotation axis
    auto angle {std::acos(originalFrame.dot(worldFrame))};
    ROOT::Math::AxisAngle axis {cross, angle};
    ROOT::Math::Rotation3D rotation {axis};
    return rotation(vec);
}