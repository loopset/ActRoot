#include "ActMergerData.h"

#include "ActColors.h"

#include <iostream>
#include <ostream>

void ActRoot::BinaryData::Print() const
{
    std::cout << BOLDYELLOW << "-- BinaryData --" << '\n';
    std::cout << " SP     : " << fSP << '\n';
    std::cout << " TL     : " << fTL << '\n';
    std::cout << " Qtotal : " << fQtotal << '\n';
    std::cout << " Qave   : " << fQave << '\n';
    std::cout << " Sil    :" << '\n';
    for(int m = 0; m < fLayers.size(); m++)
        std::cout << "    " << fLayers[m] << " P : " << fNs[m] << " E : " << fEs[m] << '\n';
    std::cout << "--------------------" << RESET << '\n';
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
    std::cout << "-> WP     : " << fWP << '\n';
    std::cout << "-> RP     : " << fRP << '\n';
    std::cout << "-> SP     : " << fSP << '\n';
    std::cout << "-> BSP    : " << fBSP << '\n';
    std::cout << "-> BraggP : " << fBraggP << '\n';
    std::cout << "-> Silicons : " << '\n';
    for(int i = 0; i < fSilLayers.size(); i++)
        std::cout << "   layer : " << fSilLayers[i] << " idx : " << fSilNs[i] << " E : " << fSilEs[i] << " MeV" << '\n';
    std::cout << "-> ThetaBeam   : " << fThetaBeam << '\n';
    std::cout << "-> ThetaBeamZ  : " << fThetaBeamZ << '\n';
    std::cout << "-> PhiBeamY    : " << fPhiBeamY << '\n';
    std::cout << "-> ThetaLight  : " << fThetaLight << '\n';
    std::cout << "-> ThetaDebug  : " << fThetaDebug << '\n';
    std::cout << "-> ThetaLegacy : " << fThetaLegacy << '\n';
    std::cout << "-> ThetaHeavy  : " << fThetaHeavy << '\n';
    std::cout << "-> Phi         : " << fPhiLight << '\n';
    std::cout << "-> Qave        : " << fQave << '\n';
    std::cout << "-> TL          : " << fTrackLength << '\n';
    std::cout << "-> Flag        : " << fFlag << '\n';
    std::cout << "::::::::::::::::::::" << RESET << '\n';
}

void ActRoot::MergerData::Stream(std::ostream& streamer) const
{
    streamer << fRun << " " << fEntry << '\n';
}
