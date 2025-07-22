#ifndef ActAFixFindRP_h
#define ActAFixFindRP_h
#include "ActCluster.h"
#include "ActVAction.h"

#include <vector>

namespace ActAlgorithm
{
namespace Actions
{
class FixFindRP : public VAction
{
public:
    // types are usually defined here
    using ClusterIt = std::vector<ActRoot::Cluster>::iterator;

private:
    bool fIntruderInBeamRegion {};

public:
    FixFindRP() : VAction("FixFindRP") {}

    void ReadConfiguration(std::shared_ptr<ActRoot::InputBlock>) override;
    void Run() override;
    void Print() const override;

private:
    void IntruderInBeamRegion();
};
} // namespace Actions
} // namespace ActAlgorithm


#endif // !ActAFixFindRP_h
