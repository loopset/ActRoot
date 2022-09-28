#ifndef ACTPARAMETERS_H
#define ACTPARAMETERS_H

//Colored output
#include <map>
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
#include <map>

namespace ActParameters
{
	//////////////////////////////////////////////
	///// PAD SIZES AND READ COBO VARIABLES //////
	//pad size
	inline constexpr int g_NPADX { 128};
	inline constexpr int g_NPADY { 128};
	inline constexpr int g_NPADZ { 512};

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
	inline constexpr double minESi0ToIncreaseMultiplicity { 1.};
	// if ESiS > this, multiplicityS += 1
	inline constexpr double minESiSToIncreaseMultiplicity { 0.5};
	//////////////////////////////////////////////

	/////////////////////////////////////////////
	///////// FIND PHYSICS OF TRACKS ////////////
	//number of pads above chamber boundaries where silicons are placed
	inline constexpr double g_NPADSISIDE { 50}; //10 cm
	inline constexpr double g_NPADSSIFRONT { 50}; //10 cm

	//[temporary] position of beam plane in Y
	inline constexpr double beamPlaneY { 64.};// = g_NPADY / 2
	
	//strings passed when classifying ActTracks
	inline const std::string trackChamber { "chamber"};
	inline const std::string trackWindow  { "likelyWindow"};
	inline const std::string trackDump    { "likelyDump"};
	inline const std::string trackDelta   { "likelyDelta"};
	inline const std::string trackHitsSiliconSideLeft { "side_left"};
	inline const std::string trackHitsSiliconSideRight { "side_right"};
	inline const std::string trackHitsSiliconFront{ "front"};
	inline const std::string trackHitsSiliconOutside{ "outside"};
	/////////////////////////////////////////////////

	/////////////////////////////////////////////////
	/////////// MEvent PARAMETERS ///////////////////
	//MEvent parameters names
	inline const std::vector<TString> VXINames {"SI0_", "SI1_", "SIS_",
											  "SIBEAM_1", "SIBEAM_2", "SIBEAM_3",
											  "INCONF", "GATCONF",
											  "T_CFAval_HF", "T_CFAval_CATSD4", "T_CFAval_CATSD5",
											  "CLK_UP", "CLK", "DT_GET_CLK_UP", "DT_GET_CLK", "DT_VXI_CLK_UP", "DT_VXI_CLK",
											  "CTR_TIMEH_UP", "CTR_TIMEH", "CTR_TIMEML_UP", "CTR_TIMEML", "CTR_EVT_UP", "CTR_EVT",
											  "SCA_TiD3_UP", "SCA_TiD3", "SCA_CATSD4_UP", "SCA_CATSD4",
											  "SCA_CATSD5_UP", "SCA_CATSD5", "SCA_CFA_UP", "SCA_CFA",
											  "SCA_SIBeam1_UP", "SCA_SIBeam1", "SCA_SIBeam2_UP", "SCA_SIBeam2",
											  "SCA_OR_Si0_UP", "SCA_OR_Si0", "SCA_OR_SiS_UP", "SCA_OR_SiS",
											  "SCA_L1ok_UP", "SCA_L1ok"};
	inline std::vector<int> VXINumbers { 11, 11, 8,
										 0, 0, 0,
										 0, 0,
										 0, 0, 0,
										 0, 0, 0, 0, 0, 0,
										 0, 0, 0, 0, 0, 0,
										 0, 0, 0, 0,
										 0, 0, 0, 0,
										 0, 0, 0, 0,
										 0, 0, 0, 0,
										 0, 0, };
	
}

#endif //ACTPARAMETERS_H
