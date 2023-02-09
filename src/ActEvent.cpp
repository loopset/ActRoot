#include "ActEvent.h"

#include "ActDetectors.h"
#include "ActHit.h"
#include "ActCalibrations.h"
#include "ActEventData.h"
#include "ActRoot.h"

#include "MEventReduced.h"

#include "TMath.h"

#include <cmath>
#include <ios>
#include <iostream>
#include <stdexcept>
#include <unordered_map>
#include <utility>
#include <vector>

ActEvent::ActEvent(unsigned int run, unsigned int entry,
                   MEventReduced*& EvtRed)
    : eventID(EvtRed->event), entryID(entry), runID(run)
{
}

void ActEvent::ReadData(ActCalibrations*& calibrations,
                        const std::vector<std::vector<int>>& TABLE,
                        const std::vector<std::vector<double>>& padAlignCoefs,
                        MEvent*& Evt, MEventReduced*& EvtRed,
                        bool alignCharge,
                        bool silIndividualThresh,
                        bool writeSilicons,
                        bool debugSilicons)
{
    //raw data is temporary only!
    SiliconRawData siliconraw {};
    int hitID {0};
    //to avoid repeating hits
    std::unordered_map<int, std::pair<int, int>> overrideHits {};
    for(int it = 0; it < EvtRed->CoboAsad.size(); it++)
    {
        int co = EvtRed->CoboAsad[it].globalchannelid>>11 ;
		int as = (EvtRed->CoboAsad[it].globalchannelid - (co<<11))>>9 ;
		int ag = (EvtRed->CoboAsad[it].globalchannelid - (co<<11)-(as<<9))>>7 ;
		int ch = EvtRed->CoboAsad[it].globalchannelid - (co<<11)-(as<<9)-(ag<<7) ;
		int where = co * ActRoot::Get()->chamber.fNB_ASAD * ActRoot::Get()->chamber.fNB_AGET * ActRoot::Get()->chamber.fNB_CHANNEL +
			as * ActRoot::Get()->chamber.fNB_AGET * ActRoot::Get()->chamber.fNB_CHANNEL +
			ag * ActRoot::Get()->chamber.fNB_CHANNEL +
			ch ;

        if(co == 31)
        {
            ReadAllButHits(Evt, EvtRed, siliconraw, it);
        }
        if((co != 31) && (co != 16))
        {
            ReadHits(Evt, EvtRed,
                     hitID,
                     TABLE, padAlignCoefs,
                     overrideHits,
                     alignCharge,
                     it, where);
        }
    }

    //AND NOW, SILICONS
    if(writeSilicons)
        CalibrateSilicons(calibrations, siliconraw, silIndividualThresh, debugSilicons);
}

void ActEvent::ReadAllButHits(MEvent*& Evt,
                              MEventReduced*& EvtRed,
                              SiliconRawData& siliconraw,
                              int it)
{
    for(unsigned int hit = 0; hit < EvtRed->CoboAsad[it].peaktime.size(); hit++)
    {
        
        int indexVXI = Evt->labelVXI[(int)(EvtRed->CoboAsad[it].peaktime[hit])];
        auto val {EvtRed->CoboAsad[it].peakheight[hit]};

        ActRoot::Get()->acq.MoveIteratorToItsClass(indexVXI, val,
                                                   siliconraw,
                                                   triggers,
                                                   tof);
    }
}

void ActEvent::ReadHits(MEvent*& Evt,
                        MEventReduced*& EvtRed,
                        int& hitID,
                        const std::vector<std::vector<int>>& TABLE,
                        const std::vector<std::vector<double>>& padAlignCoefs,
                        std::unordered_map<int, std::pair<int, int>>& overrideHits,
                        bool alignCharge,
                        int it, int where)
{
    auto xval { static_cast<int>(TABLE[4][where])};
    auto yval { static_cast<int>(TABLE[5][where])};
    for(int hit = 0; hit < EvtRed->CoboAsad[it].peakheight.size(); hit++)
    {
        if(TABLE[5][where] != -1)
        {
            double z_position { EvtRed->CoboAsad[it].peaktime[hit]};
            if(z_position > 0.)
            {
                auto Qiaux { EvtRed->CoboAsad[it].peakheight[hit]};
                double Qiaux_align {};
                if(!alignCharge)
                    Qiaux_align = Qiaux;
                else
                {
                    int order {0};
                    for(const auto& coef : padAlignCoefs.at(where))
                    {
                        Qiaux_align += coef * TMath::Power(Qiaux, order);
                        order++;
                    }
                }
                //get rebinning parameters (to avoid writing everywhere the same code)
                int rebinZ {ActRoot::Get()->chamber.fREBINZ};
                //this new version allows Z rebinning, just tuning two parameters in ActParameters!
                int zBin {(int)z_position / rebinZ};
                //if bin width == 1, assume value the bin index, not the bin center (would be val + 0.5)
                double zval {rebinZ * zBin + ((rebinZ <= 1) ? 0.0 : (double)rebinZ / 2)};
                
                ActHit candidate { hitID, XYZPoint(xval, yval, zval), Qiaux_align, EvtRed->CoboAsad[it].hasSaturation};
                
                //update hit if repeated
                int globalIndex { static_cast<int>(xval + yval * ActRoot::Get()->chamber.fNPADSX
                                                   + zBin * ActRoot::Get()->chamber.fNPADSX *
                                                   ActRoot::Get()->chamber.fNPADSY)};
                overrideHits[globalIndex].first += 1;
                if(overrideHits.at(globalIndex).first == 1)
                    overrideHits.at(globalIndex).second = hitID;
                //.second keeps hitID = index in vector of "repeated" hit, the one to be updated
                if(overrideHits.at(globalIndex).first > 1)
                {
                    auto index { overrideHits.at(globalIndex).second};
                    //std::cout<<"\t\tIndex to update: "<<index<<'\n';
                    auto alreadyCharge { voxel.fHits.at(index).GetCharge()};
                    voxel.fHits.at(index).SetCharge(alreadyCharge + Qiaux_align);
                    if(candidate.GetIsSaturated())//update also IsSaturated flag to true if ANY repeated hit is saturated
                        voxel.fHits.at(index).SetIsSaturated(true);
                }
                else
                {
                    voxel.fHits.push_back(candidate);
                    hitID++;
                }
            }
        }
	}		
}

void ActEvent::CalibrateSilicons(ActCalibrations *calibrations, SiliconRawData &raw,
                                 bool silIndividualThresh,
                                 bool debugSilicons)
{
    //call to calibrate function!
    for(const auto& key : ActRoot::Get()->silicons.fModes)
    {
        auto [mode, panel] = key;
        if(raw.fRaw.count(key))
        {
            if(raw.fRaw.at(key).size() > 0)
            {
                const auto& calibrationData {calibrations->GetSiliconCalibration(mode, panel)};
                raw.fCal[key] = {};//initialize cal silicons!
                raw.Calibrate(calibrationData, raw.fRaw.at(key), raw.fCal.at(key));
            }
        }
    }

    //and now read and write data!
    raw.ReadAndWrite(silicons, silIndividualThresh);
    if(debugSilicons)raw.Print();
    if(debugSilicons)silicons.Print();
    
}
