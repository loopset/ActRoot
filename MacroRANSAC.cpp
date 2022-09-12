#include "ActParameters.h"
#include "ActHit.h"
#include "ActTrack.h"
#include "ActClusteringResults.h"
#include "ActLine.h"
#include "ActSample.h"
#include "ActRANSAC.h"
#include "ActDraw.h"

#include <Math/Point3D.h>
#include <Math/Point3Dfwd.h>
#include <Math/Vector3D.h>
#include <Math/Vector3Dfwd.h>
#include <RtypesCore.h>
#include <TString.h>
#include <algorithm>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <map>
#include <stdexcept>
#include <vector>
#include <memory>

using XYZPoint = ROOT::Math::XYZPoint;

void MacroRANSAC()
{
	TString path {"/media/Datos/ApuntesUSC/TESE/ACTAR_tracking/analysis_2022/ACTAR_ANALYSIS_LIGHT_root6/root/arrays_of_E796_events/"};
	std::ifstream elastic_event((path + TString("index.dat")).Data());
	std::vector<int> elastic_event_index;
	int aux_index;
	while (elastic_event >> aux_index){elastic_event_index.push_back(aux_index);}

	int min_event{ 13};
	int max_event {14};
	//drawing structure
	ActDraw painter{};
	//painter.SetMaxZ(512./4);
	//painter.SetNbinsZ(static_cast<int>(512 / 4));
	painter.Init();

	//iterate over events
	//for(int i = min_event; i < max_event; i++)
	for(auto& i : elastic_event_index)
	{
		TString event { TString::Format("event_%d.dat", i) };
		std::cout<<"Reading "<<event<<'\n';
		std::ifstream infile((path + event).Data());
		std::vector<ActHit> eventVector{};
		std::vector<ActHit> allVector{};
		Double_t x, y, z, q;
		double maxZ {0.};
		while(infile >> x >> y >> z >> q)
		{
			if(z > maxZ)  maxZ = z;
			ActHit hit = ActHit(-1, XYZPoint(x, y, z), q);
			allVector.push_back(hit);
			//avoid beam region
			if((y <= 90)) continue;//in pad units
			eventVector.push_back(hit);
		}
		//std::cout<<"Max Z value: "<<maxZ<<'\n';
		//lets try to sort by Z value
		std::sort(eventVector.begin(), eventVector.end(), [](const ActHit& lhs, const ActHit& rhs){return lhs.GetPosition().Z() < rhs.GetPosition().Z();});
		//std::cout<<"First Z value: "<<eventVector[0].GetPosition().Z()<<'\n';
		//and now RANSAC estimator
		SampleConsensus::ActRANSAC estimator{500, 15, 4.};
		//customize sampling method
		auto method { RandomSampling::SamplingMethod::kGaussian};
		estimator.SetSampleMethod(method);
		//estimator.SetSampleWithReplacement(true);
		//estimator.SetChargeThreshold(200.);
		//estimator.SetFitPattern(false);
		//std::cout<<eventVector.size()<<'\n';
		try
		{
			auto out = estimator.Solve(eventVector);
			for(auto& track : out.GetTrackCandidates())
			{
				std::cout<<"Cluster id: "<<track.GetTrackID() + 1<<" with Chi2: "<<track.GetLine().GetChi2()<<'\n';
				//std::cout<<"X: "<<track.GetLine().GetDirection().X()<<" Y: "<<track.GetLine().GetDirection().Y()<<" Z:"<<track.GetLine().GetDirection().Z()<<'\n';
			}

			//drawing
			//painter.DrawEvent(eventVector);
			painter.DrawResults(allVector, out);
			//painter.DrawResults3D(eventVector, out);

			//building silhouette score
			// auto results = out.GetTrackCandidates();
			// for(int j = 0; j < (int)results.size(); j++)
			// {
			// 	std::vector<ActTrack> drop_current_track(results);
			// 	drop_current_track.erase(drop_current_track.begin() + j);

			// 	//hit array
			// 	auto hits_in_trak = results[j].GetHitArray();
			// 	for(int k = 0; k < (int)hits_in_trak.size(); k++)
			// 	{
			// 		std::vector<ActHit> drop_current_hit(hits_in_trak);
			// 		drop_current_hit.erase(drop_current_hit.begin() + k);
					
			// 	}
				
			// }
			
		}
		catch(std::runtime_error& e)
		{
			std::cout<<"Error in event"<<i<<'\n'<<e.what()<<'\n';
			continue;
		}

		//try to find
		auto eventToFind { eventVector[0]};
		auto sth = std::find_if(eventVector.begin(), eventVector.end(),
					 [&eventToFind](const ActHit& hit) -> bool {return ((eventToFind.GetPosition().X() == hit.GetPosition().X())
															   && (eventToFind.GetPosition().Y() == hit.GetPosition().Y())
															   && (eventToFind.GetPosition().Z() == hit.GetPosition().Z()));});
		ActHit ret { *sth};
		std::cout<<"Found: "<<ret.GetHitID()<<'\n';
		//test random sampling
		// RandomSampling::ActSample sampler;
		// sampler.SetSampleMehtod(3);
		// sampler.SetHitsToSample(&eventVector);
		// sampler.SetSampleWithReplacement(false);
		// auto samples = sampler.SampleHits(2);
		// std::cout<<"Samples 1: "<<samples[0].GetPosition().X()<<" with 2: "<<samples[1].GetPosition().X()<<'\n';
		eventVector.clear();

	}
	
}
