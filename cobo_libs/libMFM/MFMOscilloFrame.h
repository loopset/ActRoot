#ifndef _MFMOscilloFrame_
#define _MFMOscilloFrame_
/*
  MFMOscilloFrame.h

	Copyright Acquisition group, GANIL Caen, France

*/

#include "MFMBasicFrame.h"

#pragma pack(push, 1) // force alignment

// OSCILLO
struct MFM_OscilloItem {
	  unsigned Value :  16;
	};

struct MFM_Oscillo_EventInfo {
	  unsigned ChannelIdx :  16;
	  unsigned Config     :  16;
	};

struct MFM_OscilloHeader{
	 MFM_basic_header       OscilloBasicHeader;
	 MFM_Oscillo_EventInfo OscilloEvtInfo;
};

#define MFM_CHANNELID_BOARD_MSK 0xFFF0
#define MFM_CHANNELID_NUMBER_MSK 0x000F

#define MFM_CONFIG_ONOFF_MSK    0x8000
#define MFM_CONFIG_TIMEBASE_MSK 0x7800
#define MFM_CONFIG_TRIG_MSK     0x0700
#define MFM_CONFIG_SIGNAL_MSK   0x00E0
#define MFM_CONFIG_IDXCHAN_MSK  0x001F
#define MFM_OSCILLO_HEADERSIZE  20


//____________MFMOscilloFrame___________________________________________________________

class MFMOscilloFrame : public MFMBasicFrame
{

public :

MFMOscilloFrame();
MFMOscilloFrame(int unitBlock_size, int dataSource,
	 		 int frameType, int revision, int frameSize,int headerSize,
		       int itemSize, int nItems);
virtual ~MFMOscilloFrame();
virtual void SetPointers(void * pt =NULL);

//virtual void SetHeaderBasic(MFM_basic_header* header) ;
virtual void SetBufferSize(int size, bool ifinferior) ;
virtual void SetAttributs(void * pt =NULL);
virtual string GetHeaderDisplay(char* infotext=NULL);
virtual void SetConfig(uint16_t value);
virtual void SetConfigOnOff(uint16_t onoff);
virtual void SetConfigTimeBase(uint16_t time);
virtual void SetConfigTrig(uint16_t trig);
virtual void SetConfigSignal(uint16_t signal);
virtual void SetConfigChannelIdx(uint16_t idx);
virtual void SetChannelIdx(uint16_t idx);
virtual void SetChannelIdxNumber(uint16_t idx);
virtual void SetChannelIdxBoard(uint16_t idx);

virtual uint16_t GetConfig();
virtual uint16_t GetConfigOnOff();
virtual uint16_t GetConfigTimeBase();
virtual uint16_t GetConfigTrig();
virtual uint16_t GetConfigSignal();
virtual uint16_t GetConfigChannelIdx();
virtual uint16_t GetChannelIdx();
virtual uint16_t GetChannelIdxNumber();
virtual uint16_t GetChannelIdxBoard();



virtual void OscilloGetParametersByItem(MFM_OscilloItem *item,uint16_t *value);
virtual void OscilloSetParametersByItem(MFM_OscilloItem *item,uint16_t  value);
virtual void OscilloGetParameters(int i, uint16_t *value);
virtual void OscilloSetParameters(int i, uint16_t value);

virtual void FillEventWithRamp(uint16_t channelIdx, uint16_t Config);

};
#pragma pack(pop) // free alignment
#endif
