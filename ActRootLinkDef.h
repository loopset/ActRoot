#include <vector>
#include <utility>
#ifdef __CLING__

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

#pragma link C++ nestedclass;
#pragma link C++ nestedtypedef;
//////////////////////////////////////////////////////
////////////// BASE ACTROOT /////////////////////////
#pragma link C++ class ActRoot + ;
#pragma link C++ class ActLegacy + ;

#pragma link C++ enum ChamberMode + ;
#pragma link C++ enum SiliconMode + ;
#pragma link C++ enum SiliconPanel + ;
#pragma link C++ class std::pair<SiliconMode, SiliconPanel> + ;

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

//Event Data
#pragma link C++ struct Silicons +;
#pragma link C++ class TriggersAndGates +;
#pragma link C++ class TimeOfFlight + ;
#pragma link C++ struct SiliconsPlus + ;
#pragma link C++ class Voxels + ;
#pragma link C++ struct EventPlusCuts + ;
#pragma link C++ class RunInfo + ;
#pragma link C++ class SiliconRawData + ;
#pragma link C++ class SiliconData + ;

#pragma link C++  class ActEvent + ;

//new classes
#pragma link C++ class ActEventPlus + ;

#pragma link C++ class ActTrackPlus + ;
#pragma link C++ class std::vector<ActTrackPlus> + ;

#pragma link C++ class ActDBSCAN + ;

#pragma link C++ class ActRunManager + ;

///FILTERS
//#pragma link C++ class ActStatisticalRemovalFilter + ;

#endif
