#ifndef SIMHISTOS_H
#define SIMHISTOS_H

#include "SimRunner.h"
#include "TTree.h"
#include "TH1D.h"
#include "TH2D.h"

#include <algorithm>
#include <map>
#include <string>
#include <utility>
#include <vector>
class SimHistos
{
private:
    TTree* fTree {};
    SimulationParameters* parameters {};

    //histograms by pointer
    std::vector<std::pair<std::pair<std::string, std::string>,TH1D*>>* fHistos1D {};
    std::vector<std::pair<std::pair<std::string, std::string>,TH2D*>>* fHistos2D {};
        
public:
    SimHistos(TTree*& tree, SimulationParameters*& par,
              std::vector<std::pair<std::pair<std::string, std::string>,TH1D*>>* h1d,
              std::vector<std::pair<std::pair<std::string, std::string>,TH2D*>>* h2d);
    ~SimHistos() = default;

    template<typename T>
    void FillHistograms(std::vector<std::pair<std::pair<std::string, std::string>,T*>>*& histos, int oneIndex);

    void FillAllHistograms(int index = -1);
};

#endif
