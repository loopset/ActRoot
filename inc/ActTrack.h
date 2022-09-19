#ifndef ACTTRACK_H
#define ACTTRACK_H
//A class for storing ActHits defining a track

#include "ActHit.h"
#include <Math/Point3D.h>
#include <Math/Point3Dfwd.h>
#include <Rtypes.h>
#include <RtypesCore.h>
#include <TMath.h>
#include <TObject.h>
  
#include <algorithm>
#include <iostream>
#include <memory>
#include <utility>
#include <vector>

//class ActHit;
#include "ActHit.h"
#include "ActLine.h"

class ActTrack : public TObject
{
	protected:
	//attributes defining minimum info of a track
	Int_t fTrackID {-1};
	std::vector<ActHit> fHitArray;
	//Line fit results
	ActLine fLine {};
	//physical information
	Double_t fTotalCharge {};
	Double_t fTrackLength {};
	Double_t fTheta {};
	Double_t fPhi {};
	

	public:
	ActTrack() = default;
	ActTrack(const ActTrack& ) = default; //copy constructor
	ActTrack(ActTrack&& ) = default; //move constructor
	ActTrack &operator=(const ActTrack& ) = default;
	ActTrack &operator=(ActTrack&& ) = default; //move assignment
	~ActTrack() = default;
	friend void swap(ActTrack& a, ActTrack& b) noexcept;// swap function to handle ptr ownership (legacy)
	
	Int_t GetTrackID() const { return fTrackID; }
	std::vector<ActHit>& GetHitArray() { return fHitArray; }
	ActLine GetLine() const { return fLine; }
	// //or as constant
	const std::vector<ActHit>& GetHitArrayConst() const { return fHitArray; }

	//getters for physical info
	Double_t GetTotalCharge() const { return fTotalCharge; }
	Double_t GetTrackLength() const { return fTrackLength; }
	Double_t GetTheta() const { return fTheta; }
	Double_t GetPhi() const { return fPhi; }

	void SetTrackID(Int_t trackID) { fTrackID = trackID; }
	void AddHit(const ActHit& hit);
	void AddHit(ActHit &&hit);//r-value move
	void SetLine(const ActLine& line);

	//setters for physical info
	void SetTotalCharge(double charge){ fTotalCharge = charge; }
	void SetTrackLength(double length){ fTrackLength = length; }
	void SetTheta(double theta){ fTheta = theta; }
	void SetPhi(double phi){ fPhi = phi; }

	ClassDef(ActTrack, 1);
};
#endif //ACTTRACK_H
