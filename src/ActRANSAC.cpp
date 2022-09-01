#include "ActRANSAC.h"

#include "ActHit.h"
#include "ActLine.h"
#include "ActTrack.h"
#include "ActClusteringResults.h"

#include <Math/PdfFuncMathCore.h>
#include <TRandom.h>
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <iterator>
#include <vector>
#include <set>
#include <stdexcept>


SampleConsensus::ActRANSAC::ActRANSAC(int iteractions, int minPoints, float distanceThreshold)
	: fIterations(iteractions), fMinPatternPoints(minPoints), fDistanceThreshold(distanceThreshold)
{
	
}

ActClusteringResults SampleConsensus::ActRANSAC::Solve(const std::vector<ActHit>& hitArray)
{
	if(hitArray.size() < fMinPatternPoints)
	{
		std::cout<<"ERROR: Lower hits than minimum expected"<<std::endl;
		std::abort();
	}
	//follow ATTPROOT for sorting
	auto compareLines = [](const ActLine& a, const ActLine& b) { return a.GetChi2() < b.GetChi2(); };//lambda functor
	auto sortedLines  = std::set<ActLine, decltype(compareLines)>(compareLines);
	
	for (int i = 0; i < fIterations; i++)//RANSAC ITERATIONS
	{
		//1st we sample two points and we find a line from them
		auto line = SampleHits(hitArray);
		//2nd we evaluate RANSAC: inliers and Chi2
		auto inliers = EvaluateRANSAC(hitArray, line);
		//3rd: if more inliers than minimum, validate line
		if(inliers > fMinPatternPoints)
			sortedLines.insert(std::move(line));
	}

	//and now we extract the points that fit each line
	auto remainHits = hitArray;
	ActClusteringResults clusters;
	for(const auto& line : sortedLines)
	{
		if(remainHits.size() < fMinPatternPoints)
			break;
		auto inlierHits = RankLines(line, remainHits);
		if(inlierHits.size() > fMinPatternPoints)
		{
			//elements in std::set are const
			//we new auxiliary ActLine to modify it!
			ActLine auxLine = line;
			auto track = CreateTrack(auxLine, inlierHits);
			track.SetTrackID(clusters.GetTrackCandidates().size());
			clusters.AddTrack(track);
		}
	}


	//other hits as noise
	for(auto& hit : remainHits)
	{
		clusters.AddNoise(std::move(hit));
	}
	return clusters;
}

double SampleConsensus::ActRANSAC::GetPDF(const ActHit& hit, const ActHit& referenceHit)
{
	if(fSampleMethod == 0)//Uniform, doesnt need call to this method!
		return {};
	else if(fSampleMethod == 1)//Simple gaussian from reference point
	{
		auto dist = (referenceHit.GetPosition() - hit.GetPosition()).Mag2();
		dist      = std::sqrt(dist);
		return ROOT::Math::gaussian_pdf(dist, fGaussianSigma);
	}
	else { throw std::runtime_error("Sampling method not yet implemented or wrong int!"); }
}

ActLine SampleConsensus::ActRANSAC::SampleHits(const std::vector<ActHit>& hitArray)
{
	//std::cout<<"seed: "<<gRandom->GetSeed()<<'\n';
	std::vector<int> ind;
	std::vector<XYZPoint> sampled;
	if(fSampleMethod == 0)//Uniform
	{
		while (ind.size() < fSamplePoints)
		{
			int i = gRandom->Uniform() * hitArray.size();
			if(fSampleWithReplacement || !isInVector(i, ind))
			{
				ind.push_back(i);
				sampled.push_back(hitArray.at(i).GetPosition());
			}
		}
	}
	else if(fSampleMethod == 1)//Simple gaussian
	{
		//1st, set reference point and add it to sampled
		int refIndex = gRandom->Uniform() * hitArray.size();
		auto refHit   = hitArray.at(refIndex);
		ind.push_back(refIndex); sampled.push_back(refHit.GetPosition());
		//2nd, iterate and push back according to criteria
		while(ind.size() < fSamplePoints)
		{
			int i = gRandom->Uniform() * hitArray.size();
			auto hit = hitArray.at(i); 
			auto gauss = GetPDF(hit, refHit);
			auto r     = gRandom->Uniform();
			//we are sampling the gaussiang following the _truncated_ procedure:
			// an auxiliary random [0,1) is needed to compare with the gaussian pdf
			if(( gauss >= r) && ( fSampleWithReplacement || !isInVector(i, ind) ))
			{
				ind.push_back(i);
				sampled.push_back(hit.GetPosition());
			}
		}
	}
	else if(fSampleMethod == 2)//Charge weighted sampling
	{
		//1st, we fill weight vector
		if(!fIsWeightVectorConstructed)
		{
			double totalCharge { 0.};
			//get total charge
			for(auto& el : hitArray) totalCharge += el.GetCharge();
			//normalize weights to total charge
			for(auto& el : hitArray)fWeightVector.push_back( el.GetCharge() / totalCharge );
			//mark this operation as done, only once
			fIsWeightVectorConstructed = true;
		}
		//1st, set reference point and add it to sampled
		int refIndex = gRandom->Uniform() * hitArray.size();
		auto refHit   = hitArray.at(refIndex);
		ind.push_back(refIndex); sampled.push_back(refHit.GetPosition());
		//2nd, iterate and push back according to criteria
		while(ind.size() < fSamplePoints)
		{
			int i = gRandom->Uniform() * hitArray.size();
			auto hit = hitArray.at(i);
			//maximum of fWeightVector to sample!
			// idk why jczamorac does it using GetAvCharge, when it should be the maximum of the pdf
			auto maxWeightPDF = std::max_element(std::begin(fWeightVector), std::end(fWeightVector));
			//generate random number between (0, maxWeightPDF)
			auto r = gRandom->Uniform(0., *maxWeightPDF);
			//and now check if our sampled PDF is greater than r random value
			if(( fWeightVector[i] >= r ) && ( fSampleWithReplacement || !isInVector(i, ind) ) )
			{
				ind.push_back(i);
				sampled.push_back(hit.GetPosition());
			}
		}
	}
	else { throw std::runtime_error("Sampling method not yet implemented or wrong int!"); }

	auto sampledLine = ActLine(sampled);
	return sampledLine;
}

int SampleConsensus::ActRANSAC::EvaluateRANSAC(const std::vector<ActHit>& hitArray, ActLine& sampledLine)
{
	int nbInliers {0};
	for(const auto& hit : hitArray)
	{
		auto& pos = hit.GetPosition();
		double error = sampledLine.DistanceLineToHit(pos);
		error = error * error;
		if(error < (fDistanceThreshold * fDistanceThreshold))
		{
			nbInliers++;
		}
	}
	//set Chi2
	sampledLine.SetChi2(1.0 / nbInliers);
	return nbInliers;
}

std::vector<ActHit> SampleConsensus::ActRANSAC::RankLines(const ActLine& line, std::vector<ActHit>& remainHits)
{

	// std::vector<ActHit> inliersHits;
	// std::vector<ActHit> remainHits;

	// for(const auto& hit : hits)
	// {
	// 	double error = line.DistanceLineToHit(hit.GetPosition());
	// 	auto isInLine = (error * error) < (fDistanceThreshold * fDistanceThreshold);

	// 	if(isInLine)
	// 		inliersHits.push_back(hit);
	// }
	// std::set_difference(hits.begin(), hits.end(), inliersHits.begin(), inliersHits.end(),
	// 					std::inserter(remainHits, remainHits.begin()));
	// //output
	// std::vector<std::vector<ActHit>> retVec { inliersHits, remainHits};
	// return retVec;
	//ATTPC VERSION; ours cannot work bc c++ doesn't know how to compare ActHits
	std::vector<ActHit> retVec;
	auto itStartEqualRange = remainHits.end();
	
	for(auto it = remainHits.begin(); it != remainHits.end(); ++it)
	{
		double error = line.DistanceLineToHit(it->GetPosition());
		auto isInLine = (error * error) < (fDistanceThreshold * fDistanceThreshold);

		//start of retVec
		if(isInLine && (itStartEqualRange == remainHits.end()))
		{
			itStartEqualRange = it;
			continue;
		}

		//end of retVec
		if(!isInLine && (itStartEqualRange != remainHits.end()))
		{
			retVec.insert(retVec.end(), std::make_move_iterator(itStartEqualRange), std::make_move_iterator(it));
			remainHits.erase(itStartEqualRange, it);
			it = itStartEqualRange;
			itStartEqualRange = remainHits.end();
			continue;
		}
	}
	//remove last hit if it was in line
	if(itStartEqualRange != remainHits.end())
	{
		auto it = remainHits.end();
		retVec.insert(retVec.end(), std::make_move_iterator(itStartEqualRange), std::make_move_iterator(it));
		remainHits.erase(itStartEqualRange, it);
	}

	return retVec;
}

ActTrack SampleConsensus::ActRANSAC::CreateTrack(ActLine& line, std::vector<ActHit>& inliers)
{
	ActTrack track;
	for(auto& hit : inliers)
	{
		track.AddHit(std::move(hit));
	}

	//and now really perform 3D regression!
	if(fFitPattern)
	{
		line.FitCloudToLine(inliers, fChargeThreshold);
	}
	track.SetLine(line);
	return track;
}
