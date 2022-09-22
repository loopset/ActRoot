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
#include <utility>
#include <vector>

//class ActHit;
#include "ActHit.h"
#include "ActLine.h"
#include "ActParameters.h"
#include "TrackPhysics.h"
#include "Silicons.h"

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
	//if it is a valid track (Reaction Point inside ACTAR cave)
	Bool_t fIsGood { true};
	//physical information
	TrackPhysics fTrackPhysics {};
	

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
	Bool_t GetIsGood() const { return fIsGood; }
	// //or as constant
	const std::vector<ActHit>& GetHitArrayConst() const { return fHitArray; }

	//getters for physical info
	TrackPhysics& GetTrackPhysics() { return fTrackPhysics; }
	const TrackPhysics& GetConstTrackPhysics() const { return fTrackPhysics; }

	void SetTrackID(Int_t trackID) { fTrackID = trackID; }
	void AddHit(const ActHit& hit);
	void AddHit(ActHit &&hit);//r-value move
	void SetLine(const ActLine& line);
	void SetIsGood(Bool_t good){ fIsGood = good; }
	
	//setters for physical info
	void SetTrackPhysics(TrackPhysics& info){ fTrackPhysics = info; }
	//setter with self info
	void SetTrackPhysics();

private:
	//inner functions to set values
	void CalculateThetaTrack(ActTrack& track);
	void CalculatePhiTrack(ActTrack& track);
	void CalculateReactionPoint(ActTrack& track);
	void CalculateBoundaryPoint(ActTrack& track);
	void CalculateSiliconPoint(ActTrack& track);
	void CalculateTrackLength(ActTrack& track);
	void CalculateTrackCharge(ActTrack& track);

	inline XYZPoint IntersectionTrackPlane(XYZPoint Pp, XYZVector vp, ActTrack& track)
	{
		auto Pt { track.GetLine().GetPoint()};//point of plane
		auto vt { track.GetLine().GetDirection().Unit()};//vt is a normal vector to plane
		//following https://math.stackexchange.com/questions/3412199/how-to-calculate-the-intersection-point-of-a-vector-and-a-plane-defined-as-a-poi
		auto interesection { Pt + (((Pp - Pt).Dot(vp)) / (vt.Dot(vp))) * vt};
		return interesection;
	}
	inline bool IsInChamber(XYZPoint point)
	{
		bool condX { point.X() >= 0. && point.X() <= ActParameters::g_NPADX};
		bool condY { point.Y() >= 0. && point.Y() <= ActParameters::g_NPADY};
		bool condZ { point.Z() >= 0. && point.Z() <= ActParameters::g_NPADZ};
		return (condX && condY && condZ);
	}
	
	ClassDef(ActTrack, 1);
};
#endif //ACTTRACK_H
