#ifndef _MFMMutantFrame_
#define _MFMMutantFrame_
/*
  MFMMutantFrame.h
	Copyright Acquisition group, GANIL Caen, France
*/

#define MUT_HEADERFRAMESIZE 18
#define MUT_FRAMESIZE 60
#define MUT_NB_EVTCOUNT 4
#define MUT_NB_SCALER 5
#define MUT_NB_MULTIPLICITY 2

#include "MFMBlobFrame.h"

#pragma pack(push, 1) // force alignment
struct MFM_mut_eventInfo {
  char EventTime[6];
  unsigned EventIdx  : 32;
};

struct MFM_mut_data{
  uint16_t TriggerInfo ;
  uint16_t Mutiplicity[2];/// 0->A; 1->B;
  uint32_t EvtCount[4]; /// 0->L0 , 1->L1A , 2-> L1B  3->3L2_
  uint32_t Scaler[5];
  uint32_t D2pTime ;
};


struct MFM_mut_header{
	 MFM_common_header  MutBlobcHeader;
	 MFM_mut_eventInfo  MutEventInfo;
	 MFM_mut_data       MutData;
};


//____________MFMMutantFrame___________________________________________________________

class MFMMutantFrame : public MFMBlobFrame
{
public:
uint64_t fTimeStamp;
int32_t fEventNumber ;

public :

MFMMutantFrame();
MFMMutantFrame(int unitBlock_size, int dataSource,
	 		 int frameType, int revision, int frameSize,int headerSize);
virtual ~MFMMutantFrame();
virtual void SetPointers(void * pt =NULL);

virtual void SetBufferSize(int size, bool ifinferior) ;
virtual void SetAttributs(void * pt =NULL);
virtual uint64_t GetTimeStamp();
virtual uint64_t GetTimeStampAttribut();
virtual int GetEventNumberAttribut();
virtual int GetEventNumber();

virtual void SetTimeStamp(uint64_t timestamp);
virtual void SetEventNumber(int eventnumber);
virtual string GetHeaderDisplay(char* infotext=NULL);
// MUTANT

virtual void      SetTriggerInfo(uint16_t trig);
virtual uint16_t  GetTriggerInfo();

virtual void      SetMultiplicity(int i, uint16_t mult);
virtual uint16_t  GetMultiplicity(int i);

virtual void      SetEvtCount(int i, uint32_t count);
virtual uint32_t  GetEvtCount(int i);

virtual void      SetScaler(int i, uint32_t scaler);
virtual uint32_t  GetScaler(int i);

virtual void      SetD2pTime(uint32_t trig);
virtual uint32_t  GetD2pTime();

virtual void      FillEventRandomConst(uint64_t timestamp=0,uint32_t enventnumber=0);
virtual string    DumpData(char mode ='d', bool nozero=0);
};
#pragma pack(pop) // free aligment
#endif
