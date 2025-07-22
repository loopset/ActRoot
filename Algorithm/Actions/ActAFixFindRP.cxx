#include "ActAFixFindRP.h"

#include "ActColors.h"
#include "ActMultiAction.h"
#include "ActTPCData.h"

void ActAlgorithm::Actions::FixFindRP::ReadConfiguration(std::shared_ptr<ActRoot::InputBlock> block)
{
    fIsEnabled = block->GetBool("IsEnabled");
    if(!fIsEnabled)
        return;
    if(block->CheckTokenExists("IntruderInBeamRegion"))
        fIntruderInBeamRegion = block->GetBool("IntruderInBeamRegion");
}

void ActAlgorithm::Actions::FixFindRP::Run()
{
    if(!fIsEnabled)
        return;
    // INFO: workaround. this action is intended to be called only from the other FindRP action
    // so far there is no way of enabling and disabling actions depending on what calls them
    // but this one we can automatically disable it outside FindRP if we put it before FindRP in the multiaction.conf
    // by imposing that no RP should exist then

    if(!fTPCData->fRPs.size())
        return;
    // Intruder in beam region only if SplitRegion is in use
    if(fIntruderInBeamRegion && fMultiAction->HasAction("SplitRegion"))
        IntruderInBeamRegion();
}

void ActAlgorithm::Actions::FixFindRP::IntruderInBeamRegion()
{
    // This function solves the issue of small parts of heavy/light remaining within the Beam region
    // after the RP separation

    // 1-> Count cluster outside Beam region and locate it of intruder in Beam
    std::vector<ClusterIt> outies;
    ClusterIt intruder {};
    bool treat {};
    for(auto it = fTPCData->fClusters.begin(); it != fTPCData->fClusters.end(); it++)
        if(!it->GetIsBeamLike())
        {
            outies.push_back(it);
            if(it->GetRegionType() == ActRoot::RegionType::EBeam)
            {
                treat = true;
                intruder = it;
            }
        }
    // 2-> In case two clusters outside Beam region and one intruder
    if(outies.size() == 2 && treat)
    {
        // 1-> Get larger cluster as the front
        std::sort(outies.begin(), outies.end(),
                  [](const ClusterIt& l, const ClusterIt& r) { return l->GetSizeOfVoxels() > r->GetSizeOfVoxels(); });
        // 2-> Split
        ClusterIt big {outies.front()};
        ClusterIt small {outies.back()};
        auto& main {big->GetRefToVoxels()};
        auto& del {small->GetRefToVoxels()};
        main.insert(main.end(), del.begin(), del.end());
        // 3-> Refit
        big->ReFit();
        big->ReFillSets();
        // 4-> Delete from vector
        if(fIsVerbose)
        {
            std::cout << BOLDYELLOW << "---- FixFindRP::IntruderInBeam ----" << '\n';
            std::cout << "  Forcing MERGE of spureous cluster : " << '\n';
            small->Print();
            std::cout << RESET << '\n';
        }
        // Actually erase
        fTPCData->fClusters.erase(small);
    }
    else if(outies.size() > 2 && treat) // Difficult to assign to Out-Beam clusters. Automatically delete it
    {
        if(fIsVerbose)
        {
            std::cout << BOLDYELLOW << "---- FixFindRP::IntruderInBeam ----" << '\n';
            std::cout << "  Forcing DELETE of spureous cluster : " << '\n';
            intruder->Print();
            std::cout << RESET << '\n';
        }
        // Actually erase
        fTPCData->fClusters.erase(intruder);
    }
    else
    {
        ;
    }
}

void ActAlgorithm::Actions::FixFindRP::Print() const
{
    std::cout << BOLDCYAN << "····· " << GetActionID() << " ·····" << '\n';
    if(!fIsEnabled)
    {
        std::cout << "······························" << RESET << '\n';
        return;
    }
    std::cout << "  IntruderInBeamRegion : " << std::boolalpha << fIntruderInBeamRegion << '\n';
    std::cout << "······························" << RESET << '\n';
}
