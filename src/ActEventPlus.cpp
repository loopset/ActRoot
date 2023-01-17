#include "ActEventPlus.h"
#include "ActHit.h"
#include "ActParameters.h"
#include "ActCalibrations.h"

#include "ActStructs.h"
#include "ActTrackPlus.h"
#include "MEventReduced.h"
#include "TMath.h"
#include <cmath>
#include <ios>
#include <iostream>
#include <map>
#include <set>
#include <stdexcept>
#include <tuple>
#include <unordered_map>
#include <utility>
#include <vector>

ActEventPlus::ActEventPlus(const unsigned int& run, const unsigned int& entry,
                           MEventReduced*& EvtRed)
    : eventID(EvtRed->event), entryID(entry), runID(run)
{
}

void ActEventPlus::ReadData(ActCalibrations*& calibrations,
                            const std::vector<std::vector<int>>& TABLE, const std::vector<std::vector<double>>& padAlignCoefs,
                            MEvent*& Evt, MEventReduced*& EvtRed,
                            bool alignCharge)
{
    //legacy old silicons to write raw data
    Silicons oldSilicons {};
    int hitID {0};
    //to avoid repeating hits
    std::map<int, std::vector<int>> overrideHits {};
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
                     hitID,
                     TABLE, padAlignCoefs,
                     overrideHits,
                     alignCharge,
                     it, where);
        }
    }

    //AND NOW, SILICONS
    ReadAndCalibrateSilicons(oldSilicons, calibrations);
}

void ActEventPlus::ReadAllButHits(MEvent*& Evt,
                                  MEventReduced*& EvtRed,
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

void ActEventPlus::ReadHits(MEvent*& Evt,
                            MEventReduced*& EvtRed,
                            int& hitID,
                            const std::vector<std::vector<int>>& TABLE,
                            const std::vector<std::vector<double>>& padAlignCoefs,
                            std::map<int, std::vector<int>>& overrideHits,
                            const bool& alignCharge,
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
                // if(yval == 28 && xval == 7)//to check if parameters are OK!
                //     std::cout<<"Pad Y = 28"<<" Pad X = 7 and where = "<<where<<'\n';
                //this new version allows Z rebinning, just tuning two parameters in ActParameters!
                int zBin {(int)z_position / ActParameters::g_REBINZ};
                //if bin width == 1, assume value the bin index, not the bin center (would be val + 0.5)
                double zval {ActParameters::g_REBINZ * zBin + ((ActParameters::g_REBINZ <= 1) ? 0.0 : (double)ActParameters::g_REBINZ / 2)};
                
                ActHit candidate { hitID, XYZPoint(xval, yval, zval), Qiaux_align, EvtRed->CoboAsad[it].hasSaturation};

                //check saturation
                // if(candidate.GetIsSaturated() && entryID == 2216)
                //     std::cout<<"IsSaturated with Q = "<<Qiaux_align<<" at "<<candidate.GetPosition()<<'\n';
                
                //update hit if it is repeated
                int globalIndex { static_cast<int>(xval + yval * ActParameters::g_NPADX
                                                           + zBin * ActParameters::g_NPADX * ActParameters::g_NPADY)};
                overrideHits[globalIndex].push_back(hitID);
                if(overrideHits.at(globalIndex).size() > 1)
                {
                    // std::cout<<"Updating hit"<<'\n';
                    // for(const auto& i : overrideHits.at(globalIndex))
                    // {
                    //     std::cout<<"\tindexes in vector: "<<i<<'\n';
                    // }
                    auto index { overrideHits.at(globalIndex).front()};//hit index to reset is always the first in the vector
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

                // //fill pad
                // voxel.fPad[{xval, yval}].first += Qiaux_align;
                // voxel.fPad.at({xval, yval}).second = EvtRed->CoboAsad[it].hasSaturation;
            }
        }
				
	}		
}

void ActEventPlus::ReadAndCalibrateSilicons(Silicons& oldSilicons,
                                            ActCalibrations*& calibrations)
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

void ActEventPlus::CleanPileUp(const double &zMin, const double &zMax)
{
    for(auto it = voxel.fHits.begin(); it != voxel.fHits.end();)
    {
        const auto& pos {it->GetPosition()};
        if(!(zMin <= pos.Z() && pos.Z() <= zMax))
            it = voxel.fHits.erase(it);
        else
            it++;
    }
}

bool ActEventPlus::CheckTopologyInnerFunction(const std::string &silSide, const int &silIndex, const std::vector<ActHit>& hits)
{
    //width settings
    int xWidth {4};//counting reference pad
    int yWidth {4};
    int yProperWidth {2};//proper side = to where the track points
    //FRONT boundary
    int xMaxFront { ActParameters::g_NPADX - 1};
    int xMinFront { xMaxFront - (xWidth - 1)};
    int yMinWindowFront {};
    int yMaxWindowFront {};
    //WINDOW: where the beam enters
    int xMaxWindow {(xWidth - 1)};
    int xMinWindow { 0};
    //SIDE boundary
    int yMin {}; int yMax {};
    //PROPER SIDE
    int yProperMin {}; int yProperMax {};
    std::set<int> filledYVals {};
    if(silSide == ActParameters::trackHitsSiliconSideLeft)
    {
        yMax = yWidth - 1;//remember that pads go [0,31] and that we are counting the yMax/yMin in the width
        yMin = 0;

        yProperMax = ActParameters::g_NPADY - 1;
        yProperMin = yProperMax - (yProperWidth - 1);
              
        yMaxWindowFront = yProperMin - 1;
        yMinWindowFront = yMin + 1;
    }
    else if(silSide == ActParameters::trackHitsSiliconSideRight)
    {
        yMax = ActParameters::g_NPADY - 1;
        yMin = yMax - (yWidth - 1);

        yProperMax = (yProperWidth - 1);
        yProperMin = 0;

        yMaxWindowFront = yMin - 1;
        yMinWindowFront = yProperMax + 1;
    }
    else
    {
        throw std::runtime_error("CheckTopology received a wrong string for side");
    }
    //return values
    bool crossesWindow{false};
    bool crossesFront {false};
    bool crossesOppositeSide {false};
    for(const auto& hit : hits)
    {
        const auto& pos { hit.GetPosition()};
        //PROPER SIDE: check if all region has charge!
        if(yProperMin <= pos.Y() && pos.Y() <= yProperMax)
        {
            //crossesProperSide = true;
            filledYVals.insert(pos.Y());
            //std::cout<<"Y proper side: "<<pos.Y()<<" with charge "<<hit.GetCharge()<<" properSide? "<<std::boolalpha<<crossesProperSide<<'\n';
        }
        //from these cuts it is enough having only one hit! (no need for set checking)
        //OPPOSITE side check
        if(yMin <= pos.Y() && pos.Y() <= yMax)
        {
            crossesOppositeSide = true;
        }
        //FRONT and WINDOW
        if(yMinWindowFront<= pos.Y() && pos.Y() <= yMaxWindowFront)
        {
            if(xMinFront <= pos.X() && pos.X() <= xMaxFront)
            {
                crossesFront = true;
            }
            if(xMinWindow <= pos.X() && pos.X() <= xMaxWindow)
            {
                crossesWindow = true;
            }
        }      
    }
    //PROPER side check fill values
    bool crossesProperSide {true};
    for(int y = yProperMin; y <= yProperMax; y++)
    {
        const bool isInSet {filledYVals.find(y) != filledYVals.end()};
        if(!isInSet)
            crossesProperSide = false;
        //std::cout<<"proper side y: "<<y<<" isInSet: "<<std::boolalpha<<isInSet<<'\n';
    }
    //if not charge in any of the other flanges and YES charge in its proper side....
    return !(crossesOppositeSide || crossesFront || crossesWindow) && crossesProperSide;
}

bool ActEventPlus::CheckTopology(const std::string &silSide, const int &silIndex, const std::vector<ActHit>& hits)
{
    if(hits.size() != 0)
        return CheckTopologyInnerFunction(silSide, silIndex, hits);
    else
        return CheckTopologyInnerFunction(silSide, silIndex, voxel.fHits);
}

std::map<std::pair<int, int>, std::pair<double, bool>> ActEventPlus::GetPadMatrix() const
{
    std::map<std::pair<int, int>, std::pair<double, bool>> pad {};
    for(const auto& hit : voxel.fHits)
    {
        const auto& pos { hit.GetPosition()};
        auto charge { hit.GetCharge()};
        pad[{pos.X(), pos.Y()}].first += charge;
        if(hit.GetIsSaturated())
        {
            pad.at({pos.X(), pos.Y()}).second = true;
        }
    }
    //correct by minimum charge in pad to reassert isSaturated = true
    for(auto& [pos, vals] : pad)
    {
        if(vals.second)//check that saturated pads have Q > minChargeToSaturate!!
            if(vals.first < ActParameters::gMinChargeToSaturate)
                vals.second = false;
    }
    return pad;
}

int ActEventPlus::CountSaturatedPads()
{
    auto pad { GetPadMatrix()};
    int counter {};
    for(const auto& [pos, vals] : pad)
    {
        if(vals.second)
        {
            //std::cout<<"Sat pad at X = "<<pos.first<<" Y = "<<pos.second<<" with Q = "<<vals.first<<'\n';
            counter++;
        }
    }
    return counter;
}

void ActEventPlus::WriteToStreamer(std::ofstream &streamer) const
{
    streamer<<runID<<" "<<entryID<<'\n';
}
