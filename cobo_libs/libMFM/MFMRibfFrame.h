#ifndef _MFMRibfFrame_
#define _MFMRibfFrame_
/*
  MFMRibfFrame.h

	Copyright Acquisition group, GANIL Caen, France

*/

#define MUT_HEADERFRAMESIZE 18
#define MUT_NB_STATUS
#include "MFMBlobFrame.h"

#pragma pack(push, 1) // force alignment

struct MFM_Ribf_eventInfo {
  char EventTime[6];
  unsigned EventIdx  : 32;
};

struct MFM_Ribf_data{

};

struct MFM_Ribf_header{
	 MFM_common_header   RibfBlobcHeader;
	 MFM_Ribf_eventInfo  RibfEventInfo;
	 MFM_Ribf_data       RibfData;
};


//____________MFMRibfFrame___________________________________________________________

class MFMRibfFrame : public MFMBlobFrame
{
public:
uint64_t fTimeStamp;
int fEventNumber ;

public :

MFMRibfFrame();
MFMRibfFrame(int unitBlock_size, int dataSource,
	 		 int frameType, int revision, int frameSize,int headerSize);
virtual ~MFMRibfFrame();
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

virtual void FillEventRandomConst(uint64_t timestamp=0,uint32_t enventnumber=0);

};
#pragma pack(pop) // free aligment
#endif
