#ifndef ACTEVENTDATA_H
#define ACTEVENTDATA_H

//SILICON DATA
#include <map>
class SiliconRawData
{
public:
    std::map<int, double> fSilFront0 {};std::map<int, double>  fSilFront0_cal {};
    std::map<int, double> fSilFront1; std::map<int, double> fSilFront1_cal {};
    std::map<int, double> fSilRight0 {}; std::map<int, double> fSilRight0_cal {};
    std::map<int, double> fSilRight1 {}; std::map<int, double> fSilRight1_cal {};
    std::map<int, double> fSilLeft0 {}; std::map<int, double> fSilLeft0_cal {};
    std::map<int, double> fSilLeft1 {}; std::map<int, double> fSilLeft1_cal {};

    void Calibrate();
};

#endif
