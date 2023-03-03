#ifndef ACTUTILS_H
#define ACTUTILS_H

/*
namespace containing utility functions
 */
#include "TH1D.h"
#include "TVirtualPad.h"
#include "TCutG.h"

#include <string>
#include <map>
#include <set>

namespace ActUtils
{
    //Clean whitespaces in std::string
    void CleanWhitespaces(std::string& str);
    //Fit 1D histogram from both sides, intended for Silicon Countour
    void GetExperimentalTH1Contours(TH1D* proj, double minLeft, double maxLeft,
                                    double minRight, double maxRight, const std::string& opts = "");

    //Classes to manipulate cuts and histograms
    class ActCutsManager
    {
    public:
        std::map<std::string, TCutG*> fCuts {};
        std::set<std::string> fKeys {};
    
        ActCutsManager() = default;
        ~ActCutsManager() = default;

        void ReadGraphicalCut(const std::string& key,
                              const std::string& fileName,
                              bool verbose = false);

        void DrawCuts(TVirtualPad* pad);

        bool IsInside(const std::string& key, const double& x, const double& y);

        std::string IdentifyKey(const double& x, const double& y);
    };
}

#endif
