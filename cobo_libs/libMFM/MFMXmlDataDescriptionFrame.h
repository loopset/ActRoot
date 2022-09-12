#ifndef _MFMXmlDataDescriptionFrame_
#define _MFMXmlDataDescriptionFrame_
/*
  MFMXmlDataDescriptionFrame.h
	Copyright Acquisition group, GANIL Caen, France
*/

#include "MFMBlobFrame.h"
#include <tinyxml.h>

#pragma pack(push, 1) // force alignment
struct MFM_xml_Info {
  unsigned EventIdx  : 32;
};
struct MFM_xml_Data {
};

struct MFM_XmlDataDescription_header{
	 MFM_common_header  XmlBlobcHeader;
	 MFM_xml_Info  XmlEventInfo;
	 MFM_xml_Data  XmlData;
};



//____________MFMXmlDataDescriptionFrame___________________________________________________________

class MFMXmlDataDescriptionFrame : public MFMBlobFrame
{
public:
uint64_t fTimeStamp;
int fEventNumber ;

unsigned int NUM_INDENTS_PER_SPACE;

TiXmlDocument * MyTinyDoc;

public :

MFMXmlDataDescriptionFrame();
MFMXmlDataDescriptionFrame(int unitBlock_size, int dataSource,
	 		 int frameType, int revision, int frameSize,int headerSize);
virtual ~MFMXmlDataDescriptionFrame();
virtual void SetPointers(void * pt =NULL);

virtual void SetBufferSize(int size, bool ifinferior) ;
virtual void SetAttributs(void * pt =NULL);
virtual uint64_t GetTimeStamp();
virtual uint64_t GetTimeStampAttribut();
virtual int GetEventNumberAttribut();
virtual int GetEventNumber();

virtual void SetTimeStamp(uint64_t timestamp);
virtual void SetEventNumber(int eventnumber);


virtual string FillExampleOfText();
virtual int    InitXml(bool write_or_read, string commentaire);

void dump_to_stdout(const char* pFilename);
void dump_to_stdout( TiXmlNode* pParent, unsigned int indent = 0 );
int dump_attribs_to_stdout(TiXmlElement* pElement, unsigned int indent);
const char * getIndentAlt( unsigned int numIndents );
const char * getIndent( unsigned int numIndents );
};
#pragma pack(pop) // free aligment
#endif
