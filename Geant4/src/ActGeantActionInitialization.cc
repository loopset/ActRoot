#include "ActGeantActionInitialization.hh"

#include "ActGeantEventAction.hh"
#include "ActGeantPrimaryGenerator.hh"
#include "ActGeantRunAction.hh"
#include "ActGeantSteppingAction.hh"

#include <G4String.hh>

ActGeant::ActionInitialization::ActionInitialization(const G4String& file) : fOutFile(file) {}

void ActGeant::ActionInitialization::BuildForMaster() const
{
    auto* runAction {new RunAction {fOutFile}};
    SetUserAction(runAction);
}

void ActGeant::ActionInitialization::Build() const
{
    SetUserAction(new PrimaryGenerator);
    SetUserAction(new EventAction);
    SetUserAction(new SteppingAction);
    auto* runAction {new RunAction {fOutFile}};
    SetUserAction(runAction);
}
