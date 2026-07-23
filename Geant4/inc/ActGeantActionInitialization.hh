#ifndef ACTGEANTACTIONINITIALIZATION_HH
#define ACTGEANTACTIONINITIALIZATION_HH

#include "G4VUserActionInitialization.hh"

namespace ActGeant
{

class DetectorConstruction;

class ActionInitialization : public G4VUserActionInitialization
{
private:
    DetectorConstruction* fDetConstruction {};

public:
    ActionInitialization(DetectorConstruction*);
    ~ActionInitialization() override = default;

    void BuildForMaster() const override;
    void Build() const override;
};
} // namespace ActGeant

#endif
