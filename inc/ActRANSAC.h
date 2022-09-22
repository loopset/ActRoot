#ifndef ACTRANSAC_H
#define ACTRANSAC_H

#include <Rtypes.h>
#include <algorithm>
#include <vector>
#include <memory>
#include <utility>
#include <Math/Point3D.h>
#include <Math/Point3Dfwd.h>
#include <Math/Vector3D.h>
#include <Math/Vector3Dfwd.h>

#include "ActHit.h"
#include "ActTrack.h"
#include "ActClusteringResults.h"
#include "ActLine.h"
#include "ActSample.h"

class ActRANSAC
{
public:
	using XYZPoint = ROOT::Math::XYZPoint;
	using XYZVector = ROOT::Math::XYZVector;
private:
	int fIterations {500};
	int fMinPatternPoints {30}; //minimum number of points to consider a pattern
	float fDistanceThreshold {15}; //below this distance, a point is considered an inlier
	bool fFitPattern {true};
	double fChargeThreshold {-1.};
	int fSamplePoints {2};// for ActLine (should not be changed)
	// int fSampleMethod {0}; //sample method to choose
	// double fGaussianSigma { 30.};
	// bool fSampleWithReplacement{false}; // not implemented yet!!
	std::unique_ptr<ActSample> fSampler {std::make_unique<ActSample>()};

		
public:
	ActRANSAC() = default;
	ActRANSAC(int iteractions, int minPoints, float distanceThreshold);
	~ActRANSAC() = default;

	//setters
	void SetNumIterations(int iterations) { fIterations = iterations; }
	void SetMinPatternPoints(int minPoints) { fMinPatternPoints = minPoints; }
	void SetDistanceThreshold(float distThres) { fDistanceThreshold = distThres; }
	void SetFitPattern(bool fit) { fFitPattern = fit; }
	void SetChargeThreshold(double charThres) { fChargeThreshold = charThres; }
	//for sampler
	void SetSampler(std::unique_ptr<ActSample> samp) { fSampler = std::move(samp); }
	void SetSampleMethod(SamplingMethod sampleMethod) { fSampler->SetSampleMehtod(sampleMethod); }
	void SetGaussianSigma(double sigma) { fSampler->SetGaussianSigma(sigma); }
	void SetSampleWithReplacement(bool replace) { fSampler->SetSampleWithReplacement(replace); }

	//getters
	int GetIterations() const { return fIterations; }
	int GetMinPatterPoints() const { return fMinPatternPoints; }
	float GetDistanceThreshold() const { return fDistanceThreshold; }
	double GetChargeThreshold() const { return fChargeThreshold; }

	//for sampler
	SamplingMethod GetSampleMethod() const { return fSampler->GetSampleMethod(); }
	double GetGaussianSigma() const { return fSampler->GetGaussianSigma(); }
	bool GetSampleWithReplacement() const { return fSampler->GetSampleWithReplacement(); }

	//master method
	ActClusteringResults Solve(const std::vector<ActHit>& hitArray);

		
		
private:
	ActTrack CreateTrack(ActLine& line, std::vector<ActHit>& inliers);
	int EvaluateRANSAC(const std::vector<ActHit>& hitArray, ActLine& sampledLine);
	std::vector<ActHit> RankLines(const ActLine& line, std::vector<ActHit>& remainHits);
		
};


#endif //ACTRANSAC_H
