#ifndef Act4SimpleSD_hh
#define Act4SimpleSD_hh

#include "G4VSensitiveDetector.hh"

#include <G4HCofThisEvent.hh>
#include <G4Step.hh>
#include <G4String.hh>
#include <G4TouchableHistory.hh>
#include <G4Types.hh>

namespace ActGeant
{

class SimpleSD : public G4VSensitiveDetector
{
private:
    G4bool fIsSil {};

public:
    SimpleSD(const G4String& name);

    void Initialize(G4HCofThisEvent* hcol) override;
    G4bool ProcessHits(G4Step* step, G4TouchableHistory*) override;
    void EndOfEvent(G4HCofThisEvent* hcol) override;
};
} // namespace ActGeant

#endif // !Act4SimpleSD_hh
