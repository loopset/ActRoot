#ifndef ActMergerData_h
#define ActMergerData_h

#include "ActVData.h"

#include "Rtypes.h"

#include "TH1.h"

#include "Math/Point3D.h"
#include "Math/Point3Dfwd.h"
#include "Math/Vector3D.h"
#include "Math/Vector3Dfwd.h"

#include <ostream>
#include <string>
#include <vector>

namespace ActRoot
{
//! A small class describing a Light or Heavy track in the Merger detector
/*!
    Does not inherit from VData bc is a small struct to organize MergerData better
*/
class BinaryData
{
public:
    using XYZPointF = ROOT::Math::XYZPointF;
    using XYZVectorF = ROOT::Math::XYZVectorF;
    XYZPointF fSP {-1, -1, -1};
    std::vector<std::string> fLayers {};
    std::vector<float> fEs {};
    std::vector<int> fNs {};
    float fTL {-1};
    float fQtotal {-1};
    float fQave {-1};

    bool IsFilled() const;
    bool HasSP() const;
    bool IsL1() const;
    int GetNLayers() const;
    std::string GetLayer(int idx) const;
    void Print(int n = 3) const;

    ClassDef(BinaryData, 1);
};

//! Ouput of the MergerDetector
class MergerData : public VData
{
public:
    using XYZPointF = ROOT::Math::XYZPointF;
    using XYZVectorF = ROOT::Math::XYZVectorF;

public:
    // Profiles
    TH1F fQprojX {};
    TH1F fQProf {};
    // Points
    XYZPointF fWP {-1, -1, -1};     //!< Beam entrance point
    XYZPointF fRP {-1, -1, -1};     //!< Reaction point
    XYZPointF fSP {-1, -1, -1};     //!< LEGACY: silicon point of Light
    XYZPointF fBP {-1, -1, -1};     //!< Boundary point of Light
    XYZPointF fBSP {-1, -1, -1};    //!< Beam stopping points: hint of RP from Qx profile
    XYZPointF fBraggP {-1, -1, -1}; //!< Range of Light from its Q profile
    // Silicons
    std::vector<std::string> fSilLayers {}; //!< LEGACY
    std::vector<float> fSilEs {};           //!< LEGACY
    std::vector<float> fSilNs {};           //!< LEGACY
    // Flag for debugging purpouses
    std::string fFlag {}; //!< Describes at which point the merger stopped
    // Track length
    float fTrackLength {-1}; //!< LEGACY
    // Angles
    float fThetaBeam {-1};
    float fThetaBeamZ {-1}; // emittance angle along Z
    float fPhiBeamY {-1};   // emittance angle along Y
    float fThetaLight {-1};
    float fThetaDebug {-1};
    float fThetaLegacy {-1}; // this is just fThetaLight but will not be corrected in CorrDetector
    float fThetaHeavy {-1};
    float fPhiLight {-1};
    float fPhiHeavy {-1};
    // Q of light
    float fQave {-1}; //!< LEGACY
    // New as of July 2025: BinaryData classes to organize data
    BinaryData fLight {}; //!< Hit information of Light particle
    BinaryData fHeavy {}; //!< Hit information of Heavy particle
    // Indexes of clusters in TPCData after filtering, so any information can be recovered
    int fBeamIdx {-1};
    int fLightIdx {-1};
    int fHeavyIdx {-1};
    // Unique ID of the event
    int fEntry {-1};
    int fRun {-1};

    void Clear() override;
    void Print() const override;
    void Stream(std::ostream& streamer) const;

    ClassDefOverride(MergerData, 2);
};
} // namespace ActRoot

#endif // !ActMergerData_h
