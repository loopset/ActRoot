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
	//bools to store conditions regarding track state (Reaction Point inside ACTAR cave, etc)
	Bool_t fRPInChamber { false};
	Bool_t fSPInArray   { false};
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
	Bool_t GetRPIsInChamber() const { return fRPInChamber; }
	Bool_t GetSPIsInArray() const { return fSPInArray; }
	// //or as constant
	const std::vector<ActHit>& GetHitArrayConst() const { return fHitArray; }

	//getters for physical info
	TrackPhysics& GetTrackPhysics() { return fTrackPhysics; }
	const TrackPhysics& GetConstTrackPhysics() const { return fTrackPhysics; }

	void SetTrackID(Int_t trackID) { fTrackID = trackID; }
	void AddHit(const ActHit& hit);
	void AddHit(ActHit &&hit);//r-value move
	void SetLine(const ActLine& line);
	void SetRPIsInChamber(Bool_t good){ fRPInChamber = good; }
	void SetSPIsInArray(Bool_t good){ fSPInArray = good; }
	
	//setters for physical info
	void SetMinimalTrackPhysics();
	void SetTrackPhysics(TrackPhysics& info){ fTrackPhysics = info; }
	//setter with self info
	//void SetTrackPhysics(ActCalibrations& calibrations);

private:
	//inner functions to set values
	//two for raw units, if we dont have drift coefs available
	void CalculateSiliconPointRawUnits();
	void CalculateReactionPointRawUnits();
	//these ones work in physical units (mm or pad units)
	//void CalculateThetaTrack(ActCalibrations& calibrations);
	//void CalculatePhiTrack();
	//void CalculateReactionPoint(ActCalibrations& calibrations);
	//void CalculateSiliconPoint(ActCalibrations& calibrations);
	//void CalculateTrackLength();
	void CalculateTrackTotalCharge();
	//void CalculateTrackAverageCharge();

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
	inline bool IsInSiliconPlane(XYZPoint point, std::string mode)
	{
		bool condZ { point.Z() >= 0. && point.Z() <= ActParameters::g_NPADZ};
		bool condXY {};
		if(mode=="S")//side, only check X
		{
			condXY = point.X() >= 0. && point.X() <= ActParameters::g_NPADX;
		}
		else if (mode=="F")//front, check only Y
		{
			condXY = point.Y() >= 0. && point.Y() <= ActParameters::g_NPADY;
		}
		return (condXY && condZ);
	}
	
	ClassDef(ActTrack, 1);
};
#endif //ACTTRACK_H
