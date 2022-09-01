#ifndef ACTLINE_H
#define ACTLINE_H

//ActPath is based on AtPatternLine
#include "ActHit.h"

#include <Math/Point3D.h>
#include <Math/Point3Dfwd.h>
#include <Math/Vector3D.h>
#include <Math/Vector3Dfwd.h>
#include <Rtypes.h>
#include <RtypesCore.h>
#include <TMath.h>
  
#include <algorithm>
#include <utility>
#include <vector>


class ActLine
{
	public:
	using XYZPoint = ROOT::Math::XYZPoint;
	using XYZVector = ROOT::Math::XYZVector;

	protected:
	XYZPoint fPoint{XYZPoint(-1, -1, -1)};
	XYZVector fDirection{XYZVector(-1, -1, -1)};
	double fChi2{-1};

	//member functions
	public:
	ActLine() = default;
	ActLine(XYZPoint point, XYZVector direction, double chi);
	ActLine(std::vector<XYZPoint>& sampled);
	ActLine(const ActLine& ) = default; //compiler-generated copy constructor
	ActLine(ActLine&& ) = default;//move constructor
	ActLine &operator=(const ActLine& ) = default;//compiler-generated assignment operator
	ActLine &operator=(ActLine&& ) = default;//move assig.
	~ActLine() = default;

	//getters
	XYZPoint GetPoint() const { return fPoint; }
	XYZVector GetDirection() const { return fDirection; }
	double GetChi2() const { return fChi2; }
	
	//setters from 2 sampled ActHits->XYZPoints
	void SetPoint(std::vector<XYZPoint>& sampled);
	void SetDirection(std::vector<XYZPoint>& sampled);
	void SetChi2(double chi);

	//distance to point
	double DistanceLineToHit(const XYZPoint& point) const;
	//1st, get charge from hits
	void FitCloudToLine(const std::vector<ActHit>& pointsToFit, double chargeThreshold);
	//2nd, if chargeThreshold != -1, pass vector and do a weighted regression
	void FitCloudToLine(const std::vector<XYZPoint>& points, const std::vector<double>& charge);
	//3rd, otherwise, plain fit whithout considering charge
	void FitCloudToLine(const std::vector<XYZPoint>& points);//overload version without charge weight!
};

#endif //ACTLINE_H
