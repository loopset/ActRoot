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
    // Data holder
    auto* holder {DataHolder::Instance()};

    // Apply Si resolution
    auto deltaE0 {holder->fLight.fSilDeltaE0};
    if(deltaE0 > 0)
        holder->fLight.fSilDeltaE0 = G4RandGauss::shoot(deltaE0, fSiRes * std::sqrt(deltaE0 / 5.5));
    auto deltaE1 {holder->fLight.fSilDeltaE1};
    if(deltaE1 > 0)
        holder->fLight.fSilDeltaE1 = G4RandGauss::shoot(deltaE1, fSiRes * std::sqrt(deltaE1 / 5.5));

    // Analysis manager
    auto* ana = G4AnalysisManager::Instance();
    ///////////////// Vertex
    ana->FillNtupleDColumn(1, holder->fVertexInfo.fEBeam / MeV);
    ana->FillNtupleDColumn(2, holder->fVertexInfo.fT3 / MeV);
    ana->FillNtupleDColumn(3, holder->fVertexInfo.ftheta3 / deg);
    ana->FillNtupleDColumn(4, holder->fVertexInfo.fphi3 / deg);
    ana->FillNtupleDColumn(5, holder->fVertexInfo.fT4 / MeV);
    ana->FillNtupleDColumn(6, holder->fVertexInfo.ftheta4 / deg);
    ana->FillNtupleDColumn(7, holder->fVertexInfo.fphi4 / deg);
    ana->FillNtupleDColumn(8, holder->fVertexInfo.fthetaCM / deg);

    /////////////// Light
    // TPC
    ana->FillNtupleDColumn(11, holder->fLight.fTPCDeltaE / MeV);

    // Si layer 0
    ana->FillNtupleDColumn(14, holder->fLight.fSilDeltaE0 / MeV);
    ana->FillNtupleDColumn(15, holder->fLight.fSilEAfter0 / MeV);
    ana->FillNtupleSColumn(16, holder->fLight.fSilLayer0);
    ana->FillNtupleIColumn(17, holder->fLight.fSilIdx0);
    // Si layer 1
    ana->FillNtupleDColumn(20, holder->fLight.fSilDeltaE1);
    ana->FillNtupleDColumn(21, holder->fLight.fSilEAfter1);
    ana->FillNtupleSColumn(22, holder->fLight.fSilLayer1);
    ana->FillNtupleIColumn(23, holder->fLight.fSilIdx1);

    // Particle name
    ana->FillNtupleSColumn(24, holder->fLight.fName);

    // Write row (equivalent of TTree::Fill)
    ana->AddNtupleRow();

    // Prepare for next iteration
    holder->Clear();
}
