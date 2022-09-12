#ifndef _MFMExogamFrame_
#define _MFMExogamFrame_
/*
  MFMExogamFrame.h

	Copyright Acquisition group, GANIL Caen, France

*/
#define EXO_NB_OUTER 4
#define EXO_NB_INNER_M 2
#define EXO_NB_INNER_T 3
#define EXO_NB_STATUS 3
#define EXO_NB_MAX_PARA 17
#define EXO_FRAMESIZE 52
#define EXO_HEADERFRAMESIZE 18
#define EXO_TRIG_REQ_CRYS_ID_MASK 0x0000000f
#define EXO_NUMBER_CRISTAL_ID 2

#include "MFMBlobFrame.h"

#pragma pack(push, 1) // force alignment
struct MFM_exo_eventInfo {
  unsigned EventIdx  : 32;
  char EventTime[6];
};


struct MFM_exo_data{
  unsigned CristalId : 16;
  unsigned Status1   : 16;
  unsigned Status2   : 16;
  unsigned Status3   : 16;
  unsigned DeltaT    : 16;
  unsigned Inner6M   : 16;
  unsigned Inner20M  : 16;
  unsigned Outer1    : 16;
  unsigned Outer2    : 16;
  unsigned Outer3    : 16;
  unsigned Outer4    : 16;
  unsigned BGO       : 16;
  unsigned Csi       : 16;
  unsigned InnerT30  : 16;
  unsigned InnerT60  : 16;
  unsigned InnerT90  : 16;
  unsigned Padding   : 16;
};


struct MFM_exo_header{
	 MFM_common_header  ExoBlobcHeader;
	 MFM_exo_eventInfo  ExoEventInfo;
	 MFM_exo_data       ExoData;
};



//____________MFMExogamFrame___________________________________________________________

class MFMExogamFrame : public MFMBlobFrame
{
public:
uint64_t fTimeStamp;
int fEventNumber ;

public :

MFMExogamFrame();
MFMExogamFrame(int unitBlock_size, int dataSource,
	 		 int frameType, int revision, int frameSize,int headerSize);
virtual ~MFMExogamFrame();
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
// EXO

virtual void      ExoSetCristalId(uint16_t cristalId);
virtual uint16_t  ExoGetCristalId();
virtual uint16_t  ExoGetTGCristalId();
virtual void      ExoSetStatus(int i, uint16_t status);
virtual uint16_t  ExoGetStatus(int i);
virtual void      ExoSetDetaT(uint16_t detaT);
virtual uint16_t  ExoGetDeltaT();
virtual void      ExoSetInnerM(int i, uint16_t innner);
virtual uint16_t  ExoGetInnerM(int i);
virtual void      ExoSetOuter(int i, uint16_t outer);
virtual uint16_t  ExoGetOuter(int i);
virtual void      ExoSetBGO(uint16_t bgo);
virtual uint16_t  ExoGetBGO();
virtual void      ExoSetCsi(uint16_t Csi);
virtual uint16_t  ExoGetCsi();
virtual void      ExoSetInnerT(int i, uint16_t inner);
virtual uint16_t  ExoGetInnerT(int i);
virtual void      ExoSetPara(int i, uint16_t value);
virtual uint16_t  ExoGetPara(int i);
virtual void      ExoSetPadding(uint16_t padding);
virtual uint16_t  ExoGetPadding();
virtual void      FillEventRandomConst(uint64_t timestamp=0,uint32_t enventnumber=0);

};
#pragma pack(pop) // free aligment
#endif
