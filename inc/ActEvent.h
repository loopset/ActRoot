#ifndef ACTEVENT_H
#define ACTEVENT_H

#include "ActHit.h"
#include "ActCalibrations.h"
#include "../cobo_libs/inc/MEventReduced.h"

#include <Math/Point3Dfwd.h>
#include <Math/Point3D.h>
#include <vector>

struct Silicons
{
	std::vector<double> fSi0, fSi0_cal;
	std::vector<double> fSi1, fSi1_cal;
	std::vector<double> fSiS, fSiS_cal;

	Silicons();
	~Silicons() = default;
	
};

class ActEvent 
{
	public:
	using XYZPoint = ROOT::Math::XYZPoint;
	
	protected:
	std::vector<ActHit> fHitArray {};
	Silicons fSilicons {};


	public:
	ActEvent() = default;
	~ActEvent() = default;

	void ReadEvent(const ActCalibrations& calibrations, const MEventReduced* EvtRed);
	//get events
	const std::vector<ActHit>& GetConstEventHits() const { return fHitArray; }
	std::vector<ActHit>& GetEventHits() { return fHitArray; }

	//get structs by reference, so we can modify them
	Silicons GetEventSilicons() { return fSilicons; }

	
};


#endif //ACTEVENT_H
