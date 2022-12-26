#ifndef ACTDBSCAN_H
#define ACTDBSCAN_H

#include "ActClusteringResults.h"

#include "ActHit.h"
#include "ActTrack.h"
#include "Math/Point3D.h"
#include "Math/Vector3D.h"
#include <map>
#include <ostream>
#include <utility>
#include <vector>

#define POINTSUCCESS 0
#define POINTUNCLASSIFIED -1
#define POINTNOISE -2
#define POINTFAILURE -3
//adapted from https://github.com/amarrerod/DBSCAN
class ActDBSCAN
{
public:
    using XYZPoint = ROOT::Math::XYZPoint;
    struct Point
    {
        ActHit hit;
        int clusterID;

        friend std::ostream& operator<<(std::ostream& os, const Point& p)
        {
            return os <<p.hit.GetPosition()<<" in cluster: "<<p.clusterID;
        }
    };
private:
    unsigned int fNClusters;
    unsigned int fMinPoints;
    double fEpsilon;
    std::vector<Point> fPoints;

public:
    ActDBSCAN() = delete;
    // ActDBSCAN(const unsigned int& minPts, const double& eps,
    //           const std::map<std::pair<int, int>, std::pair<double, bool>>& padMatrix) = delete;
    ActDBSCAN(const unsigned int& minPts, const double& eps,
              const std::vector<ActHit>& hits);
    ~ActDBSCAN() = default;

    ActClusteringResults Run();
    
private:
    int ExpandCluster(Point& point, int clusterID);

    std::vector<int> CalculateCluster(const Point& point);

    double CalculateDistance(const Point& reference, const Point& target);
    
    ActTrack CreateTrack(const std::vector<ActHit>& rawClusters);

    template<typename T>
    static inline bool isInVector(T val, std::vector<T> vec)
	{
		if (vec.size() == 0)
			return false;
		return std::find(vec.begin(), vec.end(), val) != vec.end();
	}
};

#endif
