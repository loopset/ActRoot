#ifndef ACTGEANTACTIONINITIALIZATION_HH
#define ACTGEANTACTIONINITIALIZATION_HH

#include "G4VUserActionInitialization.hh"

#include <G4String.hh>

namespace ActGeant
{

class DetectorConstruction;

class ActionInitialization : public G4VUserActionInitialization
{
private:
    G4String fOutFile {};

public:
    ActionInitialization(const G4String& oufile = "");
    ~ActionInitialization() override = default;

    void BuildForMaster() const override;
    void Build() const override;
};
} // namespace ActGeant

#endif
