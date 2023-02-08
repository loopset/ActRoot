#ifndef ACTLEGACY_H
#define ACTLEGACY_H

#include "ActEventData.h"
#include "TString.h"

#include <string>
#include <utility>
#include <vector>

class ActLegacy//legacy functions to communicate with Thomas' classes
{
public:
    std::vector<std::pair<std::string, std::pair<int, int>>> fVXIActions {};//vector bc we want to keep the order from the .dat file!
        
    ActLegacy() = default;
    ~ActLegacy() = default;

    void ReadVXI(const std::string& file);
    void MoveIteratorToItsClass(int it, double val,
                                SiliconRawData& silicons,
                                TriggersAndGates& triggers,
                                TimeOfFlight& tof);
    //to initialize MEvent and MEventReduced
    std::pair<std::vector<TString>, std::vector<int>> GetParNamesAndNumbers() const;
    void Print() const;
};
#endif
