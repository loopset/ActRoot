#include "ActGeantData.hh"

#include "ActColors.h"

#include <G4ios.hh>

void ActGeant::VertexInfo::Print() const
{
    G4cout << BOLDYELLOW << "···· Vertex Info ····" << G4endl;
    G4cout << "  RP     : (" << fRP[0] << "," << fRP[1] << "," << fRP[2] << ")" << G4endl;
    G4cout << "  EBeam  : " << fEBeam << G4endl;
    G4cout << "  T3     : " << fT3 << G4endl;
    G4cout << "  theta3 : " << ftheta3 << G4endl;
    G4cout << "  phi3   : " << fphi3 << G4endl;
    G4cout << "  T4     : " << fT4 << G4endl;
    G4cout << "  theta4 : " << ftheta4 << G4endl;
    G4cout << "  phi4   : " << fphi4 << G4endl;
    G4cout << "  thetaCM: " << fthetaCM << RESET << G4endl;
}


void ActGeant::TrackInfo::Print() const
{
    G4cout << BOLDGREEN << "···· TrackInfo ····" << G4endl;
    G4cout << "  (Z,A) : (" << fZ << ", " << fA << ")" << G4endl;
    G4cout << "  TPC" << G4endl;
    G4cout << "    dE  : " << fTPCDeltaE << G4endl;
    G4cout << "    Ini : (" << fTPCIni[0] << ", " << fTPCIni[1] << ", " << fTPCIni[2] << ")" << G4endl;
    G4cout << "    End : (" << fTPCEnd[0] << ", " << fTPCEnd[1] << ", " << fTPCEnd[2] << ")" << G4endl;
    G4cout << "  Silicon" << G4endl;
    G4cout << "    Layer : " << fSilLayer << G4endl;
    G4cout << "    Index : " << fSilIdx << G4endl;
    G4cout << "    dE    : " << fSilDeltaE << G4endl;
    G4cout << "    Ini   : (" << fSilIni[0] << ", " << fSilIni[1] << ", " << fSilIni[2] << ")" << G4endl;
    G4cout << "    End   : (" << fSilEnd[0] << ", " << fSilEnd[1] << ", " << fSilEnd[2] << ")" << RESET << G4endl;
}
