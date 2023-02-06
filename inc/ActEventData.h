#ifndef ACTEVENTDATA_H
#define ACTEVENTDATA_H

//SILICON DATA

#include "ActDetectors.h"
#include <map>
#include <string>
#include <utility>
#include <vector>

class SiliconData
{
public:
    std::map<std::pair<SiliconMode, SiliconPanel>, std::map<std::string, double>> fData {};

    void Print() const;
};

class SiliconRawData
{
public:
    std::map<std::pair<SiliconMode, SiliconPanel>, std::map<int, double>> fRaw {};
    std::map<std::pair<SiliconMode, SiliconPanel>, std::map<int, double>> fCal {};
    
    // std::map<int, double> fSilFront0 {};std::map<int, double>  fSilFront0_cal {};
    // std::map<int, double> fSilFront1; std::map<int, double> fSilFront1_cal {};
    // std::map<int, double> fSilRight0 {}; std::map<int, double> fSilRight0_cal {};
    // std::map<int, double> fSilRight1 {}; std::map<int, double> fSilRight1_cal {};
    // std::map<int, double> fSilLeft0 {}; std::map<int, double> fSilLeft0_cal {};
    // std::map<int, double> fSilLeft1 {}; std::map<int, double> fSilLeft1_cal {};

    void Calibrate(const std::map<int, std::vector<double>>& calibration,
                   const std::map<int, double>& raw,
                   std::map<int, double>& cal);
    void ReadAndWrite(SiliconData& fin, bool individualThreshold = false);
};
#endif
