#include "ActModularParameters.h"

#include "TString.h"

#include <algorithm>
#include <fstream>
#include <iostream>

std::string ActRoot::ModularParameters::GetName(int vxi)
{
    auto where {fVXI.find(vxi) != fVXI.end()};
    if(where)
        return fVXI[vxi];
    else
        return "";
}

int ActRoot::ModularParameters::GetVXIOf(const std::string& name)
{
    auto it {std::find_if(fVXI.begin(), fVXI.end(), [&](const auto& p) { return p.second == name; })};
    if(it != fVXI.end())
        return it->first;
    return -11;
}

void ActRoot::ModularParameters::ReadActions(const std::vector<std::string>& names, const std::string& file)
{
    std::ifstream streamer {file};
    if(!streamer)
        throw std::runtime_error("ModPars::ReadActions(): cannot open ACTION file " + file);
    TString key {};
    int vxi {};
    int aux0 {};
    int aux1 {};
    while(streamer >> key >> vxi >> aux0 >> aux1)
    {
        for(int i = 0; i < names.size(); i++)
        {
            if(key == names[i])
            {
                fVXI[vxi] = names[i];
                break;
            }
        }
    }
}

void ActRoot::ModularParameters::Print() const
{
    std::cout << "==== ModularParameters ====" << '\n';
    for(const auto& [key, val] : fVXI)
    {
        std::cout << "-- VXI: " << key << " contains Modular " << val << '\n';
    }
    std::cout << "=======================" << '\n';
}
