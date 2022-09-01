#ifndef ACTRANSAC_H
#define ACTRANSAC_H

#include "ActHit.h"
#include <vector>
#include <Math/Point3D.h>
#include <Math/Point3Dfwd.h>
#include <Math/Vector3D.h>
#include <Math/Vector3Dfwd.h>

#include "ActHit.h"
#include "ActTrack.h"
#include "ActClusteringResults.h"
#include "ActLine.h"

namespace SampleConsensus
{
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
		int fSamplePoints {2};// for Line
		int fSampleMethod {0}; //sample method to choose
		double fGaussianSigma { 30.};
		bool fSampleWithReplacement{false}; //already not implemented!!

		//temporary while we dont do an optimal implementation of random sampling
		bool fIsWeightVectorConstructed {false};
		std::vector<double> fWeightVector {};

		

	public:
		struct Line
		{
			XYZPoint fPoint;
			XYZPoint fDirection;
			double fChi2;
		};
		
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
		void SetSampleMethod(int sampleMethod) { fSampleMethod = sampleMethod; }
		void SetGaussianSigma(double sigma) { fGaussianSigma = sigma; }
		void SetSampleWithReplacement(bool replace) { fSampleWithReplacement = replace; }

		//getters
		int GetIterations() const { return fIterations; }
		int GetMinPatterPoints() const { return fMinPatternPoints; }
		float GetDistanceThreshold() const { return fDistanceThreshold; }
		double GetChargeThreshold() const { return fChargeThreshold; }
		int GetSampleMethod() const { return fSampleMethod; }
		double GetGaussianSigma() const { return fGaussianSigma; }
		bool GetSampleWithReplacement() const { return fSampleWithReplacement; }

		ActClusteringResults Solve(const std::vector<ActHit>& hitArray);

		
		
	private:
		//Line GeneratePathFromHits(const std::vector<ActHit>& hitArray);
		//std::vector<ActHit> movePointsInPattern(Line* path, std::vector<ActHit>& indexes);
		ActTrack CreateTrack(ActLine& line, std::vector<ActHit>& inliers);

		//sampling methods, depends on fSampleMethod
		double GetPDF(const ActHit& hit, const ActHit& referenceHit);
		
		ActLine SampleHits(const std::vector<ActHit>& hitArray);
		
		template<typename T>
		static inline bool isInVector(T val, std::vector<T> vec)
		{
			if (vec.size() == 0)
				return false;
			return std::find(vec.begin(), vec.end(), val) != vec.end();
		}

		int EvaluateRANSAC(const std::vector<ActHit>& hitArray, ActLine& sampledLine);
		std::vector<ActHit> RankLines(const ActLine& line, std::vector<ActHit>& remainHits);
		
	};
}

#endif //ACTRANSAC_H
