#ifndef ACTPARAMETERS_H
#define ACTPARAMETERS_H

//Colored output
#include <tuple>
#include <utility>
#define RESET   "\033[0m"
#define BLACK   "\033[30m"      /* Black */
#define RED     "\033[31m"      /* Red */
#define GREEN   "\033[32m"      /* Green */
#define YELLOW  "\033[33m"      /* Yellow */
#define BLUE    "\033[34m"      /* Blue */
#define MAGENTA "\033[35m"      /* Magenta */
#define CYAN    "\033[36m"      /* Cyan */
#define WHITE   "\033[37m"      /* White */
#define BOLDBLACK   "\033[1m\033[30m"      /* Bold Black */
#define BOLDRED     "\033[1m\033[31m"      /* Bold Red */
#define BOLDGREEN   "\033[1m\033[32m"      /* Bold Green */
#define BOLDYELLOW  "\033[1m\033[33m"      /* Bold Yellow */
#define BOLDBLUE    "\033[1m\033[34m"      /* Bold Blue */
#define BOLDMAGENTA "\033[1m\033[35m"      /* Bold Magenta */
#define BOLDCYAN    "\033[1m\033[36m"      /* Bold Cyan */
#define BOLDWHITE   "\033[1m\033[37m"      /* Bold White */

//A header containing constants
//Following constant prescription from: https://www.learncpp.com/cpp-tutorial/sharing-global-constants-across-multiple-files-using-inline-variables/

#include <TString.h>
#include <string>
#include <vector>
#include "Math/Point3D.h"
#include <map>

namespace ActParameters
{
    using XYZPoint = ROOT::Math::XYZPoint;
	//////////////////////////////////////////////
	///// PAD SIZES AND READ COBO VARIABLES //////
	//pad size
	inline constexpr int g_NPADX { 64};
	inline constexpr int g_NPADY { 32};
	inline constexpr int g_NPADZ { 512};
    inline constexpr int g_REBINZ {2};
    inline constexpr int g_NBINSZ { g_NPADZ / g_REBINZ};
	//pad side in mm
	inline constexpr double padSideLength { 2.};//mm
	
	//silicon 4 z side length in mm
	inline constexpr double zSiliconLength { 50.};//mm

	//MEvent-related parameters for DAQ
	//with g_ to avoid confict with MEvent defined values in preprocessor
	inline constexpr int g_NB_COBO { 18};
	inline constexpr int g_NB_ASAD { 4};
	inline constexpr int g_NB_AGET { 4};
	inline constexpr int g_NB_CHANNEL { 68};
	inline constexpr int g_NB_SAMPLES { 512};
	////////////////////////////////////////////

    ///SILICON PARAMETERS
    inline constexpr int NSilLeft0 {6};//first layer
    inline constexpr int NSilLeft1 {0};//second layer
    inline constexpr int NSilRight0{6};//1st
    inline constexpr int NSilRight1{0};//2nd
    inline constexpr int NSilFront0{0};//front layer 1
    inline constexpr int NSilFront1{0};//2nd
    inline constexpr int NSilBeam  {0};//beam silicons

    //strings passed when classifying ActTracks
	inline const std::string trackChamber { "chamber"};
	inline const std::string trackWindow  { "likelyWindow"};
	inline const std::string trackDump    { "likelyDump"};
	inline const std::string trackDelta   { "likelyDelta"};
	inline const std::string trackHitsSiliconSideLeft { "left"};
	inline const std::string trackHitsSiliconSideRight { "right"};
	inline const std::string trackHitsSiliconFront{ "front"};
	inline const std::string trackHitsSiliconOutside{ "outside"};
	/////////////////////////////////////////////////

    ///////// FIND PHYSICS OF TRACKS ////////////
	//number of pads above chamber boundaries where silicons are placed
    inline constexpr double NPadsSideLeft {70.0 / padSideLength};//7 cm to pads
    inline constexpr double NPadsSideRight{70.0 / padSideLength};

    inline const std::map<std::string, XYZPoint> siliconsPlacement {{trackHitsSiliconSideLeft,
            XYZPoint(0., ActParameters::g_NPADY - 1 + ActParameters::NPadsSideLeft, 0.)},
                                                                    {trackHitsSiliconSideRight,
                                                                     XYZPoint(0., -ActParameters::NPadsSideRight, 0.)},};
    inline const std::map<std::string, std::map<int, std::pair<double, double>>> siliconsXPlacement {//refered to our standard system
        {
            trackHitsSiliconSideLeft,
            {
                {1, {89.0, 139.0}},
                {2, {39.0, 89.0}},
                {3, {-11.0, 39.0}},
                {4, {-11.0, 39.0}},
                {5, {39.0, 89.0}},
                {6, {89.0, 139.0}}
            }
        },
        {
            trackHitsSiliconSideRight,
            {
                {1, {-11.0, 39.0}},
                {2, {39.0, 89.0}},
                {3, {89.0, 139.0}},
                {4, {89.0, 139.0}},
                {5, {39.0, 89.0}},
                {6, {-11.0, 39.0}},
            }
        },
    };

    //silicon layers to correct direction signs
    inline constexpr double silicon1X { (50.0 / padSideLength) / 2};//Y is in reality X dimension
    inline constexpr double silicon1Z {  static_cast<double>(g_NPADZ) / 2};//since we cant convert yet to distance
    //according to side, we can determine the correct slope sign for each component!
    inline std::map<std::string, std::map<int, std::pair<double, double>>> siliconDirection {
        {trackHitsSiliconSideLeft,{
                {1, std::make_pair(+5* silicon1X ,  +silicon1Z)},
                {2, std::make_pair(+3 * silicon1X , +silicon1Z)},
                {3, std::make_pair(+1 * silicon1X , +silicon1Z)},
                {4, std::make_pair(+1 * silicon1X , +silicon1Z)},
                {5, std::make_pair(+3 * silicon1X , +silicon1Z)},
                {6, std::make_pair(+5 * silicon1X , +silicon1Z)}}
        },
        {trackHitsSiliconSideRight, {
                {1, std::make_pair(+1 * silicon1X , +silicon1Z)},
                {2, std::make_pair(+3 * silicon1X , +silicon1Z)},
                {3, std::make_pair(+5 * silicon1X , +silicon1Z)},
                {4, std::make_pair(+5 * silicon1X , +silicon1Z)},
                {5, std::make_pair(+3 * silicon1X , +silicon1Z)},
                {6, std::make_pair(+1 * silicon1X , +silicon1Z)}
            }}
    };
    //SILICON ENERGY THRESHOLDS!
    inline std::map<std::string, std::vector<double>> g_SilSide0Thresholds {
        {trackHitsSiliconSideLeft, {0.8, 0.8, 0.8, 0.6, 0.6, 0.7}},
        {trackHitsSiliconSideRight, {1.1, 1.1, 2.1, 0.6, 0.8, 0.8}}
    };

	////////////////////////////////////////////
	///// CALIBRATION FILES PARAMETERS ////////
	//number of pads in calibration file!
	inline constexpr int NCoefRows { 17408};

	// size of vectors in silicon calibrations
	//Silicon array sizes
	inline constexpr int NrowsSi01 { 11};
	inline constexpr int NrowsSiS  { 8};
	inline constexpr int NrowsSiBeam { 3};
	inline constexpr int NcolsSi01S { 4};
	inline constexpr int NcolsSiBeam { 2};
	//in vector form
	inline const std::vector<std::string> SiCalNames {"0", "1", "S"};//constexpr does not work with string
	//const gives error with maps when iterate
	inline std::map<std::string, int> Si01SCalibrationRows = { {SiCalNames[0], NrowsSi01},
																  {SiCalNames[1], NrowsSi01},
																  {SiCalNames[2], NrowsSiS}};
	//Si Beam calibration thresholds
	inline const std::vector<double> SiBeamCalibrationThresholds { 0., 0., 0.};

	//Silicon data map string values
	inline const std::vector<std::string> siliconMapKeys {"M", "E", "P"};
	//stands for "M" = multiplicity, "E" = energy, "P" = Si array pad

	//values for thresholds in multiplicity (MeV?)
	// if ESi0 > this, multiplicity01F += 1
	inline constexpr double minESi0ToIncreaseMultiplicity { 1.}; //MeV
	// if ESiS > this, multiplicityS += 1
	inline constexpr double minESiSToIncreaseMultiplicity { 0.5}; //MeV
	//////////////////////////////////////////////

	/////////////////////////////////////////////
    inline constexpr double g_NPADSISIDE { 42.5}; //8.5 cm
	inline constexpr double g_NPADSSIFRONT { 50}; //10 cm

	//[temporary] position of beam plane in Y
	inline constexpr double beamPlaneY { static_cast<double>(g_NPADY) / 2};// = g_NPADY / 2
	
	
	/////////////////////////////////////////////////
	/////////// MEvent PARAMETERS ///////////////////
	//MEvent parameters names
	inline const std::vector<TString> VXINames {"SI_L", "SI_R",
        "INCONF", "GATCONF",
        "THF_SIR13", "THF_SIR46", "THF_SIL13", "THF_SIL46"};
	inline std::vector<int> VXINumbers {6, 6,
        0, 0,
        0, 0, 0, 0};
	
}

// namespace ActParameters
// {
// 	//////////////////////////////////////////////
// 	///// PAD SIZES AND READ COBO VARIABLES //////
// 	//pad size
// 	inline constexpr int g_NPADX { 128};
// 	inline constexpr int g_NPADY { 128};
// 	inline constexpr int g_NPADZ { 512};

// 	//pad side in mm
// 	inline constexpr double padSideLength { 2.};//mm
	
// 	//silicon 4 z side length in mm
// 	inline constexpr double zSiliconLength { 50.};//mm

// 	//MEvent-related parameters for DAQ
// 	//with g_ to avoid confict with MEvent defined values in preprocessor
// 	inline constexpr int g_NB_COBO { 18};
// 	inline constexpr int g_NB_ASAD { 4};
// 	inline constexpr int g_NB_AGET { 4};
// 	inline constexpr int g_NB_CHANNEL { 68};
// 	inline constexpr int g_NB_SAMPLES { 512};
// 	////////////////////////////////////////////

// 	////////////////////////////////////////////
// 	///// CALIBRATION FILES PARAMETERS ////////
// 	//number of pads in calibration file!
// 	inline constexpr int NCoefRows { 17408};

// 	// size of vectors in silicon calibrations
// 	//Silicon array sizes
// 	inline constexpr int NrowsSi01 { 11};
// 	inline constexpr int NrowsSiS  { 8};
// 	inline constexpr int NrowsSiBeam { 3};
// 	inline constexpr int NcolsSi01S { 4};
// 	inline constexpr int NcolsSiBeam { 2};
// 	//in vector form
// 	inline const std::vector<std::string> SiCalNames {"0", "1", "S"};//constexpr does not work with string
// 	//const gives error with maps when iterate
// 	inline std::map<std::string, int> Si01SCalibrationRows = { {SiCalNames[0], NrowsSi01},
// 																  {SiCalNames[1], NrowsSi01},
// 																  {SiCalNames[2], NrowsSiS}};
// 	//Si Beam calibration thresholds
// 	inline const std::vector<double> SiBeamCalibrationThresholds { 0., 0., 0.};

// 	//Silicon data map string values
// 	inline const std::vector<std::string> siliconMapKeys {"M", "E", "P"};
// 	//stands for "M" = multiplicity, "E" = energy, "P" = Si array pad

// 	//values for thresholds in multiplicity (MeV?)
// 	// if ESi0 > this, multiplicity01F += 1
// 	inline constexpr double minESi0ToIncreaseMultiplicity { 1.};
// 	// if ESiS > this, multiplicityS += 1
// 	inline constexpr double minESiSToIncreaseMultiplicity { 0.5};
// 	//////////////////////////////////////////////

// 	/////////////////////////////////////////////
// 	///////// FIND PHYSICS OF TRACKS ////////////
// 	//number of pads above chamber boundaries where silicons are placed
// 	inline constexpr double g_NPADSISIDE { 42.5}; //8.5 cm
// 	inline constexpr double g_NPADSSIFRONT { 50}; //10 cm

// 	//[temporary] position of beam plane in Y
// 	inline constexpr double beamPlaneY { 64.};// = g_NPADY / 2
	
// 	//strings passed when classifying ActTracks
// 	inline const std::string trackChamber { "chamber"};
// 	inline const std::string trackWindow  { "likelyWindow"};
// 	inline const std::string trackDump    { "likelyDump"};
// 	inline const std::string trackDelta   { "likelyDelta"};
// 	inline const std::string trackHitsSiliconSideLeft { "side_left"};
// 	inline const std::string trackHitsSiliconSideRight { "side_right"};
// 	inline const std::string trackHitsSiliconFront{ "front"};
// 	inline const std::string trackHitsSiliconOutside{ "outside"};
// 	/////////////////////////////////////////////////

// 	/////////////////////////////////////////////////
// 	/////////// MEvent PARAMETERS ///////////////////
// 	//MEvent parameters names
// 	inline const std::vector<TString> VXINames {"SI0_", "SI1_", "SIS_",
// 											  "SIBEAM_1", "SIBEAM_2", "SIBEAM_3",
// 											  "INCONF", "GATCONF",
// 											  "T_CFAval_HF", "T_CFAval_CATSD4", "T_CFAval_CATSD5",
// 											  "CLK_UP", "CLK", "DT_GET_CLK_UP", "DT_GET_CLK", "DT_VXI_CLK_UP", "DT_VXI_CLK",
// 											  "CTR_TIMEH_UP", "CTR_TIMEH", "CTR_TIMEML_UP", "CTR_TIMEML", "CTR_EVT_UP", "CTR_EVT",
// 											  "SCA_TiD3_UP", "SCA_TiD3", "SCA_CATSD4_UP", "SCA_CATSD4",
// 											  "SCA_CATSD5_UP", "SCA_CATSD5", "SCA_CFA_UP", "SCA_CFA",
// 											  "SCA_SIBeam1_UP", "SCA_SIBeam1", "SCA_SIBeam2_UP", "SCA_SIBeam2",
// 											  "SCA_OR_Si0_UP", "SCA_OR_Si0", "SCA_OR_SiS_UP", "SCA_OR_SiS",
// 											  "SCA_L1ok_UP", "SCA_L1ok"};
// 	inline std::vector<int> VXINumbers { 11, 11, 8,
// 										 0, 0, 0,
// 										 0, 0,
// 										 0, 0, 0,
// 										 0, 0, 0, 0, 0, 0,
// 										 0, 0, 0, 0, 0, 0,
// 										 0, 0, 0, 0,
// 										 0, 0, 0, 0,
// 										 0, 0, 0, 0,
// 										 0, 0, 0, 0,
// 										 0, 0, };
	
// }

#endif //ACTPARAMETERS_H
