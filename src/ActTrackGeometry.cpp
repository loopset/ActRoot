#include "ActTrackGeometry.h"
#include "ActParameters.h"
#include "ActTrack.h"
#include "SimGeometry.h"
#include "TMathBase.h"

ActTrackGeometry::ActTrackGeometry(SimGeometry* g, ActTrack* t)
    : geo(g), track(t)
{
    
}

void ActTrackGeometry::CorrectDirection(const std::string &side, int silIndex)
{
    auto point {track->GetLine().GetPoint()};
    XYZPoint goodSigns {
            ActParameters::siliconDirection.at(side).at(silIndex).first - point.X(),
            ActParameters::siliconsPlacement.at(side).Y()               - point.Y(),
            ActParameters::siliconDirection.at(side).at(silIndex).second- point.Z()
        };

    auto oldDirection {track->GetLine().GetDirection()};
    XYZVector newDirection {
        TMath::Sign(oldDirection.X(), goodSigns.X()),
        TMath::Sign(oldDirection.Y(), goodSigns.Y()),
        TMath::Sign(oldDirection.Z(), goodSigns.Z()),
    };
    //and normalize
    info.fUnitaryDirection = newDirection.Unit();
}

void ActTrackGeometry::MoveTrackToGeometryFrame()
{
    DriftInfo drift { geo->GetDriftParameters()};
    auto point {track->GetLine().GetPoint()};
    info.fGravityPoint = {point.X() - drift.X, point.Y() - drift.Y, point.Z() - drift.Z};
}
