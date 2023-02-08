#include "ActLine.h"

#include "ActParameters.h"
#include "ActHit.h"

#include <cmath>
#include <exception>
#include <stdexcept>
#include <vector>
#include <iostream>

ActLine::ActLine(XYZPoint point, XYZVector direction, double chi)
	: fPoint{point}, fDirection{direction}, fChi2{chi}  
{
}
ActLine::ActLine(std::vector<XYZPoint>& sampled)
{
	if(sampled.size() != 2) throw std::runtime_error("sampled ActHits size differs from 2!");
	fPoint = sampled[0];
	fDirection = sampled[1] - sampled[0];
}

void ActLine::SetPoint(std::vector<XYZPoint> &sampled)
{
	if(sampled.size() != 2) throw std::runtime_error("sampled ActHits size differs from 2!");
	fPoint = sampled[0];//if fitted, this point corresponds to M in the regression paper
	//meaning it is the gravity center of the cloud
}

void ActLine::SetDirection(std::vector<XYZPoint> &sampled)
{
	if(sampled.size() != 2) throw std::runtime_error("sampled ActHits size differs from 2!");
	fDirection = sampled[0] - sampled[1];//for backwards compatibility, point to gravity center always
}

void ActLine::SetChi2(double chi)
{
	fChi2 = chi;
}

double ActLine::DistanceLineToHit(const XYZPoint &point) const
{
	auto vec = point - fPoint;
	auto nD = fDirection.Cross(vec);
	double dist2 = nD.Mag2() / fDirection.Mag2();

	return std::sqrt(dist2);
}

void ActLine::FitCloudToLine(const std::vector<ActHit>& pointsToFit, double chargeThreshold)
{
	std::vector<XYZPoint> points; std::vector<double> charge;
	for(const auto& hit : pointsToFit)
	{
		if(hit.GetCharge() > chargeThreshold)
		{
			points.push_back(hit.GetPosition());
			charge.push_back(hit.GetCharge());
		}
	}
	if(points.size() < 2)
	{
		std::cout<<BOLDMAGENTA<<"Warning: Charge weighted fit in ActLine::FitCloudToLine has less hits than fMinPatterHits -> Not fitting"<<RESET<<'\n';
		return;
	}
	if (chargeThreshold == -1.)
		FitCloudToLine(points);
	else
		FitCloudToLine(points, charge);
}

void ActLine::FitCloudToLine(const std::vector<XYZPoint>& points, const std::vector<double>& charge)
{
	//------3D Line Regression
    //----- adapted from: http://fr.scribd.com/doc/31477970/Regressions-et-trajectoires-3D
    int R, C;
    double Q;
    double Xm, Ym, Zm;
    double Xh, Yh, Zh;
    double a, b;
    double Sxx, Sxy, Syy, Sxz, Szz, Syz;
    double theta;
    double K11, K22, K12, K10, K01, K00;
    double c0, c1, c2;
    double p, q, r, dm2;
    double rho, phi;
  
    Q = Xm = Ym = Zm = 0.;
    double total_charge = 0;
    Sxx = Syy = Szz = Sxy = Sxz = Syz = 0.;
    bool doChargeWeight = (points.size() == charge.size());
  
    for (int i = 0; i < points.size(); ++i) {
       const auto hitQ = doChargeWeight ? charge[i] : 1.;
       const auto &pos = points[i];
       Q += hitQ / 10.;
       Xm += pos.X() * hitQ / 10.;
       Ym += pos.Y() * hitQ / 10.;
       Zm += pos.Z() * hitQ / 10.;
       Sxx += pos.X() * pos.X() * hitQ / 10.;
       Syy += pos.Y() * pos.Y() * hitQ / 10.;
       Szz += pos.Z() * pos.Z() * hitQ / 10.;
       Sxy += pos.X() * pos.Y() * hitQ / 10.;
       Sxz += pos.X() * pos.Z() * hitQ / 10.;
       Syz += pos.Y() * pos.Z() * hitQ / 10.;
    }
  
    Xm /= Q;
    Ym /= Q;
    Zm /= Q;
    Sxx /= Q;
    Syy /= Q;
    Szz /= Q;
    Sxy /= Q;
    Sxz /= Q;
    Syz /= Q;
    Sxx -= (Xm * Xm);
    Syy -= (Ym * Ym);
    Szz -= (Zm * Zm);
    Sxy -= (Xm * Ym);
    Sxz -= (Xm * Zm);
    Syz -= (Ym * Zm);
  
    theta = 0.5 * atan((2. * Sxy) / (Sxx - Syy));
  
    K11 = (Syy + Szz) * pow(cos(theta), 2) + (Sxx + Szz) * pow(sin(theta), 2) - 2. * Sxy * cos(theta) * sin(theta);
    K22 = (Syy + Szz) * pow(sin(theta), 2) + (Sxx + Szz) * pow(cos(theta), 2) + 2. * Sxy * cos(theta) * sin(theta);
    // K12 = -Sxy * (pow(cos(theta), 2) - pow(sin(theta), 2)) + (Sxx - Syy) * cos(theta) * sin(theta);
    K10 = Sxz * cos(theta) + Syz * sin(theta);
    K01 = -Sxz * sin(theta) + Syz * cos(theta);
    K00 = Sxx + Syy;
  
    c2 = -K00 - K11 - K22;
    c1 = K00 * K11 + K00 * K22 + K11 * K22 - K01 * K01 - K10 * K10;
    c0 = K01 * K01 * K11 + K10 * K10 * K22 - K00 * K11 * K22;
  
    p = c1 - pow(c2, 2) / 3.;
    q = 2. * pow(c2, 3) / 27. - c1 * c2 / 3. + c0;
    r = pow(q / 2., 2) + pow(p, 3) / 27.;
  
    if (r > 0)
       dm2 = -c2 / 3. + pow(-q / 2. + sqrt(r), 1. / 3.) + pow(-q / 2. - sqrt(r), 1. / 3.);
    else {
       rho = sqrt(-pow(p, 3) / 27.);
       phi = acos(-q / (2. * rho));
       dm2 = std::min(-c2 / 3. + 2. * pow(rho, 1. / 3.) * cos(phi / 3.),
                      std::min(-c2 / 3. + 2. * pow(rho, 1. / 3.) * cos((phi + 2. * TMath::Pi()) / 3.),
                               -c2 / 3. + 2. * pow(rho, 1. / 3.) * cos((phi + 4. * TMath::Pi()) / 3.)));
    }
  
    a = -K10 * cos(theta) / (K11 - dm2) + K01 * sin(theta) / (K22 - dm2);
    b = -K10 * sin(theta) / (K11 - dm2) - K01 * cos(theta) / (K22 - dm2);
  
    Xh = ((1. + b * b) * Xm - a * b * Ym + a * Zm) / (1. + a * a + b * b);
    Yh = ((1. + a * a) * Ym - a * b * Xm + b * Zm) / (1. + a * a + b * b);
    Zh = ((a * a + b * b) * Zm + a * Xm + b * Ym) / (1. + a * a + b * b);
  
    // First 3 are point1. Second 3 are point 2
    XYZPoint p1 = {Xm, Ym, Zm};
    XYZPoint p2 = {Xh, Yh, Zh};
	std::vector<XYZPoint> vPoints {p1, p2};

	//temporary bug fix: detect if we have NaN values here dont redefine ActLine!
	if(std::isnan(dm2))
	{
        // std::cout<<"c2 "<<c2<<'\n';
        // std::cout<<"q "<<q<<'\n';
        // std::cout<<"r "<<r<<'\n';
        // std::cout<<"rho "<<rho<<'\n';
        // std::cout<<"phi "<<phi<<'\n';
		// std::cout<<BOLDMAGENTA<<"Warning: dm2 is NaN is ActLine::FitLineToCloud due to pad saturation -> Not fitting"<<RESET<<'\n';
		//std::cout<<"Remaining line direction X: "<<GetDirection().X()<<" Y: "<<GetDirection().Y()<< " Z: "<<GetDirection().Z()<<'\n';
		return;
	}
	SetPoint(vPoints);
	SetDirection(vPoints);
	SetChi2(fabs(dm2 / Q));
	//WARNING: Something in this func returns nan sometimes! It is in variable dm2!
}

void ActLine::FitCloudToLine(const std::vector<XYZPoint>& points)
{
	//std::cout<<"Fitting ActLine without considering charge!"<<'\n';
	std::vector<double> vQ {-1.};
	FitCloudToLine(points, vQ);
}
