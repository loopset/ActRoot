#include "ActParameters.h"
#include "ActHit.h"
#include "ActTrack.h"
#include "ActClusteringResults.h"
#include "ActLine.h"
#include "ActRANSAC.h"
#include "ActDraw.h"

#include <Math/Point3D.h>
#include <Math/Point3Dfwd.h>
#include <Math/Vector3D.h>
#include <Math/Vector3Dfwd.h>
#include <RtypesCore.h>
#include <TString.h>
#include <fstream>
#include <iostream>
#include <map>
#include <vector>

using XYZPoint = ROOT::Math::XYZPoint;

void MacroRANSAC()
{
	TString path {"/media/Datos/ApuntesUSC/TESE/ACTAR_tracking/analysis_2022/ACTAR_ANALYSIS_LIGHT_root6/root/arrays_of_E796_events/"};

	int max_event {10};

	//drawing structure
	ActDraw painter{};
	painter.Init();
	for(int i = 0; i < max_event; i++)
	{
		TString event { TString::Format("event_%d.dat", i) };
		std::cout<<"Reading "<<event<<'\n';
		std::ifstream infile((path + event).Data());
		std::vector<ActHit> eventVector{};
		Double_t x, y, z, q;
		while(infile >> x >> y >> z >> q)
		{
			;
			ActHit hit = ActHit(-1, XYZPoint(x, y, z), q);
			eventVector.push_back(hit);
		}
		

		SampleConsensus::ActRANSAC estimator{500, 20, 10.};
		estimator.SetSampleMethod(2);//0->uniform(default), 1->simple gaussian, 2->charge weighted
		//estimator.SetSampleWithReplacement(false);
		estimator.SetChargeThreshold(100.);
		//std::cout<<eventVector.size()<<'\n';
		auto out = estimator.Solve(eventVector);

		for(auto& track : out.GetTrackCandidates())
		{
			std::cout<<"Cluster id: "<<track.GetTrackID()<<" with Chi2: "<<track.GetLine().GetChi2()<<'\n';
		}

		//drawing
		//painter.DrawEvent(eventVector);
		painter.DrawResults(eventVector, out);
		//painter.DrawResults3D(eventVector, out);
		eventVector.clear();

	}
	
}
