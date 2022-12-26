#include "ActDBSCAN.h"

#include "ActClusteringResults.h"
#include "ActHit.h"
#include "ActLine.h"
#include "ActTrack.h"
#include "TMath.h"
#include "Math/Point3D.h"
#include "Math/Vector3D.h"
#include <algorithm>
#include <map>
#include <ostream>
#include <utility>
#include <vector>

ActDBSCAN::ActDBSCAN(const unsigned int& minPts, const double& eps,
                     const std::vector<ActHit>& hits)
    : fMinPoints(minPts), fEpsilon(eps), fNClusters(0)
{
    fPoints.reserve(hits.size());
    for(const auto& hit : hits)
    {
        Point p;
        p.hit = hit;//{static_cast<double>(pos.first), static_cast<double>(pos.second), 0.0};
        p.clusterID = POINTUNCLASSIFIED;
        fPoints.push_back(p);
    }
}

ActClusteringResults ActDBSCAN::Run()
{
    int clusterID = 1;
    for(auto it = fPoints.begin(); it != fPoints.end(); it++)
    {
        if(it->clusterID == POINTUNCLASSIFIED)
        {
            if(ExpandCluster(*it, clusterID) != POINTFAILURE)
                clusterID++;
        }
    }

    std::map<int, std::vector<ActHit>> rawClusters;
    for(const auto& p : fPoints)
    {
        if(p.clusterID != POINTUNCLASSIFIED && p.clusterID != POINTNOISE)
            rawClusters[p.clusterID].push_back(p.hit);
    }

    ActClusteringResults clusters;
    for(const auto& [clusterID, vec] : rawClusters)
    {
        auto track {CreateTrack(vec)};
        track.SetTrackID(clusters.GetTrackCandidates().size());
        clusters.AddTrack(track);
    }
    
    return clusters;
}

int ActDBSCAN::ExpandCluster(Point& point, int clusterID)
{
    auto clusterSeeds { CalculateCluster(point)};
    if(clusterSeeds.size() < fMinPoints)
    {
        point.clusterID = POINTNOISE;
        return POINTFAILURE;
    }
    else
    {
        int index {0}; int indexCore {0};
        for(auto it = clusterSeeds.begin(); it != clusterSeeds.end(); it++)
        {
            fPoints.at(*it).clusterID = clusterID;
            if(fPoints.at(*it).hit.GetPosition() == point.hit.GetPosition())
                indexCore = index;
            index++;
        }
        //delete core point
        clusterSeeds.erase(clusterSeeds.begin() + indexCore);

        //neighors
        for(int i = 0; i < clusterSeeds.size(); i++)
        {
            auto clusterNeighbours { CalculateCluster(fPoints.at(clusterSeeds.at(i)))};
            if(clusterNeighbours.size() >= fMinPoints)
            {
                for(auto it = clusterNeighbours.begin(); it != clusterNeighbours.end(); it++)
                {
                    if(fPoints.at(*it).clusterID == POINTUNCLASSIFIED
                       || fPoints.at(*it).clusterID == POINTNOISE)
                    {
                        if(fPoints.at(*it).clusterID == POINTUNCLASSIFIED)
                            if(!isInVector(*it, clusterSeeds))//only add if not listed previously
                                clusterSeeds.push_back(*it);
                        fPoints.at(*it).clusterID = clusterID;
                    }
                }
            }
        }
        return POINTSUCCESS;
    } 
}

std::vector<int> ActDBSCAN::CalculateCluster(const Point& point)
{
    int index {0};
    std::vector<int> retVec;
    for(auto it = fPoints.begin(); it != fPoints.end(); it++)
    {
        if(CalculateDistance(point, *it) <= fEpsilon)
            retVec.push_back(index);
        index++;
    }
    return retVec;
}

double ActDBSCAN::CalculateDistance(const Point& reference, const Point& target)
{
    return TMath::Sqrt((reference.hit.GetPosition() - target.hit.GetPosition()).Mag2());
}

ActTrack ActDBSCAN::CreateTrack(const std::vector<ActHit>& hits)
{
    ActTrack track;
    for(const auto& hit : hits)
    {
        track.AddHit(std::move(hit));
    }
    ActLine line;
    line.FitCloudToLine(hits, -1);//no charge threshold
    track.SetLine(line);
    return track;
}
