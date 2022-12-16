#include <vector>

#ifdef __CLING__

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

//classes inherited from TObject/TNamed
#pragma link C++ class ActHit + ;
#pragma link C++ class ActTrack + ;
#pragma link C++ class ActClusteringResults + ;
//not inherited from TObject -> Neither does it need a streamer (-) nor a >> operator (!)
#pragma link C++ class ActLine - !;


//recommended if we use namespaces
#pragma link C++ nestedclass;
#pragma link C++ nestedtypedef;

#pragma link C++ namespace ActParameters ;

//#pragma link C++ namespace RandomSampling;
//#pragma link C++ defined_in namespace RandomSampling;
#pragma link C++ class ActSample - !;
#pragma link C++ enum SamplingMethod;

//#pragma link C++ namespace SampleConsensus;
#pragma link C++ class ActRANSAC + ;

#pragma link C++ class ActDraw - !;

//ActRoot far from RANSAC
#pragma link C++ class ActCalibrations - !;

//structs generating I/O subsystem
#pragma link C++ struct Silicons +;
#pragma link C++ struct TriggersAndGates +;
#pragma link C++ struct TrackPhysics +;
#pragma link C++ struct EventInfo + ;
#pragma link C++ struct TimeOfFlight + ;
#pragma link C++ struct Analysis + ;
#pragma link C++ struct SiliconsPlus + ;
#pragma link C++ struct Voxels + ;
#pragma link C++ class std::vector<Analysis> + ;
#pragma link C++ class std::vector<TrackPhysics>+;//in order to generate I/O streamer to write std::vector of TrackPhysics

#pragma link C++ class ActTrackGeometry + ;
#pragma link C++ class std::vector<ActTrackGeometry> + ;

#pragma link C++ class ActEventPlus + ;

#pragma link C++ class ActTrackPlus + ;
#pragma link C++ class std::vector<ActTrackPlus> + ;

#pragma link C++ class ActEvent +;

#pragma link C++ class ActAnalyzer + ;

#pragma link C++ class ActSRIM + ;

#pragma link C++ class ActKinematics + ;

#endif
