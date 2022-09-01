#ifdef __CINT__

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

#pragma link C++ namespace ActParameters ;

#pragma link C++ class ActHit + ;
#pragma link C++ class ActTrack + ;
#pragma link C++ class ActClusteringResults + ;

#pragma link C++ class ActLine - !;

#pragma link C++ namespace SampleConsensus;
#pragma link C++ class SampleConsensus::ActRANSAC - !;

#pragma link C++ class ActDraw - !;

#endif
