#ifndef ACTGEANTEVENTACTION_HH
#define ACTGEANTEVENTACTION_HH

#include "G4SystemOfUnits.hh"
#include "G4Types.hh"
#include "G4UserEventAction.hh"

namespace ActGeant
{
class EventAction : public G4UserEventAction
{
private:
    G4double fSiRes {0.25}; // Silicon resolution, so far hardcoded. Measured at 5.5 MeV
public:
    EventAction();
    ~EventAction() override = default;

    void BeginOfEventAction(const G4Event* event) override;
    void EndOfEventAction(const G4Event* event) override;
};
} // namespace ActGeant

#endif
