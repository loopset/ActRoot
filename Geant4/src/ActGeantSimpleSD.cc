#include "ActGeantSimpleSD.hh"

#include "ActGeantDataHolder.hh"

#include <G4Electron.hh>
#include <G4HCofThisEvent.hh>
#include <G4Step.hh>
#include <G4String.hh>
#include <G4SystemOfUnits.hh>
#include <G4TouchableHistory.hh>
#include <G4Types.hh>
#include <G4VSensitiveDetector.hh>
#include <G4ios.hh>

ActGeant::SimpleSD::SimpleSD(const G4String& name) : G4VSensitiveDetector(name)
{
    fIsSil = G4StrUtil::contains(name, "sil");
}

void ActGeant::SimpleSD::Initialize(G4HCofThisEvent*)
{
    // Reset variables that track Si layers
    fCurrentLayer = "";
    fIsFirstLayer = true;
}

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
            data.fName = partDef->GetParticleName();
            data.fTPCIni = DataHolder::PointToVector(step->GetPreStepPoint()->GetPosition());
            data.fIsIniTPC = false;
        }
        data.fTPCDeltaE += step->GetTotalEnergyDeposit();
        data.fTPCEnd = DataHolder::PointToVector(step->GetPostStepPoint()->GetPosition());
    }
    // Silicons
    else
    {
        auto* phys {step->GetPreStepPoint()->GetTouchableHandle()->GetVolume()};
        auto layer {phys->GetName()};
        // Determine whether is first layer or not
        if(!fCurrentLayer.empty() && (layer != fCurrentLayer))
            fIsFirstLayer = false;

        if(fIsFirstLayer)
        {
            if(data.fIsIniSil0)
            {
                data.fSilIni0 = DataHolder::PointToVector(step->GetPreStepPoint()->GetPosition());
                data.fSilLayer0 = phys->GetName();
                data.fSilIdx0 = phys->GetCopyNo();
                data.fIsIniSil0 = false;
            }
            data.fSilDeltaE0 += step->GetTotalEnergyDeposit();
            data.fSilEnd0 = DataHolder::PointToVector(step->GetPostStepPoint()->GetPosition());
            data.fSilEAfter0 = step->GetPostStepPoint()->GetKineticEnergy();
        }
        else
        {
            if(data.fIsIniSil1)
            {
                data.fSilIni1 = DataHolder::PointToVector(step->GetPreStepPoint()->GetPosition());
                data.fSilLayer1 = phys->GetName();
                data.fSilIdx1 = phys->GetCopyNo();
                data.fIsIniSil1 = false;
            }
            data.fSilDeltaE1 += step->GetTotalEnergyDeposit();
            data.fSilEnd1 = DataHolder::PointToVector(step->GetPostStepPoint()->GetPosition());
            data.fSilEAfter1 = step->GetPostStepPoint()->GetKineticEnergy();
        }

        fCurrentLayer = layer;
    }

    return true;
}

void ActGeant::SimpleSD::EndOfEvent(G4HCofThisEvent* hcol) {}
