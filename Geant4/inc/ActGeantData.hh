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
    Vector3DPoint fWP {};
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
    // Common particle info
    G4String fName {};
    G4int fZ {-1};
    G4int fA {-1};
    G4int fTrackID {-1};
    // TPC info
    G4double fTPCDeltaE {0};
    Vector3DPoint fTPCIni {};
    Vector3DPoint fTPCEnd {};
    G4bool fIsIniTPC {true};
    // Si layer 0
    G4double fSilDeltaE0 {0};
    G4double fSilEAfter0 {-1};
    Vector3DPoint fSilIni0 {};
    Vector3DPoint fSilEnd0 {};
    G4String fSilLayer0 {};
    G4int fSilIdx0 {-1};
    G4bool fIsIniSil0 {true};
    // Si layer 1
    G4double fSilDeltaE1 {0};
    G4double fSilEAfter1 {-1};
    Vector3DPoint fSilIni1 {};
    Vector3DPoint fSilEnd1 {};
    G4String fSilLayer1 {};
    G4int fSilIdx1 {-1};
    G4bool fIsIniSil1 {true};

    void Print() const;
};
} // namespace ActGeant


#endif // !ActGeantData_hh
