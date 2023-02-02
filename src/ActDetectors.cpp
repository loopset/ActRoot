#include "ActDetectors.h"
#include "ActRoot.h"

#include <algorithm>
#include <cctype>
#include <fstream>
#include <ios>
#include <iostream>
#include <limits>
#include <sstream>
#include <stdexcept>
#include <string>
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

void SiliconUnit::Print() const
{
    std::cout<<"== Silicon Unit =="<<'\n';
    std::cout<<" Centered at = "<<fCentre<<" mm"<<'\n';
    std::cout<<" Width = "<<fWidth<<" mm and height = "<<fHeight<<" mm"<<'\n';
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

void SiliconLayer::ReadFile(const std::string &fileName)
{
    std::ifstream streamer {fileName};
    if(!streamer)
        throw std::runtime_error("Error loading SiliconLayer file " + fileName);
    std::string line {};
    int row {};
    //ignore first line with column names
    streamer.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    while(std::getline(streamer, line, '\n'))
    {
        std::istringstream lineStreamer {line};
        std::string value {};
        int column {};
        int silIndex {};
        double width {}; double height {};
        double pos0 {}; double pos1 {};
        while(std::getline(lineStreamer, value, ' '))
        {
            //clean from whitespaces
            value.erase(std::remove_if(value.begin(), value.end(),
                                       [](unsigned char x){return std::isspace(x);}),
                        value.end());
            if(value.size() == 0)
                continue;
            if(column == 0)
                silIndex = std::stoi(value);
            if(column == 1)
                width = std::stod(value);
            if(column == 2)
                height = std::stod(value);
            if(column == 3)
                pos0 = std::stod(value);
            if(column == 4)
                pos1 = std::stod(value);
            column++;
        }
        SiliconUnit unit;
        if(fMode == SiliconMode::kFront)
            unit = SiliconUnit(XYZPoint(fOffsetInPads * ActRoot::GetChamber().fPadSide,
                                        pos0,
                                        pos1),
                               width,
                               height);
        else
            unit = SiliconUnit(XYZPoint(pos0,
                                        fOffsetInPads * ActRoot::GetChamber().fPadSide,
                                        pos1),
                               width,
                               height);
        //and add it!
        fPlacements[silIndex] = unit;
        row++;
    }
    streamer.close();
}

void SiliconDetector::AddLayer(SiliconMode mode, SiliconPanel panel, const SiliconLayer &layer)
{
    std::cout<<"Adding layer..."<<'\n';
    fSilicons[mode] = {panel, std::move(layer)};
    fModes.insert(mode);
}
