#include "ActGeantUtility.hh"

#include "ActInputParser.h"
#include "ActKinematics.h"
#include "ActOptions.h"

#include "TString.h"

#include "G4Material.hh"
#include "G4ParticleDefinition.hh"

#include <G4EmCalculator.hh>
#include <G4Exception.hh>
#include <G4ExceptionSeverity.hh>
#include <G4IonTable.hh>
#include <G4ParticleTable.hh>
#include <G4String.hh>
#include <G4SystemOfUnits.hh>
#include <G4Types.hh>
#include <G4ios.hh>

#include <CLHEP/Units/SystemOfUnits.h>
#include <fstream>
#include <vector>

void ActGeant::WriteDEDXTable(G4ParticleDefinition* p, G4Material* m, const G4String& file)
{
    std::ofstream streamer {file};
    if(!streamer)
        G4Exception("WriteDEDXTable", "WriteDEDX001", FatalException, "Cannot open DEDX file");

    G4cout << "Write DEDX for " << p->GetParticleName() << " in " << m->GetName() << " to file " << file << G4endl;

    G4EmCalculator calc {};
    G4double Emin {1 * keV};
    G4double Emax {1000 * MeV};
    G4double step {10 * keV};
    G4double factor {1.01}; // from IA... this sets the number of points per decade
    for(auto E = Emin; E <= Emax; E *= factor)
    {
        auto dedx {calc.ComputeTotalDEDX(E, p, m)};
        auto r {calc.GetRange(E, p, m)};
        streamer << E / MeV << "  " << dedx / (MeV / micrometer) << " " << r / mm << '\n';
    }
    streamer.close();
}

void ActGeant::WriteDEDXTables(const G4String& dir)
{
    auto config {ActRoot::Options::GetInstance()->GetConfigDir() + "geant.conf"};
    ActRoot::InputParser parser {config};
    auto block {parser.GetBlock("Kinematics")};
    auto reac {block->GetStringVector("Reaction")};
    auto aux {reac[0] + "," + reac[1]};
    // Init kinematics
    ActPhysics::Kinematics kin {aux};
    std::vector<G4ParticleDefinition*> parts;
    for(int i = 1; i <= 4; i++)
    {
        auto Z {kin.GetParticle(i).GetZ()};
        auto A {kin.GetParticle(i).GetA()};
        auto* part {G4IonTable::GetIonTable()->FindIon(Z, A)};
        parts.push_back(part);
    }

    // Materials are hardcoded so far
    std::vector<G4Material*> mats {G4Material::GetMaterial("GasMixture"), G4Material::GetMaterial("G4_Si")};

    // Do
    for(auto& part : parts)
    {
        auto pname {part->GetParticleName()};
        for(const auto& mat : mats)
        {
            auto mname {mat->GetName()};
            auto file {dir + "/table_" + pname + "_" + mname + ".txt"};
            WriteDEDXTable(part, mat, file);
        }
    }
}

G4String ActGeant::GetOutputFilename()
{
    auto config {ActRoot::Options::GetInstance()->GetConfigDir() + "geant.conf"};
    ActRoot::InputParser parser {config};
    auto block {parser.GetBlock("Kinematics")};
    auto reac {block->GetStringVector("Reaction")};
    auto aux {reac[0] + "," + reac[1]};
    // Init kinematics
    ActPhysics::Kinematics kin {aux};

    return TString::Format("simu_%s_%s_%s_ebeam_%.2f_ex_%.2f.root", kin.GetParticle(1).GetName().c_str(),
                           kin.GetParticle(2).GetName().c_str(), kin.GetParticle(3).GetName().c_str(), kin.GetT1Lab(),
                           kin.GetEx())
        .Data();
}