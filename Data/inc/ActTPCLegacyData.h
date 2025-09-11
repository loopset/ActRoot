#ifndef ActTPCLegacyData_h
#define ActTPCLegacyData_h

/*
  This class just holds the MEventReduced and ReducedData objects
*/
#include "Rtypes.h"

#include "TObject.h"

#include <vector>

class ReducedData : public TObject
{
public:
    ReducedData() { hasSaturation = false; };
    ~ReducedData() {};
    unsigned short globalchannelid; //!< 16 bit number: [ CoBo (5 bits: 0-15) | AsAd (2 bits: 0.-3) | AGET (2 bits: 0-3)
                                    //!< | Channel (7 bits: 0-127, but only 0-67 are real - 64 + 4 FPN) ]
    bool hasSaturation;
    std::vector<float> peakheight;
    std::vector<float> peaktime;
    ClassDef(ReducedData, 1);
};

class MEventReduced : public TObject
{
public:
    MEventReduced() {};
    ~MEventReduced() {};
    unsigned long int event;
    unsigned long int timestamp;
    std::vector<ReducedData> CoboAsad;
    ClassDef(MEventReduced, 1);
};


#endif
