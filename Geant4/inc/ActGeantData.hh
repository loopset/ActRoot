#ifndef ActGeantData_hh
#define ActGeantData_hh

#include <G4String.hh>
#include <G4Types.hh>

#include <vector>

namespace ActGeant
{
using Vector3DPoint = std::vector<G4double>;

class ReactionInfo
{
public:
    G4String fBeam {};
    G4String fTarget {};
    G4String fLight {};
    G4String fHeavy {};
    G4double fEBeam {};
    G4double fEx {};

    void Print() const;
};

class VertexInfo
{
public:
    Vector3DPoint fRP {};
    G4double fEBeam {};
    G4double fT3 {};
    G4double ftheta3 {};
    G4double fphi3 {};
    G4double fT4 {};
    G4double ftheta4 {};
    G4double fphi4 {};
    G4double fthetaCM {};

    void Print() const;
};

class TrackInfo
{
public:
    Vector3DPoint fTPCIni {};
    Vector3DPoint fTPCEnd {};
    Vector3DPoint fSilIni {};
    Vector3DPoint fSilEnd {};
    G4double fTPCDeltaE {0};
    G4double fSilDeltaE {0};
    G4String fSilLayer {};
    G4int fSilIdx {-1};
    G4String fName {};
    G4int fZ {-1};
    G4int fA {-1};
    G4int fTrackID {-1};
    G4bool fIsIniTPC {true};
    G4bool fIsIniSil {true};
    void Print() const;
};
} // namespace ActGeant


#endif // !ActGeantData_hh
