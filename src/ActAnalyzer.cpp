#include "ActAnalyzer.h"

#include "ActKinematics.h"
#include "ActParameters.h"
#include "ActSRIM.h"
#include "ActStructs.h"

#include <TCutG.h>
#include <Math/Point3Dfwd.h>
#include <Math/Point3D.h>
#include <Math/Vector3Dfwd.h>
#include <Math/Vector3D.h>
#include <Rtypes.h>
#include <TH1.h>
#include <THStack.h>
#include <TROOT.h>
#include <TCanvas.h>
#include <TH2.h>
#include <TCutG.h>
#include <TStyle.h>
#include <TFile.h>

#include <TUUID.h>
#include <algorithm>
#include <cmath>
#include <fstream>
#include <ios>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>

ActAnalyzer::ActAnalyzer(std::unique_ptr<TH2D> histTrackID,
						 std::unique_ptr<TH2D> histWall,
						 std::unique_ptr<TH1D> histRecoilEnergy,
						 std::unique_ptr<TH1D> histExcitation,
						 std::unique_ptr<TH2D> histKinematics,
						 std::vector<std::string> excitationKeys)
:	  fHistTrackID(std::move(histTrackID)),
	  fHistWall(std::move(histWall)),
	  fHistRecoilEnergy(std::move(histRecoilEnergy))
{
	for(auto& key : excitationKeys)
	{
		fHistosExcitation[key] = std::unique_ptr<TH1D>((TH1D*)histExcitation->Clone());
		fHistosKinematics[key] = std::unique_ptr<TH2D>((TH2D*)histKinematics->Clone());
		fHistosTheoreticalKinematics[key] = std::unique_ptr<TH2D>((TH2D*)histKinematics->Clone());
	}
}

void ActAnalyzer::ReadGraphicalCuts(std::string key, std::string fileName)
{
	if(!fEnableGraphicalCuts)
	{
		std::cout<<BOLDMAGENTA<<"Warning: skip reading TCutG files because fEnableGraphicalCuts is set to false!"<<RESET<<'\n';
		return;
	}
	auto file = std::make_unique<TFile>(fileName.c_str(), "read");
	fGCuts[key] = std::unique_ptr<TCutG>((TCutG*)file->Get("CUTG"));//name is predefined of ROOT
	if(fGCuts[key] == nullptr)
	{
		throw std::runtime_error(("Error loading TCutG from " + key + " file").c_str());
	}
}

void ActAnalyzer::ReadWallCuts(std::string key, std::string fileName)
{
	if(!fEnableWallCuts)
	{
		std::cout<<BOLDMAGENTA<<"Warning: skip reading TCutG files because fEnableWallCuts is set to false!"<<RESET<<'\n';
		return;
	}
	auto file = std::make_unique<TFile>(fileName.c_str(), "read");
	fWallCuts[key] = std::unique_ptr<TCutG>((TCutG*)file->Get("CUTG"));//name is predefined of ROOT
	if(fWallCuts[key] == nullptr)
	{
		throw std::runtime_error(("Error loading TCutG from " + key + " file").c_str());
	}
}

void ActAnalyzer::ReadAuxiliarCut(std::string fileName)
{
	if(!fEnableAuxiliarCut)
	{
		std::cout<<BOLDMAGENTA<<"Warning: skip reading TCutG files because fEnableAuxiliarCut is set to false!"<<RESET<<'\n';
		return;
	}
	auto file = std::make_unique<TFile>(fileName.c_str(), "read");
	fAuxiliarCut = std::unique_ptr<TCutG>((TCutG*)file->Get("CUTG"));//name is predefined of ROOT
	if(fAuxiliarCut == nullptr)
	{
		throw std::runtime_error(("Error loading TCutG from " + fileName).c_str());
	}
}

void ActAnalyzer::SetSkipParticles(std::vector<std::string> vec)
{
	fSkipParticles = vec;
	for(auto& particle : fSkipParticles)
	{
		std::cout<<BOLDGREEN<<"Excluding particle "<<particle<<" from analysis"<<RESET<<'\n';
	}
}
void ActAnalyzer::DrawCanvas()
{
	//particle ID
	fCanvTrackID = std::make_unique<TCanvas>("fCanvTrackID", "Track ID ", 1);
	fCanvTrackID->cd();
	fHistTrackID->Draw("colz");
	if(fEnableGraphicalCuts)
	{
		for(auto& cut : fGCuts)
		{
			cut.second->Draw("same");
		}
	}
	fCanvTrackID->Update();

	//silicon wall
	fCanvWall = std::make_unique<TCanvas>("fCanvWall", "Silicon wall", 1);
	fCanvWall->cd();
	fHistWall->Draw("colz");
	fCanvWall->Update();

	//recoil energy
	fCanvRecoilEnergy = std::make_unique<TCanvas>("fCanvRecoilEnergy", "Recoil energy", 1);
	fCanvRecoilEnergy->cd();
	fHistRecoilEnergy->Draw();
	fCanvRecoilEnergy->Update();

	//kinematics plot
	fCanvKinematics = std::make_unique<TCanvas>("fCanvKinematics", "Kinematics plot", 1);
	fCanvKinematics->Divide(fHistosKinematics.size());
	int padIndex { 1};
	for(auto& kinHisto : fHistosKinematics)
	{
		fCanvKinematics->cd(padIndex);
		kinHisto.second->SetTitle(("Kinematics for " + kinHisto.first).c_str());
		kinHisto.second->Draw();
		fHistosTheoreticalKinematics[kinHisto.first]->SetTitle(("Theoretical for " + kinHisto.first).c_str());
		fHistosTheoreticalKinematics[kinHisto.first]->SetMarkerColor(kBlue);
		fHistosTheoreticalKinematics[kinHisto.first]->Draw("same");
		padIndex++;
	}
	fCanvKinematics->cd();
	fCanvKinematics->Update();
	
	//excitation energy
	fCanvExcitation = std::make_unique<TCanvas>("fCanvExcitation", "Excitation energy", 1);
	fStackExcitation = std::make_unique<THStack>("fStackExcitation", "Excitation energies");
	for(auto& histo : fHistosExcitation)
	{
		histo.second->SetTitle(("Elastic with " + histo.first).c_str());
		fStackExcitation->Add(histo.second.get());
	}
	fStackExcitation->Draw("pads");
	fCanvExcitation->Update();
	fCanvExcitation->cd();
	fCanvExcitation->WaitPrimitive("lat", "");
	fStackExcitation.reset();
	fStackKinematics.reset();
}

double ActAnalyzer::GetGatedSiliconEnergy(const TrackPhysics& track, std::string frontPanel)
{
	auto siliconSide { track.fSiliconPlace};
	if(siliconSide == ActParameters::trackHitsSiliconSideLeft)//side_left
	{
		if(fSilicons->fData["S"]["M"] > 1)
		{//skip multiplicity over 1
			return std::nan("-999");
		}
		return fSilicons->fData["S"]["ES"];
	}
	else if(siliconSide == ActParameters::trackHitsSiliconFront)//front
	{
		if(fSilicons->fData["01F"]["M"] > 1)
			return std::nan("-999");
		auto energySi0 { fSilicons->fData["01F"]["E0"]};
		auto energySi1 { fSilicons->fData["01F"]["E1"]};
		//select which energy return
		if(frontPanel == "0")
			return energySi0;
		else if(frontPanel == "1")
			return (energySi1 > 0.) ? energySi1 : energySi0;//if energySi1 is neg or nul, return Si0
		else
		{
			throw std::runtime_error("Wrong front panel selected! Values must be 0 or 1");
		}
	}
	else//track doesnt hit silicons
	{
		return std::nan("-999");
	}
}


void ActAnalyzer::ProcessTrackID()
{
	for(auto& track : *fTracks)
	{
		if(!(track.fRPInChamber) || !(track.fSPInArray))//if track is not good, continue!
			continue;//this can be due to identification as delta or not having hit a silicon detector
		//determine analysis mode according to fSiliconMode
		std::string mode {};
		if(fSiliconMode == "side")
			mode = ActParameters::trackHitsSiliconSideLeft;
		else if(fSiliconMode == "front")
			mode = ActParameters::trackHitsSiliconFront;
		else
		{
			throw std::runtime_error("Wrong fSiliconMode passed to ActAnalyzer -> Available options are side and front");
		}
		if(track.fSiliconPlace != mode)
			continue;

		// //manage cuts on silicon wall
		bool skipFill {false};
		if(fEnableWallCuts)
		{
			if(fSiliconMode == "side")
			{
				;//no cuts here
			}
			if(fSiliconMode == "front")
			{
				for(auto& cut : fWallCuts)
				{
					if(cut.second->IsInside(track.fSiliconPoint.Y(), track.fSiliconPoint.Z()))
					{
						skipFill = true;
						break;
					}
				}
			}
		}
		//check if we need to skip track
		if(!skipFill && fSiliconMode == "front")
			continue;

		double energySi {GetGatedSiliconEnergy(track, "1")};//here we go for panel 1 if is non null!
		if(std::isnan(energySi) || energySi <= 0.)
			continue;
		auto chargeAveraged { track.fAverageCharge};
		//fill wall
		if(fSiliconMode == "side")
		{
			fHistWall->Fill(track.fSiliconPoint.X(), track.fSiliconPoint.Z());
		}
		else if(fSiliconMode == "front")
		{
			fHistWall->Fill(track.fSiliconPoint.Y(), track.fSiliconPoint.Z());
		}
		else
		{
			throw std::runtime_error("Wrong fSiliconMode passed to ActAnalyzer -> Available options are side and front");
		}

		//fill charge ID
		fHistTrackID->Fill(energySi, chargeAveraged);

		if(fEnableAuxiliarCut && fAuxiliarCut->IsInside(energySi, chargeAveraged))
			fWriteToStreamer = true;
	}	
}

std::string ActAnalyzer::IdentifyRecoilInGraphCuts(const TrackPhysics& track)
{
	auto energyAtSiliconForID { GetGatedSiliconEnergy(track, "1")};
	auto averageCharge { track.fAverageCharge};
	std::string identifiedCut {"none"};//default value if not identified (likely at punchthrough)
	for(auto& cut : fGCuts)
	{
		//WARNING:: TCutGs must be mutually exclusive
		if(cut.second->IsInside(energyAtSiliconForID, averageCharge))
		{
			identifiedCut = cut.first;
			break;
		}
	}
	return identifiedCut;
}

void ActAnalyzer::PropagateBeamInChamber(const TrackPhysics& track, ActSRIM& srim,
										   ActKinematics& kinematics)
{
	//assume this as the enter point
	XYZPoint enterPoint { 0./2, 295./2, 255./2};//mm
	auto reactionPoint { track.fReactionPoint};//mm too
	auto length { std::sqrt((enterPoint - reactionPoint).Mag2())};
	//reset beam kinematics to original state
	kinematics.ResetBeamEnergy();
	auto initialRange { srim.EvalDirect(kinematics.GetBeamParticle(), kinematics.GetBeamKineticEnergy())};
	auto rangeAtRP { initialRange - length};
	if(rangeAtRP <= 0.)
		return;//something went wrong, should check! but by now we dont modify beam energy
	auto energyAtRP { srim.EvalInverse(kinematics.GetBeamParticle(), rangeAtRP)};

	//and now set value
	kinematics.SetBeamKineticEnergy(energyAtRP);
}

void ActAnalyzer::ProcessRecoilEnergy(ActSRIM& srim, ActKinematics& kinematics)
{
	//use SRIM to get energy just after reaction point!
	for(auto& track : *fTracks)
	{
		if(!(track.fRPInChamber) || !(track.fSPInArray))//if track is not good, continue!
			continue;
		//determine analysis mode according to fSiliconMode
		std::string mode {};
		if(fSiliconMode == "side")
			mode = ActParameters::trackHitsSiliconSideLeft;
		else if(fSiliconMode == "front")
			mode = ActParameters::trackHitsSiliconFront;
		else
		{
			throw std::runtime_error("Wrong fSiliconMode passed to ActAnalyzer -> Available options are side and front");
		}
		if(track.fSiliconPlace != mode)
			continue;
		
		auto energyAtSilicon { GetGatedSiliconEnergy(track, "0")};//always energy at front panel 0
		//since particle ID uses also E1 for front panel, we need to extract its value
		if(std::isnan(energyAtSilicon) || energyAtSilicon <= 0.)
			continue;//skip wrong energy values or panels
		//track.Print();
		double energyAtRP {};
		double recoilMass {};
		double recoilTheta {};
		double theoRecoilEnergy {};
		//if we dont have done yet particle ID graphical cuts, default to proton
		if(!fEnableGraphicalCuts)
		{
			continue;
			// auto initialRange { srim.EvalDirect("p", energyAtSilicon)};
			// auto rangeAtRP   { initialRange + track.fTrackLength};
			// energyAtRP = srim.EvalInverse("p", rangeAtRP);
			// PropagateBeamInChamber(track, srim, kinematics);
			// kinematics.SetParticle("target", "p");
			// kinematics.SetTargetKineticEnergy(0.);
			// kinematics.SetEjectileAndRecoil("p");
			// kinematics.SetEjectileKineticEnergy(energyAtRP);
			// recoilTheta = track.fTheta;
			// kinematics.SetEjectileAngle(track.fTheta);
			// recoilMass = kinematics.GetRecoilInvariantMass();
			// theoRecoilEnergy = kinematics.GetTheoreticalRecoilEnergy();
			// double excitationEnergy {std::sqrt(recoilMass) - kinematics.GetMass("recoil")};
			// if(isInExcitationMap("p"))
			// {
			// 	fHistosExcitation["p"]->Fill(excitationEnergy);
			// 	fHistosKinematics["p"]->Fill(recoilTheta, energyAtRP);
			// 	fHistosTheoreticalKinematics["p"]->Fill(recoilTheta, theoRecoilEnergy);
			// }
			// else
			// {
			// 	std::cout<<BOLDRED<<"Could not find fHistosExcitation associated to particle p -> Check excitationKeys!"<<RESET<<'\n';
			// }
		}
		else
		{
			auto particle { IdentifyRecoilInGraphCuts(track)};
			if(particle == "none" || isInVector(particle, fSkipParticles))//wasnt inside TCutG or custom skip selection
				continue;
			auto initialRange { srim.EvalDirect(particle, energyAtSilicon)};
			auto rangeAtRP   { initialRange + track.fTrackLength};
			energyAtRP = srim.EvalInverse(particle, rangeAtRP);
			PropagateBeamInChamber(track, srim, kinematics);
			kinematics.SetEjectileAndRecoil(particle);
			kinematics.SetTargetKineticEnergy(0.);
			kinematics.SetEjectileKineticEnergy(energyAtRP);
			recoilTheta = track.fTheta;
			kinematics.SetEjectileAngle(track.fTheta);
			recoilMass = kinematics.GetRecoilInvariantMass();
			theoRecoilEnergy = kinematics.GetTheoreticalRecoilEnergy();
			double excitationEnergy {std::sqrt(recoilMass) - kinematics.GetMass("recoil")};
			//std::cout<<"Theoretical recoil energy "<<particle<<" :"<<theoRecoilEnergy<<'\n';
			if(isInExcitationMap(particle))
			{
				fHistosExcitation[particle]->Fill(excitationEnergy);
				fHistosKinematics[particle]->Fill(recoilTheta, energyAtRP);
				fHistosTheoreticalKinematics[particle]->Fill(recoilTheta, theoRecoilEnergy);
			}
			else
			{
				std::cout<<BOLDRED<<"Could not find fHistosExcitation associated to particle "<<particle<<" -> Check excitationKeys!"<<RESET<<'\n';
			}
		}
		//std::cout<<"Energy: "<<energyAtRP<<" with angle: "<<track.fTheta<<'\n';
		fHistRecoilEnergy->Fill(energyAtRP);
	}
}

void ActAnalyzer::ReadTree(ActSRIM& srim, ActKinematics& kinematics)
{
	if(!fTree)
	{
		std::cout<<BOLDRED<<"fTree does not point to any valid TTree -> Set it correctly"<<RESET<<'\n';
		return;
	}
	if(fSiliconMode == "front")
		fTree->SetBranchAddress("runID", &fRunID);
	fTree->SetBranchAddress("eventID", &fEventID);
	fTree->SetBranchAddress("tracks", &fTracks);
	fTree->SetBranchAddress("silicons", &fSilicons);
	fTree->SetBranchAddress("triggers", &fTriggers);

	//streamer for auxiliar cut
	std::ofstream streamer {};
	if(fEnableAuxiliarCut)
	{
		streamer.open("./FrontIndexesPunchThrough.dat", std::ios_base::app);
	}
	//number of entries
	long long nEntries { fTree->GetEntries()};
	for(long long i = 0; i < nEntries; i++)
	{
		//reset fWriteToStreamer
		fWriteToStreamer = false;
		//read entry
		fTree->GetEntry(i);
		//and here run functions
		if(fTracks->size() == 0)
			continue;//dont run funcions if we dont have tracks in event
		ProcessTrackID();
		if(fEnableAuxiliarCut && fWriteToStreamer)
		{
			streamer<<fRunID<<" "<<fEventID<<" "<<i<<'\n';
		}
		//only BINARY events
		if(fTracks->size() != fTracksPerEvent)
			continue;
		ProcessRecoilEnergy(srim, kinematics);
	}

	streamer.close();
}
