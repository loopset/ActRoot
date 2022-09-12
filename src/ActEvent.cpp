#include "ActEvent.h"

#include "ActHit.h"
#include "ActParameters.h"
#include "ActCalibrations.h"

#include "../cobo_libs/inc/MEventReduced.h"
#include <algorithm>
#include <vector>

//struct definitions
Silicons::Silicons()
	: fSi0(ActParameters::NrowsSi01, 0.), fSi0_cal(ActParameters::NrowsSi01, 0.),
	  fSi1(ActParameters::NrowsSi01, 0.), fSi1_cal(ActParameters::NrowsSi01, 0.),
	  fSiS(ActParameters::NrowsSiS, 0.),  fSiS_cal(ActParameters::NrowsSiS, 0.)
{
}

void ActEvent::ReadEvent(const ActCalibrations &calibrations, const MEventReduced *EvtRed)
{
	auto TABLE { calibrations.GetTABLE()};
	auto PadAlignCoefs { calibrations.GetPadAlignCoefs()};
	// std::vector<std::vector<std::vector<double>>> voxel(ActParameters::NPADX,
	// 													std::vector<std::vector<double>>(ActParameters::NPADY,
	// 																					 std::vector<double>(ActParameters::NPADZ, 0.)));
	
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

		if((co != 31) && (co != 16))
		{
			for(int hit = 0; hit < EvtRed->CoboAsad[it].peakheight.size(); hit++)
			{
				if(TABLE[5][where] != -1)
				{
					double z_position { EvtRed->CoboAsad[it].peaktime[hit]};
					if(z_position > 0.)
					{
						auto Qiaux { EvtRed->CoboAsad[it].peakheight[hit]};
						auto Qiaux_align { PadAlignCoefs[where][0] + PadAlignCoefs[where][1] * Qiaux
						 				   + PadAlignCoefs[where][2] * Qiaux * Qiaux};
						double xval { static_cast<double>(TABLE[4][where])};
						double yval { static_cast<double>(TABLE[5][where])};
						//std::cout<<" X: "<<xval<<" Y: "<<yval<< " Z: "<<z_position<<'\n';
						//std::cout<<"Q: "<<Qiaux<<" Q_aligned: "<<Qiaux_align<<'\n';

						// voxel[xval][yval][z_position] += Qiaux_align;
						// std::cout<<"VOXEL filled"<<'\n';
						// for(int x = 0; x < ActParameters::NPADX; x++)
						// {
						// 	for(int y = 0; y < ActParameters::NPADY; y++)
						// 	{
						// 		for(int z = 0; z < ActParameters::NPADZ; z++)
						// 		{
						// 			if(voxel[x][y][z] <= 0.) continue;
						// 			ActHit candidate { hitID, XYZPoint(x, y, z), voxel[x][y][z]};
						// 			fHitArray.push_back(candidate);
						// 			hitID++;
						// 		}
						// 	}
						// }
						// ActHit candidate { hitID, XYZPoint(xval, yval, z_position), Qiaux_align};
						// auto isInVectorLambda = [&candidate](const ActHit& h) -> bool
						// {return ((h.GetPosition().X() == candidate.GetPosition().X())
						// 		 && (h.GetPosition().Y() == candidate.GetPosition().Y())
						// 		 && (h.GetPosition().Z() == candidate.GetPosition().Z()));};
						// auto itFound { std::find_if(fHitArray.begin(), fHitArray.end(), isInVectorLambda)};
						// auto indexFound { itFound - fHitArray.begin()};
						// //if ActHit (by POSITION) is already in fHitArray,
						// //add charge and dont update ID (intended to work with rebinning)
						// if( itFound != fHitArray.end())
						// {
						// 	fHitArray[indexFound].SetCharge(fHitArray[indexFound].GetCharge()
						// 									+ Qiaux_align);
						// 	//do not increment hitID
						// }
						// else
						// {
						// 	fHitArray.push_back(candidate);
						// 	hitID++;
						// }

						//VERSION WITHOUT taking into account posible hit with lower charge
						ActHit candidate { hitID, XYZPoint(xval, yval, z_position), Qiaux_align};
						fHitArray.push_back(candidate);
						hitID++;
					}
				}
			}
		}
	}
}

