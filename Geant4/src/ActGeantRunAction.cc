#include "ActGeantRunAction.hh"

#include "ActGeantDataHolder.hh"

#include "G4AnalysisManager.hh"
#include "G4Run.hh"
#include "G4RunManager.hh"

#include <G4Material.hh>
#include <G4MaterialTable.hh>
#include <G4String.hh>
#include <G4Threading.hh>
#include <G4ios.hh>

ActGeant::RunAction::RunAction(const G4String& outFile) : fOutFile(outFile)
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

    // Si layer 0
    ana->CreateNtupleDColumn("SilIni0", holder->fLight.fSilIni0); // 12
    ana->CreateNtupleDColumn("SilEnd0", holder->fLight.fSilEnd0); // 13
    ana->CreateNtupleDColumn("SilDeltaE0");                       // 14
    ana->CreateNtupleDColumn("SilEAfter0");                       // 15
    ana->CreateNtupleSColumn("SilLayer0");                        // 16
    ana->CreateNtupleIColumn("SilIdx0");                          // 17
    // Si layer 1
    ana->CreateNtupleDColumn("SilIni1", holder->fLight.fSilIni1); // 18
    ana->CreateNtupleDColumn("SilEnd1", holder->fLight.fSilEnd1); // 19
    ana->CreateNtupleDColumn("SilDeltaE1");                       // 20
    ana->CreateNtupleDColumn("SilEAfter1");                       // 21
    ana->CreateNtupleSColumn("SilLayer1");                        // 22
    ana->CreateNtupleIColumn("SilIdx1");                          // 23

    // Particle name
    ana->CreateNtupleSColumn("PartName"); // 24
    // others that i forgot to include before
    ana->CreateNtupleDColumn("WP", holder->fVertexInfo.fWP);

    ana->FinishNtuple();
}

void ActGeant::RunAction::BeginOfRunAction(const G4Run* run)
{
    auto* ana = G4AnalysisManager::Instance();
    G4String file {"./Outputs/" + fOutFile};
    ana->OpenFile(file);
}

void ActGeant::RunAction::EndOfRunAction(const G4Run* run)
{
    // Write analys to file
    auto* ana = G4AnalysisManager::Instance();
    ana->Write();
    ana->CloseFile();
}
