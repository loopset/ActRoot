#ifndef TRACKPHYSICS_H
#define TRACKPHYSICS_H

#include <Math/Point3Dfwd.h>
#include <Math/Point3D.h>
#include <string>

struct TrackPhysics
{
	using XYZPoint = ROOT::Math::XYZPoint;
	
	int fTrackID {-1};
	double fTheta {};
	double fPhi {};
	XYZPoint fReactionPoint {-1, -1, -1};
	XYZPoint fSiliconPoint {-1, -1, -1};
	double fTrackLength {};
	double fTotalCharge {};
	double fAverageCharge {};
	std::string fReactionPlace {""};
	std::string fSiliconPlace {""};
	
	TrackPhysics() = default;
	~TrackPhysics() = default;
};

#endif //TRACKPHYSICS_H
