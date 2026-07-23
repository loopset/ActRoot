#ifndef ACTGEANTEVENTACTION_HH
#define ACTGEANTEVENTACTION_HH

#include "G4Types.hh"
#include "G4UserEventAction.hh"

namespace ActGeant
{
class EventAction : public G4UserEventAction
{
private:
    G4double fGasEnergy {};

public:
    EventAction();
    ~EventAction() override = default;

    void BeginOfEventAction(const G4Event* event) override;
    void EndOfEventAction(const G4Event* event) override;

    void AddEgas(G4double de) { fGasEnergy += de; }
};
} // namespace ActGeant

#endif
