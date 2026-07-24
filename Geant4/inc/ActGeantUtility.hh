#ifndef ActGeantUtility_hh
#define ActGeantUtility_hh

#include <G4String.hh>
// forward declarations
class G4ParticleDefinition;
class G4Material;

namespace ActGeant
{
void WriteDEDXTable(G4ParticleDefinition* p, G4Material* m, const G4String& file);

void WriteDEDXTables(const G4String& dir);
} // namespace ActGeant

#endif