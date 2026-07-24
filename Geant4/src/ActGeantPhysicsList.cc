#include "ActGeantPhysicsList.hh"

// #include "G4DecayPhysics.hh"
#include "G4EmStandardPhysics_option4.hh"
#include "G4HadronElasticPhysics.hh"
#include "G4IonPhysics.hh"
#include "G4StoppingPhysics.hh"

#include <G4EmParameters.hh>
#include <G4VModularPhysicsList.hh>

ActGeant::PhysicsList::PhysicsList() : G4VModularPhysicsList()
{
    SetVerboseLevel(1);
    // // Activate transport with multiple scattering: DOES NOT WORK BC IT IS HARDCODED IN EmStandard....
    // auto* emPars {G4EmParameters::Instance()};
    // emPars->SetTransportationWithMsc(G4TransportationWithMscType::fEnabled);
    RegisterPhysics(new G4EmStandardPhysics_option4);
    RegisterPhysics(new G4HadronElasticPhysics);
    RegisterPhysics(new G4IonPhysics);
    RegisterPhysics(new G4StoppingPhysics);

    // Ignore decays
    // RegisterPhysics(new G4DecayPhysics);
}

void ActGeant::PhysicsList::ConstructParticle()
{
    G4VModularPhysicsList::ConstructParticle();
}