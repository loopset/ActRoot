#ifdef __CLING__

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

//classes inherited from TObject/TNamed
#pragma link C++ class ActHit + ;
#pragma link C++ class ActTrack + ;
#pragma link C++ class ActClusteringResults + ;
//not inherited from ROOT -> Neither does it need a streamer (-) nor a >> operator (!)
#pragma link C++ class ActLine - !;

//recommended if we use namespaces
#pragma link C++ nestedclass;
#pragma link C++ nestedtypedef;

#pragma link C++ namespace ActParameters ;

#pragma link C++ namespace RandomSampling ;
#pragma link C++ class RandomSampling::ActSample - !;
#pragma link C++ enum RandomSampling::SamplingMethod;

#pragma link C++ namespace SampleConsensus;
#pragma link C++ class SampleConsensus::ActRANSAC - !;

#pragma link C++ class ActDraw - !;

#endif
