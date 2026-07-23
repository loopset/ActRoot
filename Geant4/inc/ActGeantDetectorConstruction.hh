#ifndef ACTGEANTDETECTORCONSTRUCTION_H
#define ACTGEANTDETECTORCONSTRUCTION_H

#include "G4VUserDetectorConstruction.hh"

#include <vector>

class G4LogicalVolume;
class G4VPhysicalVolume;

namespace ActGeant
{
class DetectorConstruction : public G4VUserDetectorConstruction
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

    // Logical volumes per Si side
    std::vector<G4LogicalVolume*> fSilLVs {};

public:
    DetectorConstruction() = default;
    ~DetectorConstruction() override = default;

    G4VPhysicalVolume* Construct() override;
    void ConstructSDandField() override;

    // Getters
    G4LogicalVolume* GetChamberLV() const { return fChamberLV; }
    G4VPhysicalVolume* GetDriftPV() const { return fDriftPV; }
    G4LogicalVolume* GetDriftLV() const { return fDriftLV; }

private:
    void DefineMaterials();
    G4VPhysicalVolume* DefineVolumes();
    void ParseGas();
};
} // namespace ActGeant
#endif // ACTGEANTDETECTORCONSTRUCTION_H
