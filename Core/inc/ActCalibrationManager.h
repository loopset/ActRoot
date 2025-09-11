#ifndef ActCalibrationManager_h
#define ActCalibrationManager_h

/*
Singleton class holding the calibrations for all the detectors!
*/

#include <map>
#include <string>
#include <tuple>
#include <unordered_map>
#include <utility>
#include <vector>

namespace ActRoot
{
//! Class managing all calibrations (NO LONGER a singleton)
/*!
  Specific methods are implemented for LT and Pad alignment,
  but the other calibrations work the same as nptools
 */
class CalibrationManager
{
private:
    std::unordered_map<std::string, std::vector<double>> fCalibs; //!< General map holding strings as keys for the
                                                                  //!< vector of doubles (coeffs) of calib
    std::map<std::pair<int, int>, std::tuple<int, int, int, int>> fInvertedLT; //<! Special: inverted pad plane LT table
    std::vector<std::vector<int>> fLT;          //!< Special for Look up table on pad plane
                                                //!< LookUpTable format: [CoBo #: 0-15] [AsAd #: 0-3] [AGET #: 0-3]
                                                //!< [Channel #: 64+4 FPN channels] [X coord: 0-127] [Y coord: 0-127]
    std::vector<std::vector<double>> fPadAlign; //!< Pad align coefficiens
    std::vector<std::string> fFiles;            //!< List of files read in calibration
    bool fIsEnabled {true};                     //!< Whether to enable or not when calling ApplyCalibration/Threshold

public:
    CalibrationManager() = default;                 //<! Default constructor
    CalibrationManager(const std::string& calfile); //!< Construct and read a standard calibration file at once
    ~CalibrationManager() = default;                //<! Destructor

    // Actar: needs improvements but that depends on .txt file format (need to add keys to parameters)
    void ReadCalibration(const std::string& file);
    void ReadLookUpTable(const std::string& file);
    void ReadInvertedLookUpTable(const std::string& file);
    void ReadPadAlign(const std::string& file);

    // Apply methods
    double ApplyCalibration(const std::string& key, double raw);
    bool ApplyThreshold(const std::string& key, double raw, double nsigma = 1);
    int ApplyLookUp(int channel, int col);
    std::tuple<int, int, int, int> ApplyInvLookUp(int x, int y);
    double ApplyPadAlignment(int channel, double q);

    // Setters
    void SetIsEnabled(bool enabled) { fIsEnabled = enabled; }

    // Getters
    bool GetIsEnabled() const { return fIsEnabled; }

    void Print() const;
};
} // namespace ActRoot

#endif
