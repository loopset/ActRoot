#ifndef _MFMEbyedatFrame_
#define _MFMEbyedatFrame_
/*
  MFMEbyedatFrame.h

	Copyright Acquisition group, GANIL Caen, France

*/

#include "MFMBasicFrame.h"
#include<map>
#include <vector>
#define EBYEDAT_EN_HEADERSIZE 20
#define EBYEDAT_TS_HEADERSIZE 22
#define EBYEDAT_ENTS_HEADERSIZE 26

#pragma pack(push, 1) // force alignment

struct MFM_Ebyedat_ENeventInfo {
  unsigned eventIdx  : 32;
};
struct MFM_Ebyedat_TSeventInfo {
	  char eventTime[6];
	};
struct MFM_Ebyedat_ENTSeventInfo {
	  unsigned eventIdx  : 32;
	  char eventTime[6];
	};

// Ebyedat
struct MFM_EbyedatItem {
	  unsigned Label :  16;
	  unsigned Value :  16;
	};

struct MFM_Ebyedat_ENheader{
	 MFM_basic_header        EbyedatBasicHeader;
	 MFM_Ebyedat_ENeventInfo EbyedatEvtInfo;
};
struct MFM_Ebyedat_TSheader{
	 MFM_basic_header        EbyedatBasicHeader;
	 MFM_Ebyedat_TSeventInfo EbyedatEvtInfo;
};
struct MFM_Ebyedat_ENTSheader{
	 MFM_basic_header          EbyedatBasicHeader;
	 MFM_Ebyedat_ENTSeventInfo EbyedatEvtInfo;
};


//____________MFMEbyedatFrame___________________________________________________________

class MFMEbyedatFrame : public MFMBasicFrame
{

private:
int * fLabelIndice; // vector to make statistic
int * fIndiceLabel; // vector to make statistic
int * fNbLabels;// vector to make statistic
uint16_t fNbPara;
public :

MFMEbyedatFrame();
MFMEbyedatFrame(int unitBlock_size, int dataSource,
	 		 int frameType, int revision, int frameSize,int headerSize,
		       int itemSize, int nItems);
virtual ~MFMEbyedatFrame();
virtual void SetPointers(void * pt =NULL);


//virtual void SetHeaderBasic(MFM_basic_header* header) ;
virtual void SetBufferSize(int size, bool ifinferior) ;
virtual void SetAttributs(void * pt =NULL);
virtual uint64_t GetTimeStampAttribut();
virtual uint64_t GetTimeStamp();
virtual int  GetEventNumber();
virtual int  GetEventNumberAttibut();
virtual void SetTimeStamp(uint64_t timestamp);
virtual void SetEventNumber(uint32_t eventnumber);
virtual void EbyedatGetParametersByItem(MFM_EbyedatItem *item,uint16_t * label, uint16_t *value);
virtual void EbyedatSetParametersByItem(MFM_EbyedatItem *item,uint16_t label,uint16_t value);
virtual void EbyedatGetParameters(int i,uint16_t * label, uint16_t *value);
virtual void EbyedatSetParameters(int i,uint16_t  label, uint16_t value);
virtual void FillEventWithRamdomConst(uint64_t timestamp=0,uint32_t enventnumber=0);
virtual void GenerateAEbyedatExample(int type,int32_t eventnumber);
virtual string GetHeaderDisplay(char* infotext) ;
virtual string DumpData(char mode='d', bool nozero=false);
virtual void InitStat() ;
virtual void FillStat();
virtual string  GetStat(string info);
virtual void PrintStat(string info );
};
#pragma pack(pop) // free alignment
#endif
