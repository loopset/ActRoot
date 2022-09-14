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
	
	protected:
	std::vector<ActHit> fHitArray {};
	Silicons fSilicons {};
	TriggersAndGates fTriggers {};


	public:
	ActEvent() = default;
	~ActEvent() = default;

	//read measured data into out structs and ActHit array
	void ReadEvent(const ActCalibrations& calibrations, const MEvent* Evt, const MEventReduced* EvtRed);
	//calibrate our data using ActCalibrations info
	void CalibrateSilicons(const ActCalibrations& calibrations);

	//read silicon data after calibration (i.e, multiplicity, stopping layer, final energy)
	void ReadSiliconsData();
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
};


#endif //ACTEVENT_H
