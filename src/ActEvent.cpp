#include "ActEvent.h"

#include "ActHit.h"
#include "ActLine.h"
#include "ActStructs.h"
#include "ActClusteringResults.h"
#include "ActTrack.h"
#include "ActParameters.h"
#include "ActCalibrations.h"

#include "MEvent.h"
#include "MEventReduced.h"

#include <algorithm>
#include <cmath>
#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <stdexcept>
#include <string>
#include <sys/types.h>
#include <utility>
#include <vector>
#include <math.h>

#include <TMath.h>

ActEvent::ActEvent()
	: voxel(ActParameters::g_NPADX *  ActParameters::g_NPADY *  ActParameters::g_NPADZ),
	  indexOfVoxelInHitArray(ActParameters::g_NPADX * ActParameters::g_NPADY * ActParameters::g_NPADZ, -1),
	  chargeInPad(ActParameters::g_NPADX, std::vector<double>(ActParameters::g_NPADY, -1.)),
      saturationMatrix(ActParameters::g_NPADX, std::vector<bool>(ActParameters::g_NPADY, false))
{
}

void ActEvent::Reset(std::string mode)
{
	if(mode == "shallow")//because we use to gate only at this stage
	{
		fEventID = -1;
		fTriggers = {};
		fSilicons = {};
        fEventInfo = {};
	}
	else if(mode == "deep")//fully reset events which go through gates
	{
		fEventID = -1;
		fTriggers = {};
		fHitArray.clear();
		fSilicons = {};
        fEventInfo = {};
		for(const auto& indexToReset : globalIndexToReset)
		{
			voxel[indexToReset] = 0;
			indexOfVoxelInHitArray[indexToReset] = -1;
		}
		globalIndexToReset.clear();
        for(const auto& pair : padToReset)
        {
            chargeInPad.at(pair.first).at(pair.second)      = -1.;
        }
        padToReset.clear();
        for(const auto& pair : saturationToReset)
        {
            saturationMatrix.at(pair.first).at(pair.second) = false;
        }
        saturationToReset.clear();
		//voxel.assign(voxel.size(), 0);
		//indexOfVoxelInHitArray.assign(indexOfVoxelInHitArray.size(), -1);
		//reset of chargeInPad is done in its method! (so it is done only when method is called)
		fTracks.clear();
	}
	else
	{
		throw std::runtime_error("Wrong string passed to ActEvent::Reset()!");
	}
}

void ActEvent::ReadTriggersAndGates(const MEvent *Evt, const MEventReduced *EvtRed)
{
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

		//Read SILICON data
		if( co == 31 ){
			for(unsigned int hit = 0; hit < EvtRed->CoboAsad[it].peaktime.size(); hit++)
			{

				int index = Evt->labelVXI[(int)(EvtRed->CoboAsad[it].peaktime[hit])];
				int counter = 0 ;

				if(index > -1 && index < 6)
				{
					int SilLeft0Index { index};
                    auto value { EvtRed->CoboAsad[it].peakheight[hit]};
                    fSilicons.fSilSide0.at("left").at(SilLeft0Index) = value;
				}
				//Index for Si signal tunned for the heavy particle
				if(index>999 && index<1006)
				{
					int SilRight0Index { index - 1000};
					auto value { EvtRed->CoboAsad[it].peakheight[hit]};
					
					fSilicons.fSilSide0.at("right").at(SilRight0Index) = value;
				}
				
				//Triggers and Gates
				if(index == 2000  ){ fTriggers.INCONF         = EvtRed->CoboAsad[it].peakheight[hit] ; }
				if(index == 3000  ){ fTriggers.GATCONF        = EvtRed->CoboAsad[it].peakheight[hit] ; } 
				// if(index == 8000  ){ fTriggers.TVAL_CFA_HF    = EvtRed->CoboAsad[it].peakheight[hit] ; }
				// if(index == 9000  ){ fTriggers.TVAL_CFA_CATD4 = EvtRed->CoboAsad[it].peakheight[hit] ; }
				// if(index == 10000 ){ fTriggers.TVAL_CFA_CATD6 = EvtRed->CoboAsad[it].peakheight[hit] ; }

				// if(index == 11000 ){ fTriggers.DT_CLK_UP      = EvtRed->CoboAsad[it].peakheight[hit] ; }
				// if(index == 12000 ){ fTriggers.DT_CLK         = EvtRed->CoboAsad[it].peakheight[hit] ; }
				// if(index == 13000 ){ fTriggers.DT_GET_UP      = EvtRed->CoboAsad[it].peakheight[hit] ; }
				// if(index == 14000 ){ fTriggers.DT_GET         = EvtRed->CoboAsad[it].peakheight[hit] ; }
				// if(index == 15000 ){ fTriggers.DT_VXI_UP      = EvtRed->CoboAsad[it].peakheight[hit] ; }
				// if(index == 16000 ){ fTriggers.DT_VXI         = EvtRed->CoboAsad[it].peakheight[hit] ; }

				// if(index == 17000 ){ fTriggers.CTR_TIMEH_UP   = EvtRed->CoboAsad[it].peakheight[hit] ; }
				// if(index == 18000 ){ fTriggers.CTR_TIMEH      = EvtRed->CoboAsad[it].peakheight[hit] ; }
				// if(index == 19000 ){ fTriggers.CTR_TIMEML_UP  = EvtRed->CoboAsad[it].peakheight[hit] ; }
				// if(index == 20000 ){ fTriggers.CTR_TIMEML     = EvtRed->CoboAsad[it].peakheight[hit] ; }

				// if(index == 21000 ){ fTriggers.CTR_EVT_UP     = EvtRed->CoboAsad[it].peakheight[hit] ; }
				// if(index == 22000 ){ fTriggers.CTR_EVT        = EvtRed->CoboAsad[it].peakheight[hit] ; }	

				// if(index == 23000 ){ fTriggers.SCA_TiD3_UP    = EvtRed->CoboAsad[it].peakheight[hit] ; }
				// if(index == 24000 ){ fTriggers.SCA_TiD3       = EvtRed->CoboAsad[it].peakheight[hit] ; }	
				// if(index == 25000 ){ fTriggers.SCA_CATSD4_UP  = EvtRed->CoboAsad[it].peakheight[hit] ; }
				// if(index == 26000 ){ fTriggers.SCA_CATSD4     = EvtRed->CoboAsad[it].peakheight[hit] ; }	
				// if(index == 27000 ){ fTriggers.SCA_CATSD5_UP  = EvtRed->CoboAsad[it].peakheight[hit] ; }
				// if(index == 28000 ){ fTriggers.SCA_CATSD5     = EvtRed->CoboAsad[it].peakheight[hit] ; }	
				// if(index == 29000 ){ fTriggers.SCA_CFA_UP     = EvtRed->CoboAsad[it].peakheight[hit] ; }
				// if(index == 30000 ){ fTriggers.SCA_CFA        = EvtRed->CoboAsad[it].peakheight[hit] ; }	
				// if(index == 31000 ){ fTriggers.SCA_SiBeam1_UP = EvtRed->CoboAsad[it].peakheight[hit] ; }
				// if(index == 32000 ){ fTriggers.SCA_SiBeam1    = EvtRed->CoboAsad[it].peakheight[hit] ; }
				// if(index == 33000 ){ fTriggers.SCA_SiBeam2_UP = EvtRed->CoboAsad[it].peakheight[hit] ; }
				// if(index == 34000 ){ fTriggers.SCA_SiBeam2    = EvtRed->CoboAsad[it].peakheight[hit] ; }	
				// if(index == 35000 ){ fTriggers.SCA_Si0_UP     = EvtRed->CoboAsad[it].peakheight[hit] ; }
				// if(index == 36000 ){ fTriggers.SCA_Si0        = EvtRed->CoboAsad[it].peakheight[hit] ; }
				// if(index == 37000 ){ fTriggers.SCA_SiS_UP     = EvtRed->CoboAsad[it].peakheight[hit] ; }
				// if(index == 38000 ){ fTriggers.SCA_SiS        = EvtRed->CoboAsad[it].peakheight[hit] ; }
				// if(index == 39000 ){ fTriggers.SCA_L1ok_UP    = EvtRed->CoboAsad[it].peakheight[hit] ; }
				// if(index == 40000 ){ fTriggers.SCA_L1ok       = EvtRed->CoboAsad[it].peakheight[hit] ; }
			}
		}
	}
}

void ActEvent::ReadHits(const ActCalibrations &calibrations, const MEventReduced *EvtRed, bool alignPads)
{
	auto TABLE { calibrations.GetTABLE()};
	std::vector<std::vector<double>> PadAlignCoefs { (alignPads) ? calibrations.GetPadAlignCoefs() : std::vector<std::vector<double>>{} };

	int hitID { 0};
	//read to array of ActHits without considering rebinning by now
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
             
		// Read HITS in 3D!
		if((co != 31) && (co != 16))
		{
            auto xval { static_cast<int>(TABLE[4][where])};
            auto yval { static_cast<int>(TABLE[5][where])};
            if(EvtRed->CoboAsad[it].hasSaturation)
            {
                saturationMatrix.at(xval).at(yval) = true;
                saturationToReset.push_back({xval, yval});
            }
            padToReset.push_back({xval, yval});//outside of inner for loop to avoid repetitions
			for(int hit = 0; hit < EvtRed->CoboAsad[it].peakheight.size(); hit++)
			{
                if(TABLE[5][where] != -1)
                {
                    double z_position { EvtRed->CoboAsad[it].peaktime[hit]};
                    if(z_position > 0.)
                    {
                        auto Qiaux { EvtRed->CoboAsad[it].peakheight[hit]};
                        double Qiaux_align {};
                        if(alignPads)
                        {
                            Qiaux_align = PadAlignCoefs[where][0] + PadAlignCoefs[where][1] * Qiaux
                                + PadAlignCoefs[where][2] * Qiaux * Qiaux;
                        }
                        else
                        {
                            Qiaux_align = Qiaux;
                        }
                        //add to pad matrix
                        chargeInPad.at(xval).at(yval) += Qiaux_align;
                        
                        //HOPEFULLY, final version
                        ActHit candidate { hitID, XYZPoint(xval, yval, z_position), Qiaux_align};
                        //define a global index for 1D vectors voxel and indexOfHit
                        int globalIndex { static_cast<int>(xval + yval * ActParameters::g_NPADX
                                                           + z_position * ActParameters::g_NPADX * ActParameters::g_NPADY)};
                        //increase count on voxel
                        voxel[globalIndex] += 1;
                        indexOfVoxelInHitArray[globalIndex] = hitID;
                        globalIndexToReset.push_back(globalIndex);
						
                        if(voxel[globalIndex] > 1)//then we have to append charge to already existent hit
                        {
                            //std::cout<<BOLDCYAN<<"Found coincidence in fHitArray -> Resetting hit"<<RESET<<'\n';
                            //we already have the index in fHitArray!!!
                            auto alreadyCharge { fHitArray[indexOfVoxelInHitArray[globalIndex]].GetCharge()};
                            fHitArray[indexOfVoxelInHitArray[globalIndex]].SetCharge(alreadyCharge + Qiaux_align);
                        }
                        else//otherwise (by default, do not check)
                        {
                            fHitArray.push_back(candidate);
                            hitID++;
                        }
                    }
                }
				
			}
		}
	}
}

//calibration functions
void ActEvent::CalibrateSideSilicons(const ActCalibrations& calibrations)
{
    auto cal { calibrations.GetSiliconSideCalibrations()};
    //run calibration
    for(const auto& [side, vec] : fSilicons.fSilSide0)
    {
        int silIndex {0};//in file, starts at 1, so that we add an 1 when reading calibration map
        for(const auto& raw : vec)
        {
            // std::cout<<"Calibrating val: "<<raw<<" at side: "<<side<<" in silicon: "<<silIndex<<'\n';
            // std::cout<<"p0: "<<cal.at(side).at(silIndex + 1).at(0)<<'\n';
            // std::cout<<"p1: "<<cal.at(side).at(silIndex + 1).at(1)<<'\n';
            // std::cout<<"Predefined value: "<<fSilicons.fSilSide0_cal.at(side).at(silIndex)<<'\n';
            // std::cout<<"Here"<<'\n';
             if(raw == 0.0 || raw == std::pow(2, 14) - 1)//saturated
             {
                 fSilicons.fSilSide0_cal.at(side).at(silIndex) = cal.at(side).at(silIndex).at(0) + cal.at(side).at(silIndex).at(1) * std::pow(2, 14) -1 ;
             }
             if(raw < cal.at(side).at(silIndex).at(2))//below is threshold
             {
                 fSilicons.fSilSide0_cal.at(side).at(silIndex) = 0.0;
             }
             if(raw > cal.at(side).at(silIndex).at(2))//signal over threshold
             {
                 fSilicons.fSilSide0_cal.at(side).at(silIndex) = cal.at(side).at(silIndex).at(0) + cal.at(side).at(silIndex).at(1) * raw ;
             }

             // std::cout<<"With value: "<<fSilicons.fSilSide0_cal.at(side).at(silIndex)<<'\n';
            silIndex++;
        }
    }
    
}

void ActEvent::ReadSideSiliconsData()
{
    for(const auto& [side, vec] : fSilicons.fSilSide0_cal)
    {
        int silIndex {0};
        double Eside {0};
        int multiplicity {0};
        int pad {-1};
        for(const auto& energy : vec)
        {
            //  std::cout<<"Side: "<<side<<" silicon: "<<silIndex<<" energy: "<<energy<<'\n';
            if(energy > ActParameters::minESiSToIncreaseMultiplicity)
            {
                multiplicity += 1;
                Eside = energy;
                pad = silIndex + 1;//add one to silicon number!
            }

            silIndex++;
        }

        //and write data to map
        fSilicons.fData[side]["M"] = multiplicity;
        if(multiplicity != 1)
        {
            fSilicons.fData[side]["P"] = -1;
            fSilicons.fData[side]["E"] = -1;
        }
        else
        {
            fSilicons.fData[side]["P"] = pad;
            fSilicons.fData[side]["E"] = Eside;
        }
    }    
}

void ActEvent::CalibrateSilicons01S(const ActCalibrations &calibrations)
{
	// //Implementation of SiVAMOS_Calibration_vfinal from Juan
	// auto siliconCalibrations { calibrations.GetSilicon01SCalibrations()};
	// //auxiliary vectors
	// std::map<std::string, std::vector<double>> offsets, slopes, thresholds;
	// //filling map with auxiliary values
	// int counter { 0};
	// for(auto& silicon : ActParameters::SiCalNames)
	// {
	// 	for(int i = 0; i < ActParameters::Si01SCalibrationRows[silicon]; i++)
	// 	{
	// 		offsets[silicon].push_back(siliconCalibrations[silicon][i][0]);
	// 		slopes[silicon].push_back(siliconCalibrations[silicon][i][1]);
	// 		thresholds[silicon].push_back(siliconCalibrations[silicon][i][2]
	// 									  + 3. * siliconCalibrations[silicon][i][3]);
	// 	}
	// 	counter++;
	// }

	// for(int i = 0; i < ActParameters::NrowsSi01; i++)//aka i < 11
	// {
	// 	//light particle for VAMOS 1st layer
	// 	if(fSilicons.fSilFront0[i] == 0 || fSilicons.fSilFront0[i] == std::pow(2, 14) -1)
	// 	{
	// 		//channel is saturated
	// 		fSilicons.fSilFront0_cal[i] = offsets["0"][i] + slopes["0"][i]*std::pow(2, 14) -1 ;
	// 	}
	// 	if(fSilicons.fSilFront0[i] > 0. && fSilicons.fSilFront0[i] < thresholds["0"][i])
	// 	{
	// 		//below threshold, just noise
	// 		fSilicons.fSilFront0_cal[i] = 0.;
	// 	}
	// 	if(fSilicons.fSilFront0[i] > thresholds["0"][i])
	// 	{
	// 		//signal over noise
	// 		fSilicons.fSilFront0_cal[i] = offsets["0"][i] + slopes["0"][i] * fSilicons.fSilFront0[i];
	// 	}

	// 	//same for 2nd layer
	// 	if(fSilicons.fSilFront1[i] == 0 || fSilicons.fSilFront1[i] == std::pow(2, 14) -1)
	// 	{
	// 		//channel is saturated
	// 		fSilicons.fSi1_cal[i] = offsets["1"][i] + slopes["1"][i]*std::pow(2, 14) -1 ;
	// 	}
	// 	if(fSilicons.fSi1[i] > 0. && fSilicons.fSi1[i] < thresholds["1"][i])
	// 	{
	// 		//below threshold, just noise
	// 		fSilicons.fSi1_cal[i] = 0.;
	// 	}
	// 	if(fSilicons.fSi1[i] > thresholds["1"][i])
	// 	{
	// 		//signal over noise
	// 		fSilicons.fSi1_cal[i] = offsets["1"][i] + slopes["1"][i] * fSilicons.fSi1[i];
	// 	}

	// 	//side layer
	// 	if( i < 8 )
	// 	{
	// 		if(fSilicons.fSiS[i] == 0 || fSilicons.fSiS[i] == std::pow(2, 14) -1)
	// 		{
	// 			//channel is saturated
	// 			fSilicons.fSiS_cal[i] = offsets["S"][i] + slopes["S"][i]*std::pow(2, 14) -1 ;
	// 		}
	// 		if(fSilicons.fSiS[i] > 0. && fSilicons.fSiS[i] < thresholds["S"][i])
	// 		{
	// 			//below threshold, just noise
	// 			fSilicons.fSiS_cal[i] = 0.;
	// 		}
	// 		if(fSilicons.fSiS[i] > thresholds["S"][i])
	// 		{
	// 			//signal over noise
	// 			fSilicons.fSiS_cal[i] = offsets["S"][i] + slopes["S"][i] * fSilicons.fSiS[i];
	// 		}
	// 	}

	// 	//and now combination of Si0 + Si1
	// 	if(fSilicons.fSi1_cal[i] > 0.5)
	// 	{
	// 		fSilicons.fSiF_cal[i] = fSilicons.fSi0_cal[i] + fSilicons.fSi1_cal[i];
	// 	}
	// 	else
	// 	{
	// 		fSilicons.fSiF_cal[i] = fSilicons.fSi0_cal[i];
	// 	}
	// }
}

void ActEvent::CalibrateSiliconsBeam(const ActCalibrations& calibrations)
{
	auto siliconCalibrations { calibrations.GetSiliconBeamCalibrations()};

	std::vector<double> offsets, slopes;
	//name stored values in ActCalibrations
	for(int i = 0; i < ActParameters::NrowsSiBeam; i++)
	{
		offsets.push_back(siliconCalibrations[i][0]);
		slopes.push_back(siliconCalibrations[i][1]);
	}

	//and calibrate
	for(int i = 0; i < ActParameters::NrowsSiBeam; i++)
	{
		//channel saturated
		if(fSilicons.fSiBeam[i] == 0.)
		{
			fSilicons.fSiBeam_cal[i] = -1000;
		}
		if(fSilicons.fSiBeam[i] > 0. && fSilicons.fSiBeam[i] < ActParameters::SiBeamCalibrationThresholds[i])
		{
			//just noise
			fSilicons.fSiBeam_cal[i] = 0.;
		}
		if(fSilicons.fSiBeam[i] > ActParameters::SiBeamCalibrationThresholds[i])
		{
			//signal over noise
			fSilicons.fSiBeam_cal[i] = offsets[i] + slopes[i] * fSilicons.fSiBeam[i];
		}
	}
}

void ActEvent::CalibrateSilicons(const ActCalibrations &calibrations)
{
	//just a wrapper to call individually Si01S and SiBeam calibration funcs
	CalibrateSilicons01S(calibrations);
	CalibrateSiliconsBeam(calibrations);
}

void ActEvent::ReadSilicons01FData()
{
	// //auxiliar variables
	// double ESi0 {};
	// double ESi1 {};
	// double ESiF {};
	// int multiplicity {};
	// int hitPad {};

	// //for(auto& el : fSilicons.fSi0_cal) std::cout<<"FSi0_cal[i]: "<<el<<'\n';
	// for(int i = 0; i < fSilicons.fSi0_cal.size(); i++)
	// {
	// 	if(fSilicons.fSi0_cal[i] > ActParameters::minESi0ToIncreaseMultiplicity)
	// 	{
	// 		multiplicity += 1;
	// 		ESi0 = fSilicons.fSi0_cal[i];
	// 		ESi1 = fSilicons.fSi1_cal[i];
	// 		hitPad = i;
	// 		ESiF = ESi0 + ESi1;
	// 	}
	// }
	// // //set multipliccity
	// fSilicons.fData["01F"]["M"] = multiplicity;

	// if(multiplicity != 1)
	// {
	// 	fSilicons.fData["01F"]["P"] = 1000;
	// 	fSilicons.fData["01F"]["E0"] = 0;
	// 	fSilicons.fData["01F"]["E1"] = 0;
	// 	fSilicons.fData["01F"]["EF"] = 0;
	// }
	// else
	// {
	// 	fSilicons.fData["01F"]["P"] = hitPad;
	// 	fSilicons.fData["01F"]["E0"] = ESi0;
	// 	fSilicons.fData["01F"]["E1"] = ESi1;
	// 	fSilicons.fData["01F"]["EF"] = ESiF;
	// }
}

void ActEvent::ReadSiliconsSData()
{
	// double ESiS {};
	// int multiplicity {};
	// int hitPad {};

	// for(int i = 0; i < fSilicons.fSiS_cal.size(); i++)
	// {
	// 	if(fSilicons.fSiS_cal[i] > ActParameters::minESiSToIncreaseMultiplicity)
	// 	{
	// 		multiplicity += 1;
	// 		ESiS = fSilicons.fSiS_cal[i];
	// 		hitPad = i;
	// 	}
	// }

	// fSilicons.fData["S"]["M"] = multiplicity;
	// if(multiplicity != 1)
	// {
	// 	fSilicons.fData["S"]["P"] = 1000;
	// 	fSilicons.fData["S"]["ES"] = 0;
	// }
	// else
	// {
	// 	fSilicons.fData["S"]["P"] = hitPad;
	// 	fSilicons.fData["S"]["ES"] = ESiS;
	// }
}

void ActEvent::ReadSiliconsData()
{
    //NOT VALID FOR NFS EXPERIMENTS
	//call subfunctions
	ReadSilicons01FData();
	ReadSiliconsSData();
	//missing beam?
}

void ActEvent::CountSaturatedPads()
{
    int counter {0};
    for(const auto& xVector : saturationMatrix)
    {
        for(const auto& val : xVector)
        {
            if(val)
                counter++;
        }
    }
    fEventInfo.fSaturatedPads = counter;
}

void ActEvent::ComputeChargeAverage()
{
    //global charge average
    double chargeSum     {};
    int numberOfPads     {};
    for(const auto& hit : fHitArray)
    {
        double charge {hit.GetCharge()};
        if(charge > 0.0)
        {
            chargeSum += charge;
            numberOfPads++;
        }
    }
    //return average
    fEventInfo.fAveragedCharge =  chargeSum / numberOfPads;  
}

void ActEvent::CleanSaturatedHits(double chargeThreshold, int minDimZToDelete)
{
	if(fHitArray.size() == 0)
	{
		throw std::runtime_error("Error: CleanSaturedHits has received an empty fHitArray -> Fill ActEvent first");
	}
	//auxiliar vector to reset only filled values in chargeInPad
	std::vector<std::pair<int, int>> positionsToDelete {};
	//vector to represent pad plane
	//map to store fHitArray indixes in each pad
	std::map<std::pair<int, int>, std::vector<int>> zInPad {};
	//std::vector<std::vector<double>> chargeInPad(ActParameters::g_NPADX,
	//											 std::vector<double>(ActParameters::g_NPADY));
	for(auto& hit : fHitArray)
	{
		auto position  { hit.GetPosition()};
		chargeInPad[position.X()][position.Y()] += hit.GetCharge();
		positionsToDelete.push_back(std::make_pair(position.X(), position.Y()));
		zInPad[std::make_pair(position.X(), position.Y())].push_back(hit.GetHitID());
	}
	//vector to store hits to delete (to delete all of them together at the end)
	bool IDHasChanged { false};
	std::vector<int> indexesToDelete;
	for(int x = 0; x < ActParameters::g_NPADX; x++)
	{
		for(int y = 0; y < ActParameters::g_NPADY; y++)
		{
			if(chargeInPad[x][y] < chargeThreshold)
				continue;
			auto indexesInPadToDeletete { zInPad[std::make_pair(x, y)]};
			if(indexesInPadToDeletete.size() > minDimZToDelete)//saturation is characterized by a large spread over Z
			{
				indexesToDelete.insert(indexesToDelete.end(),
									   indexesInPadToDeletete.begin(), indexesInPadToDeletete.end());
				IDHasChanged = true;
			}
		}
	}
	if(IDHasChanged)
	{
		//std::cout<<BOLDCYAN<<"Deleting "<<indexesToDelete.size()<<" hits which are saturated!"<<RESET<<'\n';
		std::sort(indexesToDelete.begin(), indexesToDelete.end(),
				  std::greater<int>());
		//now that they are sorted in descending way, we can erase
		for(auto& i : indexesToDelete)
		{
			fHitArray.erase(fHitArray.begin() + i);
		}
		//and we should reset hitID
		for(int newID = 0; newID < fHitArray.size(); newID++)
		{
			fHitArray[newID].SetHitID(newID);
		}
	}
	//reset class vector, only for triggered values in positionsToReset
	for(auto& par : positionsToDelete)
	{
		chargeInPad[par.first][par.second] = 0.;
	}
	positionsToDelete.clear();
}

//set ActTracks from RANSAC
void ActEvent::ReadEventTracks(ActClusteringResults &results)
{
	for(auto& track : results.GetTrackCandidates())
	{
		fTracks.push_back(track);
	}
}
