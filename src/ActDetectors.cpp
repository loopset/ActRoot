#include "ActDetectors.h"

#include <iostream>
#include <stdexcept>
ChamberDetector::ChamberDetector(ChamberMode mode, int rebinFactorZ)
{
    //NBins in Z according to rebinFactor
    fREBINZ = rebinFactorZ;
    fNBINSZ = fNPADSZ / fREBINZ;
    //pad plane according to mode
    if(mode == ChamberMode::kACTAR_TPC)
    {
        fNPADSX = 128;
        fNPADSY = 128;
    }
    else if(mode == ChamberMode::kProtoACTAR_TPC)
    {
        fNPADSX = 64;
        fNPADSY = 32;
    }
}

void ChamberDetector::Print() const
{
    std::cout<<"== Chamber set up =="<<'\n';
    std::cout<<" npadsX = "<<fNPADSX<<", npadsY = "<<fNPADSY<<'\n';
    std::cout<<" npadsZ = "<<fNPADSZ<<" with rebinning of "<<fREBINZ<<" and thus "<<fNBINSZ<<" bins"<<'\n';
    std::cout<<"===================="<<'\n';
}

SiliconUnit::SiliconUnit(const XYZPoint& centre, double width, double height)
    : fCentre(centre), fWidth(width), fHeight(height)
{
}

SiliconLayer::SiliconLayer(SiliconMode mode, double offsetInPads)
    : fMode(mode)
{
    //auto determination of silicon plane normal vector!
    if(fMode == SiliconMode::kLeft || fMode == SiliconMode::kRight)
        fNormalVector = {0.0, 1.0, 0.0};
    else if(fMode == SiliconMode::kFront)
        fNormalVector = {1.0, 0.0, 0.0};
    else
        throw std::runtime_error("Wrong SiliconMode passed to constructor! See enum class SiliconModes");
}
