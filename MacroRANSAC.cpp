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

	int max_event {20};

	//drawing structure
	ActDraw painter{};
	painter.Init();

	//iterate over events
	for(int i = 0; i < max_event; i++)
	{
		TString event { TString::Format("event_%d.dat", i) };
		std::cout<<"Reading "<<event<<'\n';
		std::ifstream infile((path + event).Data());
		std::vector<ActHit> eventVector{};
		Double_t x, y, z, q;
		while(infile >> x >> y >> z >> q)
		{
			//avoi beam region
			if((y >= 50) && (y <= 70)) continue;//in pad units
			ActHit hit = ActHit(-1, XYZPoint(x, y, z), q);
			eventVector.push_back(hit);
		}
		
		//and now RANSAC estimator
		SampleConsensus::ActRANSAC estimator{500, 10, 4.};
		//customize sampling method
		auto method { RandomSampling::SamplingMethod::kGaussian};
		estimator.SetSampleMethod(method);
		//estimator.SetSampleWithReplacement(true);
		estimator.SetChargeThreshold(200.);
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
			painter.DrawResults(eventVector, out);
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
