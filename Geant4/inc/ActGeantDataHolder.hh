#ifndef Act4DataHolder_hh
#define Act4DataHolder_hh

#include "ActGeantData.hh"

#include <G4ThreeVector.hh>
#include <G4Types.hh>

namespace ActGeant
{
// Singleton to store data across all classes
class DataHolder
{
public:
    VertexInfo fVertexInfo {};
    TrackInfo fLight {};
    TrackInfo fHeavy {};

    // Basic function to access singleton
    static DataHolder* Instance();

    // Utility functions
    void Clear();
    // Some static transformations
    static Vector3DPoint PointToVector(const G4ThreeVector& vec);

private:
    DataHolder() = default;
    ~DataHolder() = default;
    DataHolder(const DataHolder&) = delete;
    DataHolder& operator=(const DataHolder&) = delete;

    static G4ThreadLocal DataHolder* fInstance;
};
} // namespace ActGeant

#endif
