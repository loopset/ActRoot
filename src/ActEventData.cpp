#include "ActEventData.h"
#include "ActDetectors.h"
#include "ActRoot.h"
#include "RtypesCore.h"

#include <cmath>
#include <iostream>

void SiliconRawData::Calibrate(const std::map<int, std::vector<double>>& calibration,
                      const std::map<int, double>& raw,
                      std::map<int, double>& cal)
{
    for(const auto& [index, val] : raw)
    {
        if(val == 0.0 || val == std::pow(2, 14) - 1)//saturated
        {
            cal[index] = calibration.at(index).at(0) +
                calibration.at(index).at(1) * std::pow(2, 14) -1 ;
        }
        if(val < calibration.at(index).at(2))//below is threshold
        {
           cal[index] = 0.0;
        }
        if(val > calibration.at(index).at(2))//signal over threshold
        {
            cal[index] = calibration.at(index).at(0) +calibration.at(index).at(1) * val ;
        }
    }
}

void SiliconRawData::ReadAndWrite(SiliconData &fin, bool individualThreshold)
{
    
    for(const auto& key : ActRoot::GetSilicons().fModes)
    {
        auto [mode, panel] = key;
        if(fCal.count(key))
        {
            Float_t E {-1}; Int_t M {-1}; Int_t P {-1};
            for(const auto& [index, val] : fCal.at(key))
            {               
                Float_t thresh {};
                //check threshold!
                if(!individualThreshold)
                {
                    thresh = ActRoot::GetSilicons().fMap.at(key)
                        .fCommonEnergyThreshold;
                    if(val > thresh)
                    {
                        E = val;
                        M += 1;
                        P = index;
                    }
                }
                else
                {
                    thresh = ActRoot::GetSilicons().fMap.at(key)
                        .fPlacements.at(index).fEnergyThreshold;
                    if(val > thresh)
                    {
                        E = val;
                        M += 1;
                        P = index;
                    }
                }
                if(mode == SiliconMode::kLeft)
                    std::cout<<"Val for left Si: "<<val<<'\n';
            }
            //and save data according to multiplicity
            if(M != 1)
            {
                E = -1; P = -1;
            }
            fin.fData[key]["E"] = E;
            fin.fData[key]["P"] = P;
            fin.fData[key]["M"] = M;
        }
    }
}

void SiliconData::Print() const
{
    std::cout<<BOLDMAGENTA<<"===== Silicons ====="<<std::endl;
    for(const auto& [key, vals] : fData)
    {
        auto [mode, panel] = key;
        std::cout<<"At " << mode<<" and "<<panel<<'\n';
        std::cout<<" E = "<<vals.at("E")<<" MeV and pad = "<<vals.at("P")<<'\n';
    }
    std::cout<<RESET<<std::flush;
}
