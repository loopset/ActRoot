#ifndef ACTPARAMETERS_H
#define ACTPARAMETERS_H

//Colored output
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

#include <string>
#include <vector>

namespace ActParameters
{
	//pad size
	inline constexpr int NPADX { 128};
	inline constexpr int NPADY { 128};
	inline constexpr int NPADZ { 512};

	//MEvent-related parameters for DAQ
	//with g_ to avoid confict with MEvent defined values in preprocessor
	inline constexpr int g_NB_COBO { 18};
	inline constexpr int g_NB_ASAD { 4};
	inline constexpr int g_NB_AGET { 4};
	inline constexpr int g_NB_CHANNEL { 68};
	inline constexpr int g_NB_SAMPLES { 512};

	//number of pads in calibration file!
	inline constexpr int NCoefRows { 17408};

	//Silicon array sizes
	inline constexpr int NrowsSi01 { 11};
	inline constexpr int NrowsSiS  { 8};

	//MEvent parameters names
	inline std::vector<std::string> VXINames {"SI0_", "SI1_", "SIS_",
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
