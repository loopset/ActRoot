#include "ActLegacy.h"
#include "ActDetectors.h"
#include "ActEventData.h"
#include "TString.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

void ActLegacy::ReadVXI(const std::string &file)
{
    std::ifstream streamer {file};
    if(!streamer)
        throw std::runtime_error("Error loading VXI in ActLegacy");
    std::string line {};
    int row {};
    int vxiIndex {};
    while(std::getline(streamer, line, '\n'))
    {
        std::istringstream lineStreamer {line};
        std::string value {};
        std::string vxiName {};
        int column {};
        int vxiSize {};
        while(std::getline(lineStreamer, value, ' '))
        {
            switch (column)
            {
            case 0:
                vxiName = value;
                break;
            case 1:
                vxiSize = std::stoi(value);
                break;
            default:
                break;
            }
            column++;
        }
        //fill map
        fVXIActions.push_back({vxiName, {vxiIndex, vxiIndex + vxiSize - 1}});// - 1 bc we start in 0!
        vxiIndex += 1000;//following Thomas' MEvent::ReadVXILabels....
        row++;
    }
}

std::pair<std::vector<TString>, std::vector<int>> ActLegacy::GetParNamesAndNumbers() const
{
    std::vector<TString> names;
    std::vector<int> numbers;
    for(const auto& [key, vals] : fVXIActions)
    {
        names.push_back(TString(key));
        numbers.push_back((vals.second - vals.first) + 1);//bc we start counting at 0
    }
    return {names, numbers};
}

void ActLegacy::MoveIteratorToItsClass(int it, double val,
                                       SiliconRawData &silicons,
                                       TriggersAndGates &t,
                                       TimeOfFlight& tof)
{
    //find it value in map of VXI parameters
    std::string vxi {};
    int minIndex {};
    for(const auto& [key, vals] : fVXIActions)
    {
        if(vals.first <= it && it <= vals.second)
        {
            vxi = key;
            minIndex = vals.first;
            break;
        }
    }
    if(vxi == "SI0_")
    {//auto determination of index!
        auto index {it - minIndex};
        silicons.fRaw[{SiliconMode::EFront, SiliconPanel::ELayer0}][index] += val;
    }
    else if(vxi == "SI1_")
    {
        auto index {it - minIndex};
        silicons.fRaw[{SiliconMode::EFront, SiliconPanel::ELayer1}][index] += val;
    }
    else if(vxi == "SIS_" || vxi == "SI_L")
    {
        auto index {it - minIndex};
        silicons.fRaw[{SiliconMode::ELeft, SiliconPanel::ELayer0}][index] += val;
    }
    else if(vxi == "SI_R")
    {
        auto index {it - minIndex};
        silicons.fRaw[{SiliconMode::ERight, SiliconPanel::ELayer0}][index] += val;
    }
    ///ended silicons -- more values likely to appear in the future
    else if(vxi == "INCONF") t.INCONF = val;
    else if(vxi == "GATCONF") t.GATCONF = val;
    else if(vxi == "CLK_UP" || vxi == "DT_CLK_UP")
        t.DT_CLK_UP = val;
    else if(vxi == "CLK" || vxi == "DT_CLK")
        t.DT_CLK = val;
    else if(vxi == "DT_GET" || vxi == "DT_GET_CLK")
        t.DT_GET = val;
    else if(vxi == "DT_GET_UP" || vxi == "DT_GET_CLK_UP")
        t.DT_GET_UP = val;
    else if(vxi == "DT_VXI" || vxi == "DT_VXI_CLK")
        t.DT_VXI = val;
    else if(vxi == "DT_VXI_UP" || vxi == "DT_VXI_CLK_UP")
        t.DT_VXI_UP = val;
    else if(vxi == "CTR_TIMEH_UP") t.CTR_TIMEH_UP = val;
    else if(vxi == "CTR_TIMEH") t.CTR_TIMEH = val;
    else if(vxi == "CTR_TIMEML_UP") t.CTR_TIMEML_UP = val;
    else if(vxi == "CTR_TIMEML") t.CTR_TIMEML = val;
    else if(vxi == "CTR_EVT_UP") t.CTR_EVT_UP = val;
    else if(vxi == "CTR_EVT") t.CTR_EVT = val;
    else if(vxi != "")
        throw std::runtime_error("Error: " + vxi + " not added yet in ActLegagy::MoveIteratorToItsClass!");
}

void ActLegacy::Print() const
{
    for(const auto& [key, vals] : fVXIActions)
        std::cout<<"ParName = "<<key<<" indexes ["<<vals.first<<" , "<<vals.second<<"]"<<'\n';
}
