#include "ActGeantDetectorConstruction.hh"

#include "ActGeantSimpleSD.hh"
#include "ActInputParser.h"
#include "ActOptions.h"
#include "ActSilSpecs.h"

#include "G4Box.hh"
#include "G4Colour.hh"
#include "G4LogicalVolume.hh"
#include "G4Material.hh"
#include "G4NistManager.hh"
#include "G4PVPlacement.hh"
#include "G4PhysicalConstants.hh"
#include "G4SDManager.hh"
#include "G4SystemOfUnits.hh"
#include "G4ThreeVector.hh"
#include "G4Types.hh"
#include "G4VPhysicalVolume.hh"
#include "G4VisAttributes.hh"
#include "G4ios.hh"

#include <G4Color.hh>
#include <G4Element.hh>
#include <G4EventManager.hh>
#include <G4Isotope.hh>
#include <G4MaterialTable.hh>
#include <G4Transform3D.hh>

#include <CLHEP/Units/SystemOfUnits.h>
#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <iterator>
#include <string>
#include <vector>

// Construct method for geometry
G4VPhysicalVolume* ActGeant::DetectorConstruction::Construct()
{
    // Define materials
    DefineMaterials();

    // Build detector
    return DefineVolumes();
}

G4VPhysicalVolume* ActGeant::DetectorConstruction::DefineVolumes()
{
    /////////////// WORLD
    auto worldSizeX {1. * m};
    auto worldSizeY {1. * m};
    auto worldSizeZ {1. * m};
    auto worldBox = new G4Box("World", worldSizeX, worldSizeY, worldSizeZ);
    fWorldLV = new G4LogicalVolume(worldBox, G4Material::GetMaterial("Galactic"), "World");
    fWorldPV = new G4PVPlacement(0, G4ThreeVector(), fWorldLV, "World", 0, false, 0);
    fWorldLV->SetVisAttributes(G4VisAttributes::GetInvisible());

    /////////////// ACTAR TPC
    // Chamber
    auto chamberSizeX = 606. / 2 * mm;
    auto chamberSizeY = 606. / 2 * mm;
    auto chamberSizeZ = 335. / 2 * mm;
    // at center of world
    auto chamberCenterX = 0. * m;
    auto chamberCenterY = 0. * m;
    auto chamberCenterZ = 0. * m;
    auto* chamberS = new G4Box("Chamber", chamberSizeX, chamberSizeY, chamberSizeZ);
    fChamberLV = new G4LogicalVolume(chamberS, G4Material::GetMaterial("GasMixture"), "Chamber");
    fChamberPV = new G4PVPlacement(0, G4ThreeVector(chamberCenterX, chamberCenterY, chamberCenterZ), fChamberLV,
                                   "Chamber", fWorldLV, false, 0);

    // draw mechanical chamber
    auto chamberVisAtt = new G4VisAttributes(G4Colour(0.5, 0.5, 0.5)); // gray and alpha=0.25
    chamberVisAtt->SetVisibility(true);
    fChamberLV->SetVisAttributes(chamberVisAtt);

    // Active volume (drift region)
    // So far only CUBIC PAD PLANE
    auto driftSizeX = 295. / 2 * mm;
    auto driftSizeY = 295. / 2 * mm;
    auto driftSizeZ = 255. / 2 * mm;

    // centered at center of main volume
    auto driftCenterX = 0. * mm;
    auto driftCenterY = 0. * mm;
    auto driftCenterZ = 0. * mm; // drift region is not perfectlly centren (-15.5 mm offset) but I THINK does not affect
                                 // our simple calculations

    auto* driftS = new G4Box("driftBox", driftSizeX, driftSizeY, driftSizeZ);
    fDriftLV = new G4LogicalVolume(driftS, G4Material::GetMaterial("GasMixture"), "driftLog");
    fDriftPV = new G4PVPlacement(0, G4ThreeVector(driftCenterX, driftCenterY, driftCenterZ), fDriftLV, "driftPhys",
                                 fChamberLV, //<- Mother volume is chamber
                                 false, 0);

    // do not set sentitive detector by now

    // visualization attributes
    auto* driftVisAtt = new G4VisAttributes(G4Colour(0.1176, 0.5674, 1.0));
    driftVisAtt->SetVisibility(true);
    fDriftLV->SetVisAttributes(driftVisAtt);

    // Pad plane
    auto padPlaneX {128. * mm};
    auto padPlaneY {128. * mm};
    auto padPlaneZ {4.54 / 2 * mm};

    auto* padPlaneS {new G4Box("PadPlane", padPlaneX, padPlaneY, padPlaneZ)};
    auto* padPlaneLV =
        new G4LogicalVolume(padPlaneS, G4Material::GetMaterial("Galactic"), "PadPlane"); // not the right material but ...
    //--place it above chamber
    auto padPlanePosX {0. * cm};
    auto padPlanePosY {0. * cm};
    auto padPlanePosZ {-driftSizeZ + driftCenterZ}; // Pad plane glued to drift chamber (not realistic I think)
    fPadPlanePV = new G4PVPlacement(0, G4ThreeVector(padPlanePosX, padPlanePosY, padPlanePosZ), padPlaneLV, "PadPlane",
                                    fChamberLV, false, 0);

    // visualization attributes
    auto* padPlaneVisAtt {new G4VisAttributes(G4Color(1., 0., 1.))};
    padPlaneVisAtt->SetVisibility(true);
    padPlaneLV->SetVisAttributes(padPlaneVisAtt);


    ////////////////////  Silicon detectors
    auto configs {ActRoot::Options::GetInstance()->GetGeantFile()};
    ActRoot::InputParser parser {configs};
    auto sils {parser.GetBlock("Silicons")};
    auto file {sils->GetString("Specs")};
    // Path to specs file
    ActPhysics::SilSpecs specs {};
    specs.ReadFile(file);
    // Offset wrt to beam window of CENTRAL SILICONS
    // Here centre is automatically computed as meanZ below, getting min and max Z values for each layer
    auto layers {sils->GetStringVector("Layers")};
    auto offsets {sils->GetDoubleVector("Offsets")};

    // Beam centred at
    double distPadPlaneBeam {110. * mm};
    double beamCentre {padPlanePosZ + distPadPlaneBeam};
    // Unless specified, Z center of layer is aligned with this position

    // For each layer create a logic volume
    for(auto& [name, layer] : specs.GetLayers())
    {
        // Exclude layers not listed manually with offset
        auto it {std::find(layers.begin(), layers.end(), name)};
        if(it == layers.end())
            continue;
        auto iit {std::distance(layers.begin(), it)};
        // Get offset from geant.conf file
        auto offset {offsets[iit]};
        // Si unit
        auto& unit {layer.GetUnit()};
        // Logic volume depending on side
        G4Box* silBox {};
        G4LogicalVolume* silLV {};
        // Lateral
        if(auto side {layer.GetSilSide()}; side == ActPhysics::SilSide::ELeft || side == ActPhysics::SilSide::ERight)
        {
            silBox = new G4Box {"Si_" + name, 0.5 * unit.GetWidth() * mm, 0.5 * unit.GetThickness() * mm,
                                0.5 * unit.GetHeight() * mm};
            silLV = new G4LogicalVolume {
                silBox, G4NistManager::Instance()->FindOrBuildMaterial("G4_Si"), "logicSil_" + name, 0, 0, 0};
        }
        else
        {
            silBox = new G4Box {"Si_" + name, 0.5 * unit.GetThickness() * mm, 0.5 * unit.GetWidth() * mm,
                                0.5 * unit.GetHeight() * mm};
            silLV = new G4LogicalVolume {
                silBox, G4NistManager::Instance()->FindOrBuildMaterial("G4_Si"), "logicSil_" + name, 0, 0, 0};
        }
        fSilLVs.push_back(silLV);
        // Visualisation attributes
        auto* silVisAtt {new G4VisAttributes {G4Color {1.0, 0.6471, 0.0}}};
        silVisAtt->SetVisibility(true);
        silLV->SetVisAttributes(silVisAtt);

        // Get mean Z, which is wrt to pad plane
        double minZ {1111};
        double maxZ {-1111};
        for(const auto& [_, pair] : layer.GetPlacements())
        {
            if(pair.second < minZ)
                minZ = pair.second;
            if(pair.second > maxZ)
                maxZ = pair.second;
        }
        auto meanZ {(maxZ + minZ) / 2 * mm};
        auto silCentre {padPlanePosZ + meanZ};
        // Calculate correction
        auto correct {silCentre - beamCentre};

        // And now add the physical placement of the different units
        for(const auto& [idx, pair] : layer.GetPlacements())
        {
            // Z common to both positionings
            auto zFromPadPlane {pair.second * mm};
            auto z {padPlanePosZ + zFromPadPlane - correct + offset};
            // XY depending on side
            double x {};
            double y {};
            // Lateral
            if(auto side {layer.GetSilSide()};
               side == ActPhysics::SilSide::ELeft || side == ActPhysics::SilSide::ERight)
            {
                // X and Y referred wrt to chamber centre, of course
                y = layer.GetPoint().Y() * mm - padPlaneY;
                x = pair.first * mm - padPlaneX;
            }
            // Front
            if(auto side {layer.GetSilSide()}; side == ActPhysics::SilSide::EFront)
            {
                x = layer.GetPoint().X() * mm - padPlaneX;
                y = pair.first * mm - padPlaneY;
            }
            // Position vector
            auto pos {G4ThreeVector {x, y, z}};
            // std::cout << "Layer: " << name << " idx: " << idx << " pos: " << pos << '\n';
            // And placement (no need to store ptr)
            new G4PVPlacement {nullptr, pos, silLV, name, fChamberLV, false, idx};
        }
    }
    // std::exit(1);

    // // Testing Si detector
    // auto* silBox = new G4Box {"SiPad", 100 * mm, 5 * mm, 100 * mm};
    // auto* silLV =
    //     new G4LogicalVolume {silBox, G4NistManager::Instance()->FindOrBuildMaterial("G4_Si"), "logicSi", 0, 0, 0};
    // fSilLVs.push_back(silLV);
    // auto pos {G4ThreeVector {0, +50 * cm, 0}};
    // new G4PVPlacement {nullptr, pos, silLV, "SiPV", fWorldLV, false, 0};

    // always return the physical world
    return fWorldPV;
}

void ActGeant::DetectorConstruction::DefineMaterials()
{
    // NIST database
    G4NistManager* nist = G4NistManager::Instance();
    auto* N = nist->FindOrBuildElement("N");
    auto* O = nist->FindOrBuildElement("O");

    // galactic material
    G4double z, a, gal_rho, gal_p, gal_t;
    auto* Galactic = new G4Material("Galactic", z = 1., a = 1.01 * g / mole, gal_rho = universe_mean_density, kStateGas,
                                    gal_t = 2.73 * kelvin, gal_p = 3.e-18 * pascal);

    // air
    auto* Air = nist->FindOrBuildMaterial("G4_AIR");
    if(!Air)
        G4cout << "Null G4_AIR" << G4endl;
    // mylar
    auto* Mylar = nist->FindOrBuildMaterial("G4_MYLAR");
    if(!Mylar)
        G4cout << "Null G4_MYLAT" << G4endl;
    // Al
    auto* Aluminium {nist->FindOrBuildMaterial("G4_Al")};
    if(!Aluminium)
        G4cout << "Null G4_Al" << G4endl;

    // Define basic elements
    auto* isoD {new G4Isotope {"D", 1, 2, 2.01410178 * g / mole}};
    auto* elD {new G4Element {"D", "D", 1}};
    elD->AddIsotope(isoD, 1);

    // And parse config file for gas parameters
    ParseGas();

    // G4cout << *(G4Material::GetMaterialTable()) << '\n';
}

void ActGeant::DetectorConstruction::ParseGas()
{
    // Retrieve file path. So far name is hardcoded
    auto file {ActRoot::Options::GetInstance()->GetGeantFile()};
    ActRoot::InputParser parser {file};

    // Get [Gas] block
    auto gas {parser.GetBlock("Gas")};
    // Gas components
    auto comps {gas->GetStringVector("Components")};
    // VOLUME = MOLE fractions
    auto fracs {gas->GetDoubleVector("Fractions")};
    // Pressure
    auto p {gas->GetDouble("Pressure")}; // in mbar
    p *= 1e-3 * bar;

    // Gas constant
    double R {8.314 * joule / (mole * kelvin)};
    // Temperature (20C assumed)
    double t {293.15 * kelvin};

    // Pointer to NIST
    auto* nist {G4NistManager::Instance()};

    // Build materials of mixture
    std::vector<G4Material*> mats {};
    std::vector<double> molarMasses {};
    for(const auto& comp : comps)
    {
        G4Material* mat {};
        double molarMass {};
        double density {};
        if(comp == "D2")
        {
            molarMass = 2. * 2.0140 * g / mole;
            density = (p * molarMass) / (R * t);
            mat = new G4Material(comp, density, 1, kStateGas, t, p);
            mat->AddElement(G4Element::GetElement("D"), 2);
        }
        if(comp == "iC4H10")
        {
            molarMass = (4. * 12.0107 + 10. * 1.00794) * g / mole;
            density = (p * molarMass) / (R * t);
            mat = new G4Material {"iC4H10", density, 2, kStateGas, t, p};
            mat->AddElement(nist->FindOrBuildElement("C"), 4);
            mat->AddElement(nist->FindOrBuildElement("H"), 10);
        }
        std::cout << "Comp : " << comp << " molar mass: " << molarMass / (g / mole) << " rho: " << density / (g / cm3)
                  << '\n';
        mats.push_back(mat);
        molarMasses.push_back(molarMass);
    }
    // Compute total mass of mixture
    double massMix {};
    for(int i = 0; i < molarMasses.size(); i++)
        massMix += static_cast<double>(fracs[i]) / 100 * molarMasses[i];
    // And mass fractions
    std::vector<double> massFracs {};
    for(int i = 0; i < molarMasses.size(); i++)
        massFracs.push_back((static_cast<double>(fracs[i]) / 100 * molarMasses[i]) / massMix);

    // And density
    auto rhoMix {(p * massMix) / (R * t)};

    std::cout << "Rho of mixture : " << rhoMix / (g / cm3) << " g/cm3" << '\n';
    auto* mixture {new G4Material {"GasMixture", rhoMix, static_cast<G4int>(mats.size()), kStateGas, t, p}};
    for(int i = 0; i < mats.size(); i++)
    {
        const auto& mat {mats[i]};
        mixture->AddMaterial(mat, massFracs[i]);
    }
}

void ActGeant::DetectorConstruction::ConstructSDandField()
{
    // Here define the SDs
    // Both TPC and Si share the same SD altough in different pointers (different instances)

    auto* tpcSD {new SimpleSD {"tpcSD"}};
    G4SDManager::GetSDMpointer()->AddNewDetector(tpcSD);
    SetSensitiveDetector(fDriftLV, tpcSD);

    auto* silSD {new SimpleSD {"silSD"}};
    G4SDManager::GetSDMpointer()->AddNewDetector(silSD);
    for(auto& silLV : fSilLVs)
        SetSensitiveDetector(silLV, silSD);
}
