#include "ActGeantEventAction.hh"

#include "ActGeantDataHolder.hh"

#include <Randomize.hh>

#include "G4AnalysisManager.hh"
#include "G4Event.hh"
#include "G4SystemOfUnits.hh"

#include <G4SDManager.hh>
#include <G4UserEventAction.hh>
#include <G4ios.hh>

#include <cmath>

ActGeant::EventAction::EventAction() : G4UserEventAction() {}

void ActGeant::EventAction::BeginOfEventAction(const G4Event* event) {}

void ActGeant::EventAction::EndOfEventAction(const G4Event* event)
{
    // // Data holder
    auto* holder {DataHolder::Instance()};

    // // Apply Si resolution
    auto silE {holder->fLight.fSilDeltaE};
    holder->fLight.fSilDeltaE = G4RandGauss::shoot(silE, fSiRes * std::sqrt(silE / 5.5));

    // Analysis manager
    auto* ana = G4AnalysisManager::Instance();
    // Vertex
    ana->FillNtupleDColumn(1, holder->fVertexInfo.fEBeam / MeV);
    ana->FillNtupleDColumn(2, holder->fVertexInfo.fT3 / MeV);
    ana->FillNtupleDColumn(3, holder->fVertexInfo.ftheta3 / deg);
    ana->FillNtupleDColumn(4, holder->fVertexInfo.fphi3 / deg);
    ana->FillNtupleDColumn(5, holder->fVertexInfo.fT4 / MeV);
    ana->FillNtupleDColumn(6, holder->fVertexInfo.ftheta4 / deg);
    ana->FillNtupleDColumn(7, holder->fVertexInfo.fphi4 / deg);
    ana->FillNtupleDColumn(8, holder->fVertexInfo.fthetaCM / deg);
    // Light
    ana->FillNtupleDColumn(11, holder->fLight.fTPCDeltaE / MeV);
    ana->FillNtupleDColumn(14, holder->fLight.fSilDeltaE / MeV);
    ana->FillNtupleSColumn(15, holder->fLight.fSilLayer);
    ana->FillNtupleIColumn(16, holder->fLight.fSilIdx);

    // Name
    ana->FillNtupleSColumn(17, holder->fLight.fName);

    // Write row (equivalent of TTree::Fill)
    ana->AddNtupleRow();

    // Prepare for next iteration
    holder->Clear();
}
