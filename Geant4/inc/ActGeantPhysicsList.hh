#ifndef ActGeantPhysicsList_hh
#define ActGeantPhysicsList_hh

#include "G4VModularPhysicsList.hh"
namespace ActGeant
{
class PhysicsList : public G4VModularPhysicsList
{
public:
    PhysicsList();
    virtual ~PhysicsList() override = default;

    virtual void ConstructParticle() override;
};
} // namespace ActGeant
#endif // !ActGeantPhysicsList_hh