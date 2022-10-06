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
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>

ActAnalyzer::ActAnalyzer(TTree* tree, std::unique_ptr<TH2D> histTrackID,
						 std::unique_ptr<TH1D> histRecoilEnergy,
						 std::unique_ptr<TH1D> histExcitation,
						 std::unique_ptr<TH2D> histKinematics,
						 std::vector<std::string> excitationKeys)
	: fTree(tree),
	  fHistTrackID(std::move(histTrackID)),
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
	//fCanvTrackID->WaitPrimitive();

	//recoil energy
	fCanvRecoilEnergy = std::make_unique<TCanvas>("fCanvRecoilEnergy", "Recoil energy", 1);
	fCanvRecoilEnergy->cd();
	fHistRecoilEnergy->Draw();
	fCanvRecoilEnergy->Update();
	//fCanvRecoilEnergy->WaitPrimitive();

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
	// fStackKinematics = std::make_unique<THStack>("fStackKinematics", "Kinematics");
	// for(auto& histo : fHistosKinematics)
	// {
	// 	histo.second->SetTitle(("Kinematics for " + histo.first).c_str());
	// 	fStackKinematics->Add(histo.second.get());
	// }
	// //theoretical ones
	// fStackTheoreticalKinematics = std::make_unique<THStack>("fStackTheoreticalKinematics", "Theoretical");
	// for(auto& histo : fHistosTheoreticalKinematics)
	// {
	// 	histo.second->SetTitle(("Theoretical for " + histo.first).c_str());
	// 	histo.second->SetMarkerColor(kBlue);
	// 	fStackTheoreticalKinematics->Add(histo.second.get());
	// }
	// fCanvKinematics->cd();
	// fStackKinematics->Draw("pads");
	// fStackTheoreticalKinematics->Draw("pads sames");
	// fCanvKinematics->Update();

	
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

double ActAnalyzer::GetGatedSiliconEnergy(TrackPhysics& track, std::string frontPanel)
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

void ActAnalyzer::ReadTree(ActSRIM& srim, ActKinematics& kinematics)
{
	fTree->SetBranchAddress("tracks", &fTracks);
	fTree->SetBranchAddress("silicons", &fSilicons);
	fTree->SetBranchAddress("triggers", &fTriggers);
	//number of entries
	long long nEntries { fTree->GetEntries()};
	for(long long i = 0; i < nEntries; i++)
	{
		fTree->GetEntry(i);
		//and here run functions
		if(fTracks->size() == 0)
			continue;//dont run funcions if we dont have tracks in event
		ProcessTrackID();
		//only BINARY events
		if(fTracks->size() != fTracksPerEvent)
			continue;
		ProcessRecoilEnergy(srim, kinematics);
	}
}

void ActAnalyzer::ProcessTrackID()
{
	for(auto& track : *fTracks)
	{
		if(!(track.fRPInChamber) || !(track.fSPInArray))//if track is not good, continue!
			continue;//this can be due to identification as delta or not having hit a silicon detector
		//get silicon place
		
		double energySi {GetGatedSiliconEnergy(track, "1")};//here we go for panel 1 if is non null!
		if(std::isnan(energySi) || energySi <= 0.)
			continue;
		auto chargeAveraged { track.fAverageCharge};

		fHistTrackID->Fill(energySi, chargeAveraged);
	}	
}

std::string ActAnalyzer::IdentifyRecoilInGraphCuts(TrackPhysics& track)
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

void ActAnalyzer::PropagateBeamInChamber(TrackPhysics& track, ActSRIM& srim,
										   ActKinematics& kinematics)
{
	//assume this as the enter point
	XYZPoint enterPoint { 0./2, 295./2, 255./2};//mm
	auto reactionPoint { track.fReactionPoint};//mm too
	auto length { std::sqrt((enterPoint - reactionPoint).Mag2())};
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
		if(track.fSiliconPlace != ActParameters::trackHitsSiliconSideLeft)
			continue;
		
		auto energyAtSilicon { GetGatedSiliconEnergy(track, "0")};//always energy at front panel 0
		//since particle ID uses also E1 for front panel, we need to extract its value
		if(std::isnan(energyAtSilicon) || energyAtSilicon <= 0.)
			continue;//skip wrong energy values or panels
		double energyAtRP {};
		double recoilMass {};
		double recoilTheta {};
		double theoRecoilEnergy {};
		//if we dont have done yet particle ID graphical cuts, default to proton
		if(!fEnableGraphicalCuts)
		{
			auto initialRange { srim.EvalDirect("p", energyAtSilicon)};
			auto rangeAtRP   { initialRange + track.fTrackLength};
			energyAtRP = srim.EvalInverse("p", rangeAtRP);
			PropagateBeamInChamber(track, srim, kinematics);
			kinematics.SetParticle("target", "p");
			kinematics.SetTargetKineticEnergy(0.);
			kinematics.SetEjectileAndRecoil("p");
			kinematics.SetEjectileKineticEnergy(energyAtRP);
			recoilTheta = track.fTheta;
			kinematics.SetEjectileAngle(track.fTheta);
			recoilMass = kinematics.GetRecoilInvariantMass();
			theoRecoilEnergy = kinematics.GetTheoreticalRecoilEnergy();
			double excitationEnergy {std::sqrt(recoilMass) - kinematics.GetMass("recoil")};
			if(isInExcitationMap("p"))
			{
				fHistosExcitation["p"]->Fill(excitationEnergy);
				fHistosKinematics["p"]->Fill(recoilTheta, energyAtRP);
				fHistosTheoreticalKinematics["p"]->Fill(recoilTheta, theoRecoilEnergy);
			}
			else
			{
				std::cout<<BOLDRED<<"Could not find fHistosExcitation associated to particle p -> Check excitationKeys!"<<RESET<<'\n';
			}
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
			kinematics.SetParticle("target", particle);
			kinematics.SetTargetKineticEnergy(0.);
			kinematics.SetEjectileAndRecoil(particle);
			kinematics.SetEjectileKineticEnergy(energyAtRP);
			recoilTheta = track.fTheta;
			kinematics.SetEjectileAngle(track.fTheta);
			recoilMass = kinematics.GetRecoilInvariantMass();
			theoRecoilEnergy = kinematics.GetTheoreticalRecoilEnergy();
			double excitationEnergy {std::sqrt(recoilMass) - kinematics.GetMass("recoil")};
			//std::cout<<"Excitation energy with "<<particle<<" :"<<excitationEnergy<<'\n';
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
