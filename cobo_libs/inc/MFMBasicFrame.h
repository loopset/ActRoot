#ifndef _MFMBasicFrame_
#define _MFMBasicFrame_
/*
  MFMBasicFrame.cc

	Copyright Acquisition group, GANIL Caen, France

*/
#include "MFMCommonFrame.h"
#pragma pack(push, 1) // force alignment

//____________MFMBasicFrame___________________________________________________________

class MFMBasicFrame : public MFMCommonFrame
{
public:
MFM_basic_header * pBasicHeader;//< pointer on header

uint64_t           fTimeStamp;//<Time Stamp
uint32_t           fEventNumber; //<Event Number
uint32_t           fItemSize; //<Item Size
uint32_t           fNumberItems;//Number of items
void              * pCurrentItem; // current pointer on item
public :

MFMBasicFrame();
MFMBasicFrame(int unitBlock_size, int dataSource,
	 		 int frameType, int revision, int frameSize,int headerSize,
		       int itemSize, int nItems);
virtual ~MFMBasicFrame();
virtual void SetPointers(void * pt =NULL);
virtual int  GetHeaderSize();
virtual int  GetHeaderSizeAttribut();
virtual int  GetItemSize();
virtual int  GetItemSizeAttribut();
virtual int  GetNbItems();
virtual int  GetNbItemsAttribut();

virtual void SetHeaderBasic(MFM_basic_header* header) ;
virtual void SetHeaderSize(int headersize);
virtual void SetItemSize(int itemsize);
virtual void SetNbItem(int nbitem);
virtual void SetBufferSize(int size,bool ifinferior=true);
virtual void SetAttributs(void * pt=NULL);
virtual uint64_t GetTimeStamp();
virtual uint64_t GetTimeStampAttibut();
virtual int GetEventNumber();
virtual int  GetEventNumberAttribut();
virtual void *GetItem(int i);
virtual void *GetCurrentItem(){return pCurrentItem;};
virtual string GetHeaderDisplay(char* infotext=NULL);
virtual void MFM_make_header(int unitBlock_size, int dataSource,
			    int frameType, int revision, int frameSize,int headerSize,
			       int itemSize, int nItems);

virtual void MFM_fill_header(int unitBlock_size,
				   int dataSource, int frameType, int revision,
				   int frameSize,int headerSize,
			       int itemSize, int nItems);
};
#pragma pack(pop) // free aligment
#endif
