#include "ActGeantData.hh"

#include "ActColors.h"

#include <G4ios.hh>

void ActGeant::ReactionInfo::Print() const
{
    G4cout << BOLDCYAN << " ---- ReactionInfo ----" << G4endl;
    G4cout << "  Beam : " << fBeam << G4endl;
    G4cout << "  Target : " << fTarget << G4endl;
    G4cout << "  Light : " << fLight << G4endl;
    G4cout << "  Heavy : " << fHeavy << G4endl;
    G4cout << "  EBeam : " << fEBeam << G4endl;
    G4cout << "  Ex : " << fEx << RESET << G4endl;
}

void ActGeant::VertexInfo::Print() const
{
    G4cout << BOLDYELLOW << "···· VertexInfo ····" << G4endl;
    G4cout << "  WP     : (" << fWP[0] << "," << fWP[1] << "," << fWP[2] << ")" << G4endl;
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
    G4cout << "  Name  : " << fName << G4endl;
    G4cout << "  (Z,A) : (" << fZ << ", " << fA << ")" << G4endl;
    G4cout << "  TPC" << G4endl;
    G4cout << "    dE  : " << fTPCDeltaE << G4endl;
    if(fTPCIni.size())
        G4cout << "    Ini : (" << fTPCIni[0] << ", " << fTPCIni[1] << ", " << fTPCIni[2] << ")" << G4endl;
    if(fTPCEnd.size())
        G4cout << "    End : (" << fTPCEnd[0] << ", " << fTPCEnd[1] << ", " << fTPCEnd[2] << ")" << G4endl;
    G4cout << "  Si layer 0" << G4endl;
    G4cout << "    Layer : " << fSilLayer0 << G4endl;
    G4cout << "    Index : " << fSilIdx0 << G4endl;
    G4cout << "    dE    : " << fSilDeltaE0 << G4endl;
    G4cout << "    EAfter: " << fSilEAfter0 << G4endl;
    if(fSilIni0.size())
        G4cout << "    Ini   : (" << fSilIni0[0] << ", " << fSilIni0[1] << ", " << fSilIni0[2] << ")" << G4endl;
    if(fSilEnd0.size())
        G4cout << "    End   : (" << fSilEnd0[0] << ", " << fSilEnd0[1] << ", " << fSilEnd0[2] << ")" << G4endl;
    G4cout << "  Si layer 1" << G4endl;
    G4cout << "    Layer : " << fSilLayer1 << G4endl;
    G4cout << "    Index : " << fSilIdx1 << G4endl;
    G4cout << "    dE    : " << fSilDeltaE1 << G4endl;
    G4cout << "    EAfter: " << fSilEAfter1 << G4endl;
    if(fSilIni1.size())
        G4cout << "    Ini   : (" << fSilIni1[0] << ", " << fSilIni1[1] << ", " << fSilIni1[2] << ")" << G4endl;
    if(fSilEnd1.size())
        G4cout << "    End   : (" << fSilEnd1[0] << ", " << fSilEnd1[1] << ", " << fSilEnd1[2] << ")" << RESET
               << G4endl;
}
