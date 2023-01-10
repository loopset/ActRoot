#ifndef SIMBEAM_H
#define SIMBEAM_H

/*
Class for store and manipulate beam energy distribution, flux and so on
*/

#include "SimStructs.h"
#include "TF1.h"
#include "TRandom3.h"
#include "TSpline.h"
#include <memory>
#include <utility>
#include <vector>
class SimBeam
{
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
    void Draw();
    double SampleBeamEnergy(TRandom3* generator = nullptr) const;
    double GetIntegratedFlux(bool withRadius = true) const;
    void GetBeamRange(double& minT, double& maxT) const;
    double GetFluxAtEnergy(double Tn, bool withRadius = true);
    double ComputeScalingFactor(ExperimentInfo*& experiment,
                                     double Tn, long long iter,
                                     double scatteringXSAtTn);
    

    //for beam radius
    void SetBeamRadius(double radiusIncm){ beamRadius = radiusIncm; }
    double GetBeamRadius() const { return beamRadius; }
};

#endif
