#ifndef ACT4PRIMARYGENERATOR_HH
#define ACT4PRIMARYGENERATOR_HH

#include "ActKinematicGenerator.h"

#include "G4ParticleDefinition.hh"
#include "G4VUserPrimaryGeneratorAction.hh"
#include "globals.hh"

#include <G4Material.hh>

#include <vector>

class G4ParticleGun;
class G4Event;

class ActPrimaryGenerator : public G4VUserPrimaryGeneratorAction
{
private:
    G4ParticleGun* fParticleGun {};
    ActSim::KinematicGenerator* fKinGen {};
    // Beam parameters parsed in constructor
    double fEBeam {};
    double fBeamCentreZ {}; // Beam centre wrt to pad plane in mm
    double fBeamSigmaXY {}; // Preliminary emittances (just gaussians)
    double fBeamSigmaZ {};
    std::vector<G4ParticleDefinition*> fPartDefs {};


public:
    ActPrimaryGenerator();
    ~ActPrimaryGenerator() override;

    void GeneratePrimaries(G4Event* event) override;

private:
    double SlowDownBeam(G4ParticleDefinition* part, double E, double d, G4Material* mat);
};

#endif
