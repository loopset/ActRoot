#include "ActRANSAC.h"

#include "ActHit.h"
#include "ActLine.h"
#include "ActTrack.h"
#include "ActClusteringResults.h"

#include <Math/PdfFuncMathCore.h>
#include <Rtypes.h>
#include <TRandom.h>
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <iterator>
#include <vector>
#include <set>
#include <stdexcept>

ActRANSAC::ActRANSAC(int iteractions, int minPoints, float distanceThreshold)
	: fIterations(iteractions), fMinPatternPoints(minPoints), fDistanceThreshold(distanceThreshold)
{
}

ActClusteringResults ActRANSAC::Solve(const std::vector<ActHit>& hitArray)
{
	if(hitArray.size() < fMinPatternPoints)
	{
		throw std::runtime_error("In ActRANSAC::Solve, passed hitArray has less hits than fMinPatterPoints");
	}
	//follow ATTPROOT for sorting
	auto compareLines = [](const ActLine& a, const ActLine& b) { return a.GetChi2() < b.GetChi2(); };//lambda functor
	auto sortedLines  = std::set<ActLine, decltype(compareLines)>(compareLines);

	//set hits to sample
	fSampler->SetHitsToSample(&hitArray);
	for (int i = 0; i < fIterations; i++)//RANSAC ITERATIONS
	{
		//1st we sample two points and we find a line from them
		auto hitSamples = fSampler->SampleHits(fSamplePoints);
		std::vector<XYZPoint> pointSamples;//temporary, we need to convert ActHit to XYZPoint
		for(auto& hs : hitSamples) pointSamples.push_back(hs.GetPosition());
		auto line = ActLine(pointSamples);
		//auto line = SampleHits(hitArray);
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

int ActRANSAC::EvaluateRANSAC(const std::vector<ActHit>& hitArray, ActLine& sampledLine)
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

std::vector<ActHit> ActRANSAC::RankLines(const ActLine& line, std::vector<ActHit>& remainHits)
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

ActTrack ActRANSAC::CreateTrack(ActLine& line, std::vector<ActHit>& inliers)
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
