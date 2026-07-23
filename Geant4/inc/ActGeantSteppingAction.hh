#ifndef ACT4STEPPINGACTION_HH
#define ACT4STEPPINGACTION_HH

#include "G4UserSteppingAction.hh"

namespace ActGeant
{
class SteppingAction : public G4UserSteppingAction
{
public:
    SteppingAction();
    ~SteppingAction() override = default;

    void UserSteppingAction(const G4Step* step) override;
};
} // namespace ActGeant

#endif
