#ifndef ActSilSpecs
#define ActSilSpecs

#include "ActInputParser.h"
#include "ActSilMatrix.h"

#include "TVirtualPad.h"

#include "Math/GenVector/Cartesian3D.h"
#include "Math/GenVector/DisplacementVector3D.h"
#include "Math/GenVector/PositionVector3D.h"
#include "Math/Point3D.h"
#include "Math/Point3Dfwd.h"
#include "Math/Vector3D.h"
#include "Math/Vector3Dfwd.h"

#include <memory.h>

#include <map>
#include <memory>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>

namespace ActPhysics
{
enum class SilSide
{
    ELeft,
    ERight,
    EFront,
    EBack
};

enum class SilParticle
{
    ELight,
    EHeavy,
    EBoth,
};

//! A class representing a unit of silicon detector: geometry specs
class SilUnit
{
private:
    double fWidth {};
    double fHeight {};
    double fThick {};

public:
    SilUnit() = default;
    SilUnit(double height, double width, double thick) : fHeight(height), fWidth(width), fThick(thick) {}
    void Print() const;
    double GetWidth() const { return fWidth; }
    double GetHeight() const { return fHeight; };
    double GetThickness() const { return fThick; }
};

class SilLayer
{
public:
    template <typename T>
    using Point = ROOT::Math::PositionVector3D<ROOT::Math::Cartesian3D<T>>;
    template <typename T>
    using Vector = ROOT::Math::DisplacementVector3D<ROOT::Math::Cartesian3D<T>>;
    using XYZPointF = Point<float>;
    using XYZVectorF = Vector<float>;

private:
    std::map<int, std::pair<double, double>> fPlacements; //!< Individual centres of each silicon. In mm
    std::map<int, double> fThresholds;                    //!< Thresholds for all silicons. In MeV
    SilUnit fUnit;                                        //!< Specifications of unit silicon. In mm
    XYZPointF fPoint;                      //!< Contains centre of layer, where beam is centered in mm units
    XYZVectorF fNormal;                    //!< Normal vector of silicon plane, needed to compute the SP
    double fMargin {};                     //!< Margin in mm to validate or not SP in MatchesRealPlacement function
    std::shared_ptr<SilMatrix> fMatrix {}; //!< Pointer to SiliconMatrix
    SilSide fSide;                         //!< Enum to spec side of layer with respect to ACTAR's frame
    SilParticle fPart;                     //! Type of particle that can hit this silicon layer
    std::set<int> fMults {};               //!< Allowed multiplicities for this silicon layer
    int fPadIdx {1};                       //!< In HistogramPainter, index of pad for this layer

public:
    SilLayer() = default;
    void ReadConfiguration(std::shared_ptr<ActRoot::InputBlock> block);
    void ReplaceWithMatrix(SilMatrix* sm);
    void Print() const;

    // Getters and setters
    const std::map<int, std::pair<double, double>>& GetPlacements() const { return fPlacements; }
    const std::map<int, double>& GetThresholds() const { return fThresholds; }
    template <typename T>
    bool ApplyThreshold(int idx, T val) const
    {
        return fThresholds.at(idx) <= val;
    }
    const SilUnit& GetUnit() const { return fUnit; }
    const XYZPointF& GetPoint() const { return fPoint; }
    const XYZVectorF& GetNormal() const { return fNormal; }
    const SilSide& GetSilSide() const { return fSide; }
    std::shared_ptr<SilMatrix> GetSilMatrix() const { return fMatrix; }
    SilParticle GetParticle() const { return fPart; }
    const std::set<int>& GetMults() const { return fMults; }
    bool CheckMult(int mult) const;
    int GetPadIdx() const { return fPadIdx; }

    ////
    void SetPoint(const XYZPointF& pInPads) { fPoint = pInPads; }

    // Computation operations
    template <typename T>
    std::pair<Point<T>, bool>
    GetSiliconPointOfTrack(const Point<T>& point, const Vector<T>& vector, bool isPadUnits) const;

    template <typename T>
    Point<T> GetBoundaryPointOfTrack(int padx, int pady, const Point<T>& point, const Vector<T>& vector) const;

    template <typename T>
    bool MatchesRealPlacement(int i, const Point<T>& sp, bool useZ = true) const;

    template <typename T>
    int GetIndexOfMatch(const Point<T>& p) const;

    template <typename T>
    int AssignSPtoPad(const Vector<T>& vsp, const std::vector<int>& pads) const;

    // Useful for simulation basically
    void UpdatePlacementsFromMatrix();
    void MoveZTo(double z, const std::set<int>& idxs);
    double MeanZ(const std::set<int>& idxs);

private:
    std::shared_ptr<SilMatrix> BuildSilMatrix() const;
};

class SilSpecs
{
public:
    using LayerMap = std::unordered_map<std::string, SilLayer>;
    using PartSet = std::set<std::string>;
    using PartPair = std::pair<PartSet, PartSet>;
    using XYZPoint = ROOT::Math::XYZPoint;
    using XYZVector = ROOT::Math::XYZVector;
    using SearchTuple = std::tuple<std::string, int, XYZPoint>;
    using SearchPair = std::pair<int, XYZPoint>;

private:
    LayerMap fLayers;

public:
    void ReadFile(const std::string& file);
    void ReplaceWithMatrix(const std::string& name, SilMatrix* sm);
    void Print() const;

    SilLayer& GetLayer(const std::string& name) { return fLayers[name]; };
    LayerMap& GetLayers() { return fLayers; }
    bool CheckLayersExists(const std::string& name) const { return fLayers.count(name); }
    void EraseLayer(const std::string& name);
    // Classify given layer names by Light or Heavy particle
    PartPair ClassifyLayers(const std::vector<std::string>& names);

    // Simulation query functions
    SearchTuple FindLayerAndIdx(const XYZPoint& p, const XYZVector& v, bool verbose = false);
    SearchPair FindSPInLayer(const std::string& name, const XYZPoint& p, const XYZVector& v);
    // Drawing functions
    TVirtualPad* DrawGeo(double zoffset = 0, bool withActar = true);
};
} // namespace ActPhysics

#endif // !ActSilSpecs
