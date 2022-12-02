#ifndef ACTTRACKGEOMETRY_H
#define ACTTRACKGEOMETRY_H
#include "ActTrack.h"
#include "SimGeometry.h"

#include <Math/Point3Dfwd.h>
#include <Math/Point3D.h>

#include <Math/Vector3Dfwd.h>
#include <Math/Vector3D.h>
#include <string>

struct TrackInfo
{
    using XYZPoint = ROOT::Math::XYZPoint;
	using XYZVector = ROOT::Math::XYZVector;

    XYZPoint fGravityPoint {-1, -1, -1};
    XYZVector fUnitaryDirection {-1, -1, -1};// to Silicon
    int fSilIndexGeo {-1};
    int fSilIndexTrigger {-1};
    XYZPoint fSiliconPoint {-1, -1, -1}; 
};

class ActTrackGeometry
{
public:
    using XYZPoint = ROOT::Math::XYZPoint;
	using XYZVector = ROOT::Math::XYZVector;
private:
    SimGeometry* geo {};
    ActTrack*    track{};
    TrackInfo info {};

public:
    ActTrackGeometry() = default;
    ActTrackGeometry(SimGeometry* g, ActTrack* t);
    ~ActTrackGeometry() = default;

    void CorrectDirection(const std::string& side, int silIndex);
    void MoveTrackToGeometryFrame();

    void PropagateToSilicon();
};


#endif
