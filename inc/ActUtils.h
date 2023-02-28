#ifndef ACTUTILS_H
#define ACTUTILS_H

/*
namespace containing utility functions
 */
#include "TH1D.h"

#include <string>

namespace ActUtils
{
    //Clean whitespaces in std::string
    void CleanWhitespaces(std::string& str);
    //Fit 1D histogram from both sides, intended for Silicon Countour
    void GetExperimentalTH1Contours(TH1D* proj, double minLeft, double maxLeft,
                                    double minRight, double maxRight, const std::string& opts = "");
}

#endif
