#include "ActUtils.h"

#include "ActRoot.h"

#include "TF1.h"
#include "TFile.h"

#include <algorithm>
#include <cctype>

void ActUtils::CleanWhitespaces(std::string &str)
{
    str.erase(std::remove_if(str.begin(), str.end(),
                             [](unsigned char x){return std::isspace(x);}),
              str.end());
}

void ActUtils::GetExperimentalTH1Contours(TH1D* proj, double minLeft, double maxLeft, double minRight, double maxRight, const std::string& opts)
{
    bool enableLeft {minLeft != maxLeft};
    bool enableRight {minRight != maxRight};
    TF1* fLeft {}; TF1* fRight {};
    if(enableLeft)
    {
        fLeft = new TF1("fLeft", "0.5 * [0] * (1 + TMath::Erf((x - [1]) / (TMath::Sqrt(2.) * [2])))", 0., 300.);
        fLeft->SetParameter(0, 200);
        fLeft->SetParameter(1, (minLeft + maxLeft) / 2);
        fLeft->SetParameter(2, 2.);
        fLeft->SetRange(minLeft, maxLeft);
        proj->Fit(fLeft, ("RLME" + opts).c_str());
    }
    if(enableRight)
    {
        fRight = new TF1("f2", "0.5 * [0] * (1 - TMath::Erf((x - [1]) / (TMath::Sqrt(2.) * [2])))", 0., 300.);
        fRight->SetParameter(0, 200);
        fRight->SetParameter(1, (minRight + maxRight) / 2);
        fRight->SetParameter(2, 2.);
        fRight->SetRange(minRight, maxRight);
        proj->Fit(fRight, ("RLME" + opts + "+").c_str(), "same");
    }
    //print RESULTS
    std::cout<<BOLDMAGENTA<<"== Experimental Silicon Contours for "<<proj->GetTitle()<<" =="<<'\n';
    if(enableLeft)
    {
        std::cout<<" Left  p1 = "<<fLeft->GetParameter(1)<<" "<<proj->GetXaxis()->GetTitle()<<'\n';
        std::cout<<"       p2 = "<<fLeft->GetParameter(2)<<" "<<proj->GetXaxis()->GetTitle()<<'\n';
    }
    if(enableRight)
    {
        std::cout<<" Right p1 = "<<fRight->GetParameter(1)<<" "<<proj->GetXaxis()->GetTitle()<<'\n';
        std::cout<<"       p2 = "<<fRight->GetParameter(2)<<" "<<proj->GetXaxis()->GetTitle()<<'\n';
    }
    std::cout<<"===================================================="<<RESET<<std::endl;
}

void ActUtils::ActCutsManager::ReadGraphicalCut(const std::string& key,
                                                const std::string& fileName,
                                                bool verbose)
{
    auto assignColor = [this](){int val {(int)fCuts.size() + 1}; if(val == 0 || val == 10) return 46; else return val;};
    if(verbose)
        std::cout<<BOLDCYAN<<"ActCutsManager: Reading key: "<<key<<" in file: "<<fileName<<RESET<<'\n';
    auto* file {new TFile(fileName.c_str())};
    file->cd();
    if(!fCuts.count(key))
    {
        fCuts[key] = file->Get<TCutG>("CUTG");
        fKeys.insert(key);
        if(!fCuts.at(key))
            throw std::runtime_error("Error: nullptr in Get<TCutG>: check cut name in file");
        fCuts.at(key)->SetLineWidth(2);
        fCuts.at(key)->SetLineColor(assignColor());
        fCuts.at(key)->SetTitle(("cut_" + key).c_str());
    }
    else
    {
        throw std::runtime_error("Reading TCutG with reapeated key : " + key);
    }
    file->Close();
    delete file;
}

void ActUtils::ActCutsManager::DrawCuts(TVirtualPad *pad)
{
    pad->cd();
    for(auto& [key, cut] : fCuts)
    {
        cut->Draw("same");
    }
}

bool ActUtils::ActCutsManager::IsInside(const std::string &key, const double &x, const double &y)
{
    return fCuts.at(key)->IsInside(x, y);
}

std::string ActUtils::ActCutsManager::IdentifyKey(const double &x, const double &y)
{
    for(auto& [key,cut] : fCuts)
    {
        if(cut->IsInside(x, y))
        {
            return key;
        }
    }
    return std::string("none");
}
