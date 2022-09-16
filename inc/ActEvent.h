#ifndef ACTEVENT_H
#define ACTEVENT_H

#include "ActHit.h"
#include "ActCalibrations.h"
#include "ActStructs.h"

#include "../cobo_libs/inc/MEvent.h"
#include "../cobo_libs/inc/MEventReduced.h"

#include <Math/Point3Dfwd.h>
#include <Math/Point3D.h>
#include <vector>


class ActEvent 
{
	public:
	using XYZPoint = ROOT::Math::XYZPoint;
	
protected://if we ever decide to inherit the class
	std::vector<ActHit> fHitArray {};
	Silicons fSilicons {};
	TriggersAndGates fTriggers {};

	private:
	//auxiliar vectors to ReadHits
	std::vector<int> voxel;
	std::vector<int> indexOfVoxelInHitArray;
	// //auxiliar vector to CleanSaturatedPads
	// std::vector<double> pad;


	public:
	ActEvent();//not default bc we have to initialize voxel and indexOfVoxelInHitArray
	~ActEvent() = default;
	void Reset();

	////////////// READERS = SETTERS ////////////////////////////////
	//READ measured data into out structs and ActHit array
	//one function to read at the same time Triggers and Hits
	//time-consuming if we choose to gate GATCONF; however, if we checkOverlap = false time-consumption is improved (at the expense of probably having repeated hits)
	void ReadEvent(const ActCalibrations& calibrations, const MEvent* Evt, const MEventReduced* EvtRed, bool checkOverlap = true);
	//two functions that split reading to our structures; ReadTriggers should be run before ReadHits
	void ReadTriggersAndGates(const MEvent* Evt, const MEventReduced* EvtRed);
	void ReadHits(const ActCalibrations& calibrations, const MEventReduced* EvtRed);

	//clean fHitArray from saturated pads
	void CleanSaturatedHits(double chargeThreshold, int minDimZToDelete = 20);
	
	//calibrate our data using ActCalibrations info
	void CalibrateSilicons(const ActCalibrations& calibrations);

	//read silicon data after calibration (i.e, multiplicity, stopping layer, final energy)
	void ReadSiliconsData();

	/////////////////////// GETTERS ///////////////////////////////
	//get events
	const std::vector<ActHit>& GetConstEventHits() const { return fHitArray; }
	std::vector<ActHit>& GetEventHits() { return fHitArray; }

	//get structs by reference, so we can modify them
	Silicons& GetEventSilicons() { return fSilicons; }
	const Silicons& GetConstEventSilicons() const { return fSilicons; }

	TriggersAndGates& GetEventTriggers() { return fTriggers; }
	const TriggersAndGates& GetConstEventTriggers() const { return fTriggers; }

	protected:
	//splitted functions by types of silicons
	void CalibrateSilicons01S(const ActCalibrations& calibrations);
	void CalibrateSiliconsBeam(const ActCalibrations& calibrations);

	void ReadSilicons01FData();
	void ReadSiliconsSData();

	//inline function to search for matches between ActHits based on position
	inline bool isAlreadyInHitArray(ActHit& hit, ActHit& newHit)
	{
		auto position { hit.GetPosition()};
		auto newPosition { newHit.GetPosition()};
		bool xCondition { position.X() == newPosition.X()};
		bool yCondition { position.Y() == newPosition.Y()};
		bool zCondition { position.Z() == newPosition.Z()};

		return (xCondition && yCondition && zCondition);
	}
	template <typename T>
	inline bool isInVector(T val, std::vector<T> vec)
	{
		if (vec.size() == 0)
			return false;
		return std::find(vec.begin(), vec.end(), val) != vec.end();
	}
	
};


#endif //ACTEVENT_H
