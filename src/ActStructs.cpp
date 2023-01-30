#include "ActStructs.h"

#include "ActParameters.h"
#include "ActCalibrations.h"

#include <TMath.h>
#include <iostream>
#include <ostream>
#include <string>
#include <utility>

Silicons::Silicons()
	: fSilLeft0(ActParameters::NSilLeft0), fSilLeft0_cal(ActParameters::NSilLeft0),
      fSilLeft1(ActParameters::NSilLeft1), fSilLeft1_cal(ActParameters::NSilLeft1),
      fSilRight0(ActParameters::NSilRight0), fSilRight0_cal(ActParameters::NSilRight0),
      fSilRight1(ActParameters::NSilRight1), fSilRight1_cal(ActParameters::NSilRight1),
      fSilFront0(ActParameters::NSilFront0), fSilFront0_cal(ActParameters::NSilFront0),
      fSilFront1(ActParameters::NSilFront1), fSilFront1_cal(ActParameters::NSilFront1),
      fSiBeam(ActParameters::NrowsSiBeam, 0.), fSiBeam_cal(ActParameters::NrowsSiBeam, 0.),
      fSilSide0({{"left", std::vector<double>(ActParameters::NSilLeft0)}, {"right", std::vector<double>(ActParameters::NSilRight0)}}),
      fSilSide0_cal({{"left", std::vector<double>(ActParameters::NSilLeft0)}, {"right", std::vector<double>(ActParameters::NSilRight0)}})
{
}

void Silicons::Print() const
{
	std::cout<<BOLDCYAN<<"=========== Event Silicons =========="<<RESET<<'\n';
	// std::cout<<"Side-> M: "<<fData["S"]["M"]<<" P: "<<fData["S"]["P"]<<" ES: "<<fData["S"]["ES"]<<'\n';
	// std::cout<<"Front-> M: "<<fData["01F"]["M"]<<" P: "<<fData["01F"]["P"]<<" E0: "<<fData["01F"]["E0"]<<" E1: "<<fData["01F"]["E1"]<<'\n';
    for(const auto& [side, vecRaw] : fSilSide0)
    {
        int silIndex {0};
        for(const auto& val : vecRaw)
        {
            std::cout<<"\tSide: "<<side<<" sil: "<<silIndex + 1<<" RAW: "<<val<<" Channel CAL: "<<fSilSide0_cal.at(side).at(silIndex)<<" MeV"<<'\n';

            silIndex++;
        }
    }
    std::cout<<"------------------------------------------------"<<'\n';
    std::cout<<"Left-> M: "<<fData.at("left").at("M")<<" P: "<<fData.at("left").at("P")<<" E: "<<fData.at("left").at("E")<<'\n';
	std::cout<<"Right-> M: "<<fData.at("right").at("M")<<" P: "<<fData.at("right").at("P")<<" E: "<<fData.at("right").at("E")<<'\n';
	std::cout<<"================================================"<<RESET<<'\n';
}

std::pair<std::string, int> Silicons::GetSilSideAndIndex() const
{
    //return first side with M = 1 and its index
    int silIndex {-1};
    std::string side {"none"};
    int counter {0};
    for(const auto& [key, innerMap] : fData)
    {
        if(innerMap.at("M") == 1)
        {
            counter++;
            if(counter <= 1)
            {
                side     = key;
                silIndex = innerMap.at("P");
            }
        }
    }
    if(counter <= 1)
        return {side, silIndex};
    else
        return {"both", -11};
}

void SiliconsPlus::Print() const
{
    std::cout<<BOLDGREEN<<"===== Silicons ====="<<RESET<<std::endl;
    for(const auto& [side, innerMap] : fData)
    {
        std::cout<<" "<<side<<" with E: "<<innerMap.at("E")<<" MeV and pad: "<<innerMap.at("P")<<'\n';   
    }
    std::cout<<BOLDGREEN<<"=================="<<RESET<<std::endl;
}

std::pair<std::string, int> SiliconsPlus::GetSilSideAndIndex() const
{
    //return first side with M = 1 and its index
    int silIndex {-1};
    std::string side {"none"};
    int counter {0};
    for(const auto& [key, innerMap] : fData)
    {
        if(innerMap.at("M") == 1)
        {
            counter++;
            if(counter <= 1)
            {
                side     = key;
                silIndex = innerMap.at("P");
            }
        }
    }
    if(counter <= 1)
        return {side, silIndex};
    else
        return {"both", -11};
}

void RunInfo::Print() const
{
    std::cout<<BOLDMAGENTA<<" == RunInfo =="<<'\n';
    std::cout<<" Total events : "<<fTotalRecordedEvents<<'\n';
    std::cout<<" Duration     : "<<fRunDuration<<" s"<<'\n';
    std::cout<<" DT for GET   : "<<fDT_GET<<" +-/ "<<fuDT_GET<<" %"<<'\n';
    std::cout<<" DT for VXI   : "<<fDT_VXI<<" +/- "<<fuDT_VXI<<" %"<<'\n';
    std::cout<<RESET<<std::endl<<std::flush;
}
