#include "ActEventPlus.h"
#include "ActParameters.h"
#include "ActCalibrations.h"

#include "ActStructs.h"
#include "MEventReduced.h"
#include <unordered_map>
#include <utility>
#include <vector>

ActEventPlus::ActEventPlus(ActCalibrations* calibrations, MEvent*& Evt, MEventReduced*& EvtRed)
{
    //Set event ID
     eventID = EvtRed->event;
     //read data
     ReadData(calibrations, Evt, EvtRed);
}

void ActEventPlus::ReadData(ActCalibrations* calibrations, MEvent*& Evt, MEventReduced*& EvtRed)
{
    //read TABLE
    auto TABLE {calibrations->GetTABLE()};
    //legacy old silicons to write raw data
    Silicons oldSilicons {};
    int hitID {0};
    for(int it = 0; it < EvtRed->CoboAsad.size(); it++)
    {
        int co = EvtRed->CoboAsad[it].globalchannelid>>11 ;
		int as = (EvtRed->CoboAsad[it].globalchannelid - (co<<11))>>9 ;
		int ag = (EvtRed->CoboAsad[it].globalchannelid - (co<<11)-(as<<9))>>7 ;
		int ch = EvtRed->CoboAsad[it].globalchannelid - (co<<11)-(as<<9)-(ag<<7) ;
		int where = co * ActParameters::g_NB_ASAD * ActParameters::g_NB_AGET * ActParameters::g_NB_CHANNEL +
			as * ActParameters::g_NB_AGET * ActParameters::g_NB_CHANNEL +
			ag * ActParameters::g_NB_CHANNEL +
			ch ;

        if(co == 31)
        {
            ReadAllButHits(Evt, EvtRed, oldSilicons, it);
        }
        if((co != 31) && (co != 16))
        {
            ReadHits(Evt, EvtRed,
                    hitID, TABLE,
                    it, where);
        }
    }

    //AND NOW, SILICONS
    ReadAndCalibrateSilicons(oldSilicons, calibrations);
}

void ActEventPlus::ReadAllButHits(MEvent* Evt,
                                  MEventReduced* EvtRed,
                                  Silicons& oldSilicons,
                                  const int& it)
{
    for(unsigned int hit = 0; hit < EvtRed->CoboAsad[it].peaktime.size(); hit++)
    {

        int index = Evt->labelVXI[(int)(EvtRed->CoboAsad[it].peaktime[hit])];
        int counter = 0 ;
        //Read SILICON data
        if(index > -1 && index < 6)
        {
            int SilLeft0Index { index};
            auto value { EvtRed->CoboAsad[it].peakheight[hit]};
            oldSilicons.fSilSide0.at("left").at(SilLeft0Index) = value;
        }
        //Index for Si signal tunned for the heavy particle
        if(index>999 && index<1006)
        {
            int SilRight0Index { index - 1000};
            auto value { EvtRed->CoboAsad[it].peakheight[hit]};
					
            oldSilicons.fSilSide0.at("right").at(SilRight0Index) = value;
        }
        //Triggers, Gates and TOF
        if(index == 2000  ){ triggers.INCONF         = EvtRed->CoboAsad[it].peakheight[hit] ; }
        if(index == 3000  ){ triggers.GATCONF        = EvtRed->CoboAsad[it].peakheight[hit] ; }
        if(index == 4000  ){ tof.tSilR13             = EvtRed->CoboAsad[it].peakheight[hit] ; }
        if(index == 5000  ){ tof.tSilR46             = EvtRed->CoboAsad[it].peakheight[hit] ; }
        if(index == 6000  ){ tof.tSilL13             = EvtRed->CoboAsad[it].peakheight[hit] ; }
        if(index == 7000  ){ tof.tSilL46             = EvtRed->CoboAsad[it].peakheight[hit] ; }}
}

void ActEventPlus::ReadHits(MEvent* Evt,
                            MEventReduced* EvtRed,
                            int& hitID,
                            const std::vector<std::vector<int>>& TABLE,
                            const int& it, const int& where)
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
                double Qiaux_align {Qiaux};

                ActHit candidate { hitID, XYZPoint(xval, yval, z_position), Qiaux_align, EvtRed->CoboAsad[it].hasSaturation};
                voxel.fHits.push_back(candidate);
                hitID++;

                // //fill pad
                // voxel.fPad[{xval, yval}].first += Qiaux_align;
                // voxel.fPad.at({xval, yval}).second = EvtRed->CoboAsad[it].hasSaturation;
            }
        }
				
	}		
}

void ActEventPlus::ReadAndCalibrateSilicons(Silicons& oldSilicons,
                                            ActCalibrations* calibrations)
{
    auto cal { calibrations->GetSiliconSideCalibrations()};
    //CALIBRATE
    for(const auto& [side, vec] : oldSilicons.fSilSide0)
    {
        int silIndex {0};//in file, starts at 1, so that we add an 1 when reading calibration map
        for(const auto& raw : vec)
        {
            if(raw == 0.0 || raw == std::pow(2, 14) - 1)//saturated
            {
                oldSilicons.fSilSide0_cal.at(side).at(silIndex) = cal.at(side).at(silIndex).at(0) + cal.at(side).at(silIndex).at(1) * std::pow(2, 14) -1 ;
            }
            if(raw < cal.at(side).at(silIndex).at(2))//below is threshold
            {
                oldSilicons.fSilSide0_cal.at(side).at(silIndex) = 0.0;
            }
            if(raw > cal.at(side).at(silIndex).at(2))//signal over threshold
            {
                oldSilicons.fSilSide0_cal.at(side).at(silIndex) = cal.at(side).at(silIndex).at(0) + cal.at(side).at(silIndex).at(1) * raw ;
            }
            silIndex++;
        }
    }

    //WRITE DATA
    for(const auto& [side, vec] : oldSilicons.fSilSide0_cal)
    {
        int silIndex {0};
        double Eside {0};
        int multiplicity {0};
        int pad {-1};
        for(const auto& energy : vec)
        {
            if(energy > ActParameters::g_SilSide0Thresholds.at(side).at(silIndex))
            {
                multiplicity += 1;
                Eside = energy;
                pad = silIndex + 1;//add one to silicon number!
            }

            silIndex++;
        }
        //and write data to map
        silicons.fData[side]["M"] = multiplicity;
        if(multiplicity != 1)
        {
            silicons.fData[side]["P"] = -1;
            silicons.fData[side]["E"] = -1;
        }
        else
        {
            silicons.fData[side]["P"] = pad;
            silicons.fData[side]["E"] = Eside;
        }
    }    
}
