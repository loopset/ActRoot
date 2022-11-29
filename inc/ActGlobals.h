#ifndef ACTGLOBALS_H
#define ACTGLOBALS_H

#include <algorithm>
#include <stdexcept>
#include <vector>
#include <string>
#include <map>

#include "TString.h"
struct FullACTAR
{
    //////////////////////////////////////////////
	///// PAD SIZES AND READ COBO VARIABLES //////
	//pad size 
	 const int g_NPADX { 128};
	 const int g_NPADY { 128};
	 const int g_NPADZ { 512};

	//pad side in mm
	 const double padSideLength { 2.};//mm
	
	//silicon 4 z side length in mm
	 const double zSiliconLength { 50.};//mm

	//MEvent-related parameters for DAQ
	//with g_ to avoid confict with MEvent defined values in preprocessor
	 const int g_NB_COBO { 18};
	 const int g_NB_ASAD { 4};
	 const int g_NB_AGET { 4};
	 const int g_NB_CHANNEL { 68};
	 const int g_NB_SAMPLES { 512};
	////////////////////////////////////////////

	////////////////////////////////////////////
	///// CALIBRATION FILES PARAMETERS ////////
	//number of pads in calibration file!
	 const int NCoefRows { 17408};

	// size of vectors in silicon calibrations
	//Silicon array sizes
	 const int NrowsSi01 { 11};
	 const int NrowsSiS  { 8};
	 const int NrowsSiBeam { 3};
	 const int NcolsSi01S { 4};
	const int NcolsSiBeam { 2};
	//in vector form
	 const std::vector<std::string> SiCalNames {"0", "1", "S"};//const does not work with string
	//const gives error with maps when iterate
	const  std::map<std::string, int> Si01SCalibrationRows = { {SiCalNames[0], NrowsSi01},
																  {SiCalNames[1], NrowsSi01},
																  {SiCalNames[2], NrowsSiS}};
	//Si Beam calibration thresholds
	 const std::vector<double> SiBeamCalibrationThresholds { 0., 0., 0.};

	//Silicon data map string values
	 const std::vector<std::string> siliconMapKeys {"M", "E", "P"};
	//stands for "M" = multiplicity, "E" = energy, "P" = Si array pad

	//values for thresholds in multiplicity (MeV?)
	// if ESi0 > this, multiplicity01F += 1
	const double minESi0ToIncreaseMultiplicity { 1.};
	// if ESiS > this, multiplicityS += 1
	 const double minESiSToIncreaseMultiplicity { 0.5};
	//////////////////////////////////////////////

	/////////////////////////////////////////////
	///////// FIND PHYSICS OF TRACKS ////////////
	//number of pads above chamber boundaries where silicons are placed
	const double g_NPADSISIDE { 42.5}; //8.5 cm
	const double g_NPADSSIFRONT { 50}; //10 cm

	//[temporary] position of beam plane in Y
	 const double beamPlaneY { 64.};// = g_NPADY / 2
	
	//strings passed when classifying ActTracks
	 const std::string trackChamber { "chamber"};
	 const std::string trackWindow  { "likelyWindow"};
	 const std::string trackDump    { "likelyDump"};
	 const std::string trackDelta   { "likelyDelta"};
	 const std::string trackHitsSiliconSideLeft { "side_left"};
	 const std::string trackHitsSiliconSideRight { "side_right"};
	 const std::string trackHitsSiliconFront{ "front"};
	 const std::string trackHitsSiliconOutside{ "outside"};
	/////////////////////////////////////////////////

	/////////////////////////////////////////////////
	/////////// MEvent PARAMETERS ///////////////////
	//MEvent parameters names
	 const std::vector<TString> VXINames {"SI0_", "SI1_", "SIS_",
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
	 std::vector<int> VXINumbers { 11, 11, 8,
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
};

class ActGlobals
{
private:
    std::string mode;

    FullACTAR actar {};

    static ActGlobals& GetInstance(std::string* const s = nullptr)
    {
        static ActGlobals instance {s};
        return instance;
    }

    ActGlobals(std::string* const s)
        : mode(s ? std::move(*s) : std::string {})
    {
        if(!s)
        {
            throw std::runtime_error("ActGlobals holding ACTAR parameters is not initialized!");
        }
    }
    
    
};

#endif
