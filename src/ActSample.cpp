#include "ActSample.h"

#include "ActHit.h"
#include "ActParameters.h"

#include <Rtypes.h>
#include <TRandom.h>
#include <algorithm>
#include <cmath>
#include <ostream>
#include <stdexcept>
#include <vector>
#include <functional>
#include <numeric>
#include <utility>

#include <Math/PdfFuncMathCore.h>
#include <Math/Point3D.h>
#include <Math/Vector3D.h>


//1st, build PDF
std::vector<double> ActSample::PDF(const ActHit& hit)
{
	if(fSampleMethod == SamplingMethod::kUniform)//uniform
		return {1.};
	else if(fSampleMethod == SamplingMethod::kGaussian)
	{
		auto dist = (fReferenceHit.GetPosition() - hit.GetPosition()).Mag2();
		dist = std::sqrt(dist);
		return {ROOT::Math::gaussian_pdf(dist, fGaussianSigma)};
	}
	else if(fSampleMethod == SamplingMethod::kChargeWeighted)//charge weighted
		return {hit.GetCharge()};
	else if( fSampleMethod == SamplingMethod::kWeightedGaussian)
	{
		auto dist = (fReferenceHit.GetPosition() - hit.GetPosition()).Mag2();
		dist = std::sqrt(dist);
		return {ROOT::Math::gaussian_pdf(dist, fGaussianSigma), hit.GetCharge()};
	}
	else
	{
		throw std::runtime_error("SimplePDF not available for chosen sampling method");
	}
}

//2nd, fill CDF
void ActSample::FillCDF()
{
	std::vector<double> normalization;
	fCDF.clear();
	for(const auto& hit : *fHitArray)
	{
		//1st, get marginal and joint (= multiplication if independent) PDF
		auto pdfMarginal = PDF(hit);
		auto pdfJoint = std::accumulate(pdfMarginal.begin(), pdfMarginal.end(), 1.0, std::multiplies<>());
		//initialize normalization vector
		if(normalization.size() == 0)
			normalization.assign(pdfMarginal.size(), 0);

		for(int i = 0; i < pdfMarginal.size(); i++)
		{
			normalization[i] += pdfMarginal[i];
		}

		//and now push back to CDF
		if(fCDF.size() == 0)
			fCDF.push_back(pdfJoint);
		else
			fCDF.push_back(pdfJoint + fCDF.back());
	}

	//get norm
	auto norm = std::accumulate(normalization.begin(), normalization.end(), 1.0, std::multiplies<>());
	for(auto& el : fCDF)
		el /= norm;
}
double ActSample::getPDFfromCDF(int index)
{
	return (index == 0) ? fCDF[0] : (fCDF[index] - fCDF[index - 1]);
}
//and now methods to sample from CDF! remember that we do implement now with(out) replacement
//if without, that means we have to scale fCDF values!!

int ActSample::getIndexFromCDF(double r, double rmCDF, std::vector<int> vetoed)
{
	//track probability removed from CDF
	double probRemoved { 0.};
	
	for(int i = 0; i < fCDF.size(); i++)
	{
		if(isInVector(i, vetoed))
		{
			probRemoved += getPDFfromCDF(i);
			continue;
		}
		if(((fCDF[i] - probRemoved) / (1. - rmCDF)) >= r)
			return i;
	}
	return fCDF.size() - 1;
}

std::vector<int> ActSample::sampleIndicesFromCDF(int N, std::vector<int> vetoed)
{
	//probability contained in vetoed hits
	auto probSum = [this](double accum, int ind){ return accum + getPDFfromCDF(ind); };
	double rmProb = std::accumulate(vetoed.begin(), vetoed.end(), 0.0, probSum);
	
	std::vector<int> sampledInd;
    int counter {0};
	while(sampledInd.size() < N)
	{
        if(counter > 10)//bugfix: cut to avoid infinite loop
        {
            std::cout<<BOLDRED<<"Avoiding infinile loop in sampleIndicesFromCDF"<<'\n';
            while(sampledInd.size() < N)
            {//fallback to Uniform
                sampledInd.push_back(static_cast<int>(gRandom->Uniform() * fHitArray->size()));
            }
           break; 
        }
        //random number [0,1)
		auto r = gRandom->Uniform();
		auto hitInd = getIndexFromCDF(r, rmProb, vetoed);
		//now implement without replacement
		if(!fSampleWithReplacement)
		{
			rmProb += getPDFfromCDF(hitInd);
			//in any case, add hitInd to vetoed if not present yet
			if(!isInVector(hitInd, vetoed))
				vetoed.push_back(hitInd);
			//std::cout<<"vetoed index: "<<hitInd<<'\n';
		}

		if(fSampleWithReplacement || !isInVector(hitInd, sampledInd))
			sampledInd.push_back(hitInd);
        counter++;
	}
    //bugfix: for some event, the previous while enters a infine loop
	return sampledInd;
}

//for reference Hit
void ActSample::SetReferenceHit(ActHit hit)
{
	fReferenceHit = std::move(hit);
	//FillCDF(); //filling is done is SetHitsToSample
}


void ActSample::SampleReferenceHit()
{
	if(fSampleMethod == SamplingMethod::kGaussian)//gaussian
	{
		int refIndex = gRandom->Uniform() * fHitArray->size();
		SetReferenceHit(fHitArray->at(refIndex));
	}
	if(fSampleMethod == SamplingMethod::kWeightedGaussian)//charge weighted + gaussian
	{
		//here, the reference hit is sampled from a charge weighted distribution
		//so first we trick the class to work as fSampleMethod = 2
		SetSampleMehtod(SamplingMethod::kGaussian);
		FillCDF();
		SetReferenceHit(std::move(SampleHits(1)[0]));
		//and finally reset sample method and fill again CDF
		SetSampleMehtod(SamplingMethod::kWeightedGaussian);
		//FillCDF();//this is done in SetHitsToSample!
	}
	
}

void ActSample::SetHitsToSample(const std::vector<ActHit> *hits)
{
	fHitArray = std::move(hits);
	if(fSampleMethod == SamplingMethod::kChargeWeighted)
		FillCDF();
	
	
}

//and finally, sample
std::vector<ActHit> ActSample::SampleHits(int N)
{
	std::vector<ActHit> out;
	//for Uniform (fSampleMethod == 0), override previous methods
	if(fSampleMethod == SamplingMethod::kUniform)
	{
		std::vector<int> ind;
		while(ind.size() < N)
		{
			int i = gRandom->Uniform() * fHitArray->size();
			if(fSampleWithReplacement || !isInVector(i, ind))
			{
				ind.push_back(i);
				out.push_back(fHitArray->at(i));
			}
		}
		return out;//break here
	}
	else if(fSampleMethod == SamplingMethod::kGaussian)
	{
		SampleReferenceHit();
		FillCDF();
	}
	else if(fSampleMethod == SamplingMethod::kChargeWeighted)
		;//do nothing, FillingCDF is done once in SetHitsToSample()
	else if(fSampleMethod == SamplingMethod::kWeightedGaussian)//for Weighted + gaussian we need a trick!
	{
		SampleReferenceHit();
		FillCDF();
	}
	else
	{
		throw std::runtime_error("SetHitsToSample not available for chosen sampling method");
	}
	
	auto sampledIndices = sampleIndicesFromCDF(N);
	for(auto& ind : sampledIndices)
		out.push_back(fHitArray->at(ind));
	return out;
}
