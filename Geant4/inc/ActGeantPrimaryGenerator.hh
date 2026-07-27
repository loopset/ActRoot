#ifndef ACT4PRIMARYGENERATOR_HH
#define ACT4PRIMARYGENERATOR_HH

#include "G4ParticleDefinition.hh"
#include "G4VUserPrimaryGeneratorAction.hh"

#include <G4Material.hh>
#include <G4ThreeVector.hh>

#include <vector>

// forward declarations
class G4ParticleGun;
class G4Event;

namespace ActPhysics
{
class Kinematics;
}
namespace ActSim
{
class KinematicGenerator;
class CrossSection;
}

namespace ActGeant
{
class PrimaryGenerator : public G4VUserPrimaryGeneratorAction
{
private:
    G4ParticleGun* fParticleGun {};
    ActPhysics::Kinematics* fKin {};
    ActSim::KinematicGenerator* fKinGen {};
    ActSim::CrossSection* fCrossSection {};
    // Beam parameters parsed in constructor
    double fEBeam {};
    double fEx {};
    double fBeamCentreZ {}; // Beam centre wrt to pad plane in mm
    double fBeamSigmaXY {}; // Preliminary emittances (just gaussians)
    double fBeamSigmaZ {};
    std::vector<G4ParticleDefinition*> fPartDefs {};


public:
    PrimaryGenerator();
    ~PrimaryGenerator() override;

    void GeneratePrimaries(G4Event* event) override;

private:
    double SlowDownBeam(G4ParticleDefinition* part, double E, double d, G4Material* mat);
    void InitialiseParticles();
    G4ThreeVector RotateToWorld(const G4ThreeVector& vec, const G4ThreeVector& beamDir);
};
} // namespace ActGeant
#endif
