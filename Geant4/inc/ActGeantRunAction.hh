#ifndef ACT4RUNACTION_HH
#define ACT4RUNACTION_HH

#include "G4UserRunAction.hh"

#include <G4String.hh>

class G4Run;

namespace ActGeant
{
class RunAction : public G4UserRunAction
{
private:
    G4String fOutFile {};

public:
    RunAction(const G4String& outFile = "");
    ~RunAction() override = default;

    void BeginOfRunAction(const G4Run* run) override;
    void EndOfRunAction(const G4Run* run) override;
};
} // namespace ActGeant
#endif
