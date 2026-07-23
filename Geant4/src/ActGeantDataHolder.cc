#include "ActGeantDataHolder.hh"

#include <G4Types.hh>
#include <G4ios.hh>

G4ThreadLocal ActGeant::DataHolder* ActGeant::DataHolder::fInstance = nullptr;

ActGeant::DataHolder* ActGeant::DataHolder::Instance()
{
    if(!fInstance)
        fInstance = new DataHolder;
    return fInstance;
}

void ActGeant::DataHolder::Clear()
{
    fVertexInfo = {};
    fLight = {};
    fHeavy = {};
}

ActGeant::Vector3DPoint ActGeant::DataHolder::PointToVector(const G4ThreeVector& vec)
{
    return {vec.x(), vec.y(), vec.z()};
}
