#ifndef ACTTRACK_H
#define ACTTRACK_H
//A class for storing ActHits defining a track

#include <Math/Point3Dfwd.h>
#include <Math/Vector3Dfwd.h>
#include <Rtypes.h>
#include <RtypesCore.h>
#include <TMath.h>
#include <TObject.h>
  
#include <algorithm>
#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include <vector>

//class ActHit;
#include "ActCalibrations.h"
#include "ActHit.h"
#include "ActLine.h"
#include "ActParameters.h"
#include "ActStructs.h"


class ActTrack : public TObject
{
public:
	using XYZPoint = ROOT::Math::XYZPoint;
	using XYZVector = ROOT::Math::XYZVector;
protected:
	//attributes defining minimum info of a track
	Int_t fTrackID {-1};
	std::vector<ActHit> fHitArray;
	//Line fit results
	ActLine fLine {};
	
	

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
	const std::vector<ActHit>& GetHitArrayConst() const { return fHitArray; }

	//getters for physical info
	// TrackPhysics& GetTrackPhysics() { return fTrackPhysics; }
	// const TrackPhysics& GetConstTrackPhysics() const { return fTrackPhysics; }

	void SetTrackID(Int_t trackID) { fTrackID = trackID; }
	void AddHit(const ActHit& hit);
	void AddHit(ActHit &&hit);//r-value move
	void SetLine(const ActLine& line);

	ClassDef(ActTrack, 1);
};
#endif //ACTTRACK_H
