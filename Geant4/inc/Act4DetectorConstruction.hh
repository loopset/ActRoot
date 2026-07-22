#ifndef ACT4DETECTORCONSTRUCTION_H
#define ACT4DETECTORCONSTRUCTION_H

#include "G4VUserDetectorConstruction.hh"
#include "globals.hh"

class G4LogicalVolume;
class G4VPhysicalVolume;

class ActDetectorConstruction : public G4VUserDetectorConstruction
{
private:
    // Logical volumes for ACTAR
    G4LogicalVolume* fWorldLV {};
    G4LogicalVolume* fChamberLV {};
    G4LogicalVolume* fDriftLV {};
    // Physical volumes for ACTAR
    G4VPhysicalVolume* fWorldPV {};
    G4VPhysicalVolume* fChamberPV {};
    G4VPhysicalVolume* fDriftPV {};
    G4VPhysicalVolume* fPadPlanePV {};
    G4VPhysicalVolume* fSilPV {};

public:
    ActDetectorConstruction() = default;
    ~ActDetectorConstruction() override = default;

    G4VPhysicalVolume* Construct() override;

    // Getters
    G4LogicalVolume* GetChamberLV() const { return fChamberLV; }
    G4VPhysicalVolume* GetDriftPV() const { return fDriftPV; }
    G4LogicalVolume* GetDriftLV() const { return fDriftLV; }

private:
    void DefineMaterials();
    G4VPhysicalVolume* DefineVolumes();
    void ParseGas();
};

#endif // ACTDETECTORCONSTRUCTION_H
