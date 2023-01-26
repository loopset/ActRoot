#ifndef SIMBEAM_H
#define SIMBEAM_H

/*
Class for store and manipulate beam energy distribution, flux and so on
*/

#include "Math/Point3D.h"
#include "Math/Vector3D.h"
#include "SimGeometry.h"
#include "SimStructs.h"
#include "TF1.h"
#include "TRandom3.h"
#include "TSpline.h"
#include <memory>
#include <utility>
#include <vector>
class SimBeam
{
public:
    using XYZPoint = ROOT::Math::XYZPoint;
    using XYZVector = ROOT::Math::XYZVector;
private:
    std::vector<std::pair<double, double>> fPoints {};
    std::unique_ptr<TSpline3> splineBeam {};
    std::unique_ptr<TF1> funcBeam {};
    std::unique_ptr<TSpline3> splineFluxEnergy {};
    std::unique_ptr<TF1> funcFluxEnergy {};

    double beamRadius {};

public:
    SimBeam(double radius, std::vector<std::pair<double, double>> points = {});
    ~SimBeam() = default;
    void SetBeamEnergyPDF(std::vector<std::pair<double, double>> points = {});
    void Draw();
    double SampleBeamEnergy(TRandom3* generator = nullptr) const;
    double GetIntegratedFlux(bool withRadius = true) const;
    void GetBeamRange(double& minT, double& maxT) const;
    double GetFluxAtEnergy(double Tn, bool withRadius = true);
    double ComputeScalingFactor(ExperimentInfo*& experiment,
                                     double Tn, long long iter,
                                     double scatteringXSAtTn);
    XYZPoint SampleVertex(TRandom3* generator, const DriftInfo& actar);
    

    //for beam radius
    void SetBeamRadius(double radiusIncm){ beamRadius = radiusIncm; }
    double GetBeamRadius() const { return beamRadius; }
};

#endif
