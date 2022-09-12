#ifndef _MFMMergeFrame_
#define _MFMMergeFrame_
/*
  MFMMergeFrame.h

	Copyright Acquisition group, GANIL Caen, France

*/

#include "MFMBasicFrame.h"

#define MERGE_EN_HEADERSIZE 20
#define MERGE_TS_HEADERSIZE 24

#pragma pack(push, 1) // force alignment

struct MFM_Merge_ENeventInfo {
  unsigned eventIdx  : 32;
};
struct MFM_Merge_TSeventInfo {
	  char     eventTime[6];
	  uint32_t deltaTime;
	};


// Merge

struct MFM_Merge_ENheader{
	 MFM_basic_header      MergeBasicHeader;
	 MFM_Merge_ENeventInfo MergeEvtInfo;
};
struct MFM_Merge_TSheader{
	 MFM_basic_header      MergeBasicHeader;
	 MFM_Merge_TSeventInfo MergeEvtInfo;
};



//____________MFMMergeFrame___________________________________________________________

class MFMMergeFrame : public MFMBasicFrame
{

private:

uint32_t	fDeltaTime;

public :

MFMMergeFrame();
MFMMergeFrame(int unitBlock_size, int dataSource,
	 		 int frameType, int revision, int frameSize,int headerSize,
		       int itemSize, int nItems);
virtual ~MFMMergeFrame();

virtual string GetHeaderDisplay(char* infotext =NULL);
virtual void SetPointers(void * pt =NULL);

virtual void SetBufferSize(int size, bool ifinferior) ;
virtual void SetAttributs(void * pt =NULL);
virtual uint64_t GetTimeStampAttribut();
virtual uint64_t GetTimeStamp();
virtual uint32_t GetDeltaTimeAttribut();
virtual uint32_t GetDeltaTime();
virtual int  GetEventNumber();
virtual int  GetEventNumberAttibut();

virtual void SetDeltaTime(uint32_t deltatule);
virtual void SetTimeStamp(uint64_t timestamp);
virtual void SetEventNumber(int eventnumber);
virtual void AddFrame(MFMCommonFrame* frame);
virtual void ReadInFrame(MFMCommonFrame* frame);
virtual void ResetAdd();
virtual void ResetReadInMem();


};
#pragma pack(pop) // free alignment
#endif
