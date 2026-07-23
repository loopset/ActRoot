#include "ActGeantEventAction.hh"

#include "ActGeantDataHolder.hh"

#include "G4AnalysisManager.hh"
#include "G4Event.hh"

#include <G4SDManager.hh>
#include <G4UserEventAction.hh>
#include <G4ios.hh>

ActGeant::EventAction::EventAction() : G4UserEventAction() {}

void ActGeant::EventAction::BeginOfEventAction(const G4Event* event) {}

void ActGeant::EventAction::EndOfEventAction(const G4Event* event)
{
    // Analysis manager
    auto* ana = G4AnalysisManager::Instance();
    //
    // // Data holder
    auto* holder {DataHolder::Instance()};
    // Vertex
    ana->FillNtupleDColumn(1, holder->fVertexInfo.fEBeam);
    ana->FillNtupleDColumn(2, holder->fVertexInfo.fT3);
    ana->FillNtupleDColumn(3, holder->fVertexInfo.ftheta3);
    ana->FillNtupleDColumn(4, holder->fVertexInfo.fphi3);
    ana->FillNtupleDColumn(5, holder->fVertexInfo.fT4);
    ana->FillNtupleDColumn(6, holder->fVertexInfo.ftheta4);
    ana->FillNtupleDColumn(7, holder->fVertexInfo.fphi4);
    ana->FillNtupleDColumn(8, holder->fVertexInfo.fthetaCM);
    // Light
    ana->FillNtupleDColumn(11, holder->fLight.fTPCDeltaE);

    // Write row (equivalent of TTree::Fill)
    ana->AddNtupleRow();

    // Prepare for next iteration
    holder->Clear();
}
