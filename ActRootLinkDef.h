#include <vector>
//#include <utility>
//#include <map>
#ifdef __CLING__

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

#pragma link C++ nestedclass;
#pragma link C++ nestedtypedef;
//////////////////////////////////////////////////////
////////////// BASE ACTROOT
#pragma link C++ class ActRoot + ;
#pragma link C++ class ActLegacy + ;

#pragma link C++ enum ChamberMode + ;
#pragma link C++ enum SiliconMode + ;
#pragma link C++ enum SiliconPanel + ;
//#pragma link C++ class std::map<std::pair<SiliconMode, SiliconPanel>, std::map<int, double>> + ;

#pragma link C++ class ChamberDetector + ;
#pragma link C++ class SiliconUnit + ;
#pragma link C++ class SiliconLayer + ;
#pragma link C++ class SiliconDetector + ;

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////// ANALYSIS ////////////////////////////////////////////
#pragma link C++ class ActHit + ;
#pragma link C++ class ActTrack + ;
#pragma link C++ class ActClusteringResults + ;
#pragma link C++ class ActLine + ;

#pragma link C++ namespace ActParameters ;

#pragma link C++ class ActSample + ;
#pragma link C++ enum SamplingMethod;

#pragma link C++ class ActRANSAC + ;

#pragma link C++ class ActDraw + ;

#pragma link C++ class ActCalibrations + ;

//structs
#pragma link C++ struct Silicons +;
#pragma link C++ struct TriggersAndGates +;
#pragma link C++ struct TimeOfFlight + ;
#pragma link C++ struct SiliconsPlus + ;
#pragma link C++ struct Voxels + ;
#pragma link C++ struct EventPlusCuts + ;
#pragma link C++ struct RunInfo + ;

//new classes
#pragma link C++ class ActEventPlus + ;

#pragma link C++ class ActTrackPlus + ;
#pragma link C++ class std::vector<ActTrackPlus> + ;

#pragma link C++ class ActDBSCAN + ;

#pragma link C++ class ActRunManager + ;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////// SIMULATION ////////////////////////////////////////////////
#pragma link C++ class SimKinematics + ;

#pragma link C++ class SimGeometry + ;

#pragma link C++ struct SilInfo + ;
#pragma link C++ struct DriftInfo + ;
#pragma link C++ struct IterationInfo + ;
#pragma link C++ struct ExperimentInfo + ;
#pragma link C++ struct SimulationParameters + ;

#pragma link C++ class SimRunner + ;

#pragma link C++ class SimSRIM + ;

#pragma link C++ class SimCrossSection + ;

#pragma link C++ class SimBeam + ;

#pragma link C++ class SimHistos + ;


#endif
