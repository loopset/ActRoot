#ifndef _MFMBlobFrame_
#define _MFMBlobFrame_
/*
  MFMBlobFrame.cc

	Copyright Acquisition group, GANIL Caen, France

*/
#include "MFMCommonFrame.h"


//____________MFMBlobFrame___________________________________________________________

class MFMBlobFrame : public MFMCommonFrame
{
public:



public :

MFMBlobFrame(int unitBlock_size, int dataSource,
	 		 int frameType, int revision, int frameSize);
MFMBlobFrame();
virtual ~MFMBlobFrame();

};
#endif
