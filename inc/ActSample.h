#ifndef ACTSAMPLE_H
#define ACTSAMPLE_H

#include "ActHit.h"

#include <Math/Point3D.h>
#include <Math/Point3Dfwd.h>

#include <Rtypes.h>
#include <algorithm>
#include <vector>

enum class SamplingMethod { kUniform, kGaussian, kChargeWeighted, kWeightedGaussian};
	
class ActSample
{
public:
	using XYZPoint = ROOT::Math::XYZPoint;
protected:
	const std::vector<ActHit>* fHitArray {nullptr};
	std::vector<double> fCDF {};//cumulative distribution function
	SamplingMethod fSampleMethod { SamplingMethod::kUniform};
	bool fSampleWithReplacement { false};
	double fGaussianSigma { 30.};
	ActHit fReferenceHit {ActHit()};

public:
	ActSample() = default;
	~ActSample() = default;

	//return functions
	std::vector<ActHit> SampleHits(int N);

	//setters
	void SetHitsToSample(const std::vector<ActHit>* hits);
	void SetSampleMehtod(SamplingMethod method) { fSampleMethod = method; }
	void SetSampleWithReplacement(bool method) { fSampleWithReplacement = method; }
	void SetGaussianSigma(double val) { fGaussianSigma = val; }
	void SetReferenceHit(ActHit hit);

	SamplingMethod GetSampleMethod() const { return fSampleMethod; }
	bool GetSampleWithReplacement() const { return fSampleWithReplacement; }
	const ActHit& GetReferenceHit() const { return fReferenceHit; }
	double GetGaussianSigma() const { return fGaussianSigma; }

protected://internal methods
	std::vector<double> PDF(const ActHit& hit);
	void FillCDF();
	double getPDFfromCDF(int index);
	std::vector<int> sampleIndicesFromCDF(int N, std::vector<int> vetoed = {});
	int getIndexFromCDF(double r, double rmCDF, std::vector<int> vetoed);
	template <typename T>
	static inline bool isInVector(T val, std::vector<T> vec)
	{
		if (vec.size() == 0)
			return false;
		return std::find(vec.begin(), vec.end(), val) != vec.end();
	}
	void SampleReferenceHit();
};

#endif //ACTSAMPLE_H
