#include "ActGeantSimpleSD.hh"

#include "ActGeantDataHolder.hh"

#include <G4Electron.hh>
#include <G4HCofThisEvent.hh>
#include <G4Step.hh>
#include <G4String.hh>
#include <G4TouchableHistory.hh>
#include <G4Types.hh>
#include <G4VSensitiveDetector.hh>
#include <G4ios.hh>

ActGeant::SimpleSD::SimpleSD(const G4String& name) : G4VSensitiveDetector(name)
{
    fIsSil = G4StrUtil::contains(name, "sil");
}

void ActGeant::SimpleSD::Initialize(G4HCofThisEvent*) {}

G4bool ActGeant::SimpleSD::ProcessHits(G4Step* step, G4TouchableHistory*)
{
    // Get track
    auto* track {step->GetTrack()};
    auto id {track->GetTrackID()};
    auto* partDef {track->GetParticleDefinition()};

    // Apply cuts: ignore neutral and secondary e-
    if(partDef->GetPDGCharge() == 0)
        return false;
    if(partDef == G4Electron::Definition())
        return false;

    // Write info
    auto& data {(id == 1) ? DataHolder::Instance()->fLight : DataHolder::Instance()->fHeavy};
    // TPC
    if(!fIsSil)
    {
        if(data.fIsIniTPC) // get initial position
        {
            data.fTrackID = id;
            data.fZ = partDef->GetAtomicNumber();
            data.fA = partDef->GetAtomicMass();
            data.fTPCIni = DataHolder::PointToVector(step->GetPreStepPoint()->GetPosition());
            data.fIsIniTPC = false;
        }
        data.fTPCDeltaE += step->GetTotalEnergyDeposit();
        data.fTPCEnd = DataHolder::PointToVector(step->GetPreStepPoint()->GetPosition());
    }
    else // is Sil
    {
        if(fIsSil)
        {
            data.fSilIni = DataHolder::PointToVector(step->GetPreStepPoint()->GetPosition());
            // Get Si layer and idx
            auto* phys {step->GetPreStepPoint()->GetTouchableHandle()->GetVolume()};
            data.fSilLayer = phys->GetName();
            data.fSilIdx = phys->GetCopyNo();
            data.fIsIniSil = false;
        }
        data.fSilDeltaE += step->GetTotalEnergyDeposit();
        data.fSilEnd = DataHolder::PointToVector(step->GetPreStepPoint()->GetPosition());
    }


    return true;
}

void ActGeant::SimpleSD::EndOfEvent(G4HCofThisEvent* hcol)
{
    return;
}
