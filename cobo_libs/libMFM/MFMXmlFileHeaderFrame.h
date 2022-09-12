#ifndef _MFMXmlFileHeaderFrame_
#define _MFMXmlFileHeaderFrame_
/*
  MFMXmlFileHeaderFrame.h
	Copyright Acquisition group, GANIL Caen, France
*/

#include "MFMBlobFrame.h"
#include <tinyxml.h>
#pragma pack(push, 1) // force alignment

struct MFM_XmlFile_header{
	 MFM_common_header XmlFile_header;
};

//____________MFMXmlFileHeaderFrame___________________________________________________________

class MFMXmlFileHeaderFrame : public MFMBlobFrame
{

public :

MFMXmlFileHeaderFrame();
MFMXmlFileHeaderFrame(int unitBlock_size, int dataSource,
	 		 int frameType, int revision, int frameSize,int headerSize);
virtual ~MFMXmlFileHeaderFrame();
virtual void SetPointers(void * pt =NULL);

virtual void SetBufferSize(int size, bool ifinferior) ;
virtual void SetAttributs(void * pt =NULL);
virtual uint64_t GetTimeStamp();
virtual uint64_t GetTimeStampAttribut();
virtual int GetEventNumberAttribut();
virtual int GetEventNumber();

virtual void SetTimeStamp(uint64_t timestamp);
virtual void SetEventNumber(int eventnumber);


virtual void      FillExampleOfText();

};
#pragma pack(pop) // free aligment
#endif
