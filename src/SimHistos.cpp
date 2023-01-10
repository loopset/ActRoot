#include "SimHistos.h"

#include "TH1D.h"
#include "TH2D.h"
#include "TEntryListArray.h"

#include <vector>
#include <utility>

SimHistos::SimHistos(TTree*& tree, SimulationParameters*& par,
                     std::vector<std::pair<std::pair<std::string, std::string>,TH1D*>>* h1d,
                     std::vector<std::pair<std::pair<std::string, std::string>,TH2D*>>* h2d)
    : fTree(tree), parameters(par), fHistos1D(h1d), fHistos2D(h2d)
{
        
}

template<typename T>
void SimHistos::FillHistograms(std::vector<std::pair<std::pair<std::string, std::string>,T*>>*& histos, int oneIndex)
{
    for(const auto& [index, scalingOpts] : parameters->fSimuMap)
    {
        //for plotting only one energy
        if(oneIndex != -1 && index != oneIndex)
            continue;
        //iteration for each energy
        std::string energyCut {"iter.energyIndex == " + std::to_string(index)};
        fTree->Draw(">>energyEntryList", energyCut.c_str(), "entrylistarray");
        auto* energyEntryList { gDirectory->Get<TEntryListArray>("energyEntryList")};
        fTree->SetEntryList(energyEntryList);
        //iteration for each histogram
        for(const auto& [fillOpts, h] : *histos)
        {
            //clone histogram
            auto* hIter {(T*)h->Clone("hIter")};
            std::string hIterName { hIter->GetName()};
            std::string fill {fillOpts.first + ">>" + hIterName};
            if(!fillOpts.second.empty())
            {
                fTree->Draw(fill.c_str(), fillOpts.second.c_str(), "goff");
            }
            else
            {
                fTree->Draw(fill.c_str(), nullptr, "goff");
            }
            hIter->Scale(scalingOpts.second);
            h->Add(hIter);
            delete hIter;
        }
            

        fTree->SetEntryList(nullptr);
        delete energyEntryList;
    }
}

void SimHistos::FillAllHistograms(int index)
{
    if(fHistos1D && fHistos1D->size() > 0)
    {
        std::cout<<"Filling TH1D histograms"<<'\n';
        FillHistograms(fHistos1D, index);
    }
    if(fHistos2D && fHistos2D->size() > 0)
    {
        std::cout<<"Filling TH2D histograms"<<'\n';
        FillHistograms(fHistos2D, index);
    }
}
