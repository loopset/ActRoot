#include "ActGeantRunAction.hh"

#include "ActGeantDataHolder.hh"

#include "TString.h"

#include "G4AnalysisManager.hh"
#include "G4Run.hh"
#include "G4RunManager.hh"

#include <G4Material.hh>
#include <G4MaterialTable.hh>
#include <G4String.hh>
#include <G4Threading.hh>
#include <G4ios.hh>

#include <vector>

ActGeant::RunAction::RunAction()
{
    G4RunManager::GetRunManager()->SetPrintProgress(true);
    auto* ana = G4AnalysisManager::Instance();
    ana->SetVerboseLevel(1);
    ana->SetNtupleMerging(true);

    // Holder
    auto* holder {DataHolder::Instance()};

    // Add columns
    ana->CreateNtuple("ActGeant", "A simple NTuple created with Geant4");
    // Vertex info
    ana->CreateNtupleDColumn("RP", holder->fVertexInfo.fRP); // idx 0
    ana->CreateNtupleDColumn("EBeam");                       // idx 1
    ana->CreateNtupleDColumn("T3");
    ana->CreateNtupleDColumn("theta3");
    ana->CreateNtupleDColumn("phi3");
    ana->CreateNtupleDColumn("T4");
    ana->CreateNtupleDColumn("theta4");
    ana->CreateNtupleDColumn("phi4");
    ana->CreateNtupleDColumn("thetaCM"); // idx 8
    // // Track info (only light id = 1)
    ana->CreateNtupleDColumn("TPCIni", holder->fLight.fTPCIni); // 9
    ana->CreateNtupleDColumn("TPCEnd", holder->fLight.fTPCEnd);
    ana->CreateNtupleDColumn("TPCDeltaE"); // 11

    // Silicon
    ana->CreateNtupleDColumn("SilIni", holder->fLight.fSilIni); // 12
    ana->CreateNtupleDColumn("SilEnd", holder->fLight.fSilEnd); // 13
    ana->CreateNtupleDColumn("SilDeltaE");                      // 14
    ana->CreateNtupleSColumn("SilLayer");                       // 15
    ana->CreateNtupleIColumn("SilIdx");                         // 16

    // Particle name
    ana->CreateNtupleSColumn("PartName"); // 17

    ana->FinishNtuple();
}

void ActGeant::RunAction::BeginOfRunAction(const G4Run* run)
{
    auto* ana = G4AnalysisManager::Instance();
    G4String file {"./Outputs/simu.root"};
    // auto* holder {DataHolder::Instance()};
    // file += "simu_" + holder->fReacInfo.fBeam + "_ " + holder->fReacInfo.fTarget + "_" + holder->fReacInfo.fLight +
    //         "_ex_" + TString::Format("%.2f", holder->fReacInfo.fEx).Data() + ".root";
    ana->OpenFile(file);
}

void ActGeant::RunAction::EndOfRunAction(const G4Run* run)
{
    // Write analys to file
    auto* ana = G4AnalysisManager::Instance();
    ana->Write();
    ana->CloseFile();
}
