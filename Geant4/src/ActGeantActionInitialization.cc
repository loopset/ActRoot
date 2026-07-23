#include "ActGeantActionInitialization.hh"

#include "ActGeantEventAction.hh"
#include "ActGeantPrimaryGenerator.hh"
#include "ActGeantRunAction.hh"
#include "ActGeantSteppingAction.hh"

ActGeant::ActionInitialization::ActionInitialization(DetectorConstruction* det) : fDetConstruction(det) {}

void ActGeant::ActionInitialization::BuildForMaster() const
{
    SetUserAction(new RunAction);
}

void ActGeant::ActionInitialization::Build() const
{
    SetUserAction(new RunAction);
    SetUserAction(new PrimaryGenerator);
    SetUserAction(new EventAction);
    SetUserAction(new SteppingAction());
}
