#ifndef ACT4RUNACTION_HH
#define ACT4RUNACTION_HH

#include "G4UserRunAction.hh"

class G4Run;

namespace ActGeant
{
class RunAction : public G4UserRunAction
{
public:
    RunAction();
    ~RunAction() override = default;

    void BeginOfRunAction(const G4Run* run) override;
    void EndOfRunAction(const G4Run* run) override;
};
} // namespace ActGeant
#endif
