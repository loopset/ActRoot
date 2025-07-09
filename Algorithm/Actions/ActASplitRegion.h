#ifndef ActASplitRegion_h
#define ActASplitRegion_h

#include "ActInputParser.h"
#include "ActRegion.h"
#include "ActVAction.h"

#include <memory>
#include <unordered_map>

namespace ActAlgorithm
{
namespace Actions
{
class SplitRegion : public VAction
{
public:
    using RegionMap = std::unordered_map<ActRoot::RegionType, ActRoot::Region>; //!< A map holding all defined regions
    using BrokenVoxels = std::vector<std::vector<ActRoot::Voxel>>;              //!< Voxels broken into regions
private:
    RegionMap fRegions;
    int fMinVoxelsAfterBreak {}; //!< Minimum number of voxels after breaking into regions

public:
    SplitRegion() : VAction("SplitRegion") {};
    void ReadConfiguration(std::shared_ptr<ActRoot::InputBlock> block) override;
    void Run() override;
    void Print() const override;

    // Functions intrinsic to this action
    const RegionMap& GetRegions() const { return fRegions; }

private:
    void AddRegion(unsigned int i, const std::vector<double>& vec);
    void CheckRegionsReadout();
    void BreakIntoRegions();
    ActRoot::RegionType AssignClusterToRegion(ActRoot::Cluster& cluster);
    bool BreakCluster(ActRoot::Cluster& cluster, BrokenVoxels& brokenVoxels);
    ActRoot::RegionType AssignVoxelToRegion(const ActRoot::Voxel& voxel);
    void ProcessNotBeam(BrokenVoxels& brokenVoxels);
    void ResetID();
};
} // namespace Actions
} // namespace ActAlgorithm
#endif
