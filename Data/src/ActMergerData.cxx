#include "ActMergerData.h"

#include "ActColors.h"

#include <iostream>
#include <ostream>

bool ActRoot::BinaryData::IsFilled() const
{
    return fEs.size() > 0;
}

bool ActRoot::BinaryData::HasSP() const
{
    return fSP.X() != -1;
}

void ActRoot::BinaryData::Print(int n) const
{
    if(IsFilled())
    {
        std::cout << std::string(n, ' ') << "SP     : " << fSP << '\n';
        std::cout << std::string(n, ' ') << "TL     : " << fTL << '\n';
        std::cout << std::string(n, ' ') << "Qtotal : " << fQtotal << '\n';
        std::cout << std::string(n, ' ') << "Qave   : " << fQave << '\n';
        std::cout << std::string(n, ' ') << "Sil    :" << '\n';
        for(int m = 0; m < fLayers.size(); m++)
            std::cout << std::string(n, ' ') << fLayers[m] << " P : " << fNs[m] << " E : " << fEs[m] << '\n';
    }
    else
    {
        std::cout << std::string(n, ' ') << "is empty!" << '\n';
    }
}

void ActRoot::MergerData::Clear()
{
    // Reset but still keep run, entry number and flag
    auto run {fRun};
    auto entry {fEntry};
    auto flag {fFlag};
    *this = MergerData {};
    fRun = run;
    fEntry = entry;
    fFlag = flag;
}

void ActRoot::MergerData::Print() const
{
    std::cout << BOLDGREEN << ":::: MergerData ::::" << '\n';
    std::cout << "-> Points : " << '\n';
    std::cout << "   WP     : " << fWP << '\n';
    std::cout << "   RP     : " << fRP << '\n';
    std::cout << "   BSP    : " << fBSP << '\n';
    std::cout << "   BraggP : " << fBraggP << '\n';
    std::cout << RESET << GREEN;
    std::cout << "-> Legacy : " << '\n';
    std::cout << "   SP   : " << fSP << '\n';
    std::cout << "   Sils : " << '\n';
    for(int i = 0; i < fSilLayers.size(); i++)
        std::cout << "   layer : " << fSilLayers[i] << " idx : " << fSilNs[i] << " E : " << fSilEs[i] << " MeV" << '\n';
    std::cout << BOLDCYAN;
    std::cout << "-> Beam   : " << '\n';
    std::cout << "   Theta  : " << fThetaBeam << '\n';
    std::cout << "   ThetaZ : " << fThetaBeamZ << '\n';
    std::cout << "   PhiY   : " << fPhiBeamY << '\n';
    std::cout << BOLDYELLOW;
    std::cout << "-> Light      :" << '\n';
    std::cout << "   Theta      : " << fThetaLight << '\n';
    std::cout << "   ThetaDebug : " << fThetaDebug << '\n';
    std::cout << "   Phi        : " << fPhiLight << '\n';
    fLight.Print();
    std::cout << BOLDGREEN;
    std::cout << "-> Heavy : " << '\n';
    std::cout << "   Theta : " << fThetaHeavy << '\n';
    std::cout << "   Phi   : " << fPhiHeavy << '\n';
    fHeavy.Print();
    std::cout << "-> Flag  : " << fFlag << '\n';
    std::cout << "::::::::::::::::::::" << RESET << '\n';
}

void ActRoot::MergerData::Stream(std::ostream& streamer) const
{
    streamer << fRun << " " << fEntry << '\n';
}
