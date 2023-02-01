#ifndef ACTLEGACY_H
#define ACTLEGACY_H

#include "ActStructs.h"
#include "ActEventData.h"
#include "TString.h"

#include <unordered_map>
#include <string>
#include <utility>
#include <vector>

class ActLegacy//legacy functions to communicate with Thomas' classes
{
public:
    std::unordered_map<std::string, std::pair<int, int>> fVXIActions {};//including indexes!
        
    ActLegacy() = default;
    ~ActLegacy() = default;

    void ReadVXI(const std::string& file);
    void MoveIteratorToItsClass(int it, double val,
                                SiliconRawData& silicons, TriggersAndGates& triggers);
    //to initialize MEvent and MEventReduced
    std::pair<std::vector<TString>, std::vector<int>> GetParNamesAndNumbers() const;
};
#endif
