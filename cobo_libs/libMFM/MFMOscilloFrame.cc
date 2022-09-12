/*
 MFMOscilloFrame.cc
 Copyright Acquisition group, GANIL Caen, France
 */

#include <iostream>
#include <cmath>
#include <string.h>
#include <iostream>
#include <stdio.h>
#include <sstream>
#include <stdlib.h>
using namespace std;

#include "MFMOscilloFrame.h"

//_______________________________________________________________________________
MFMOscilloFrame::MFMOscilloFrame(int unitBlock_size, int dataSource,
		int frameType, int revision, int frameSize, int headerSize,
		int itemSize, int nItems) {
	SetPointers();
}
//_______________________________________________________________________________
MFMOscilloFrame::MFMOscilloFrame() {
	//	cout << "debug constructor of MFMOscilloFrame::MFMOscilloFrame()\n";
}
//_______________________________________________________________________________
MFMOscilloFrame::~MFMOscilloFrame() {
	//cout << "debug destructor of MFMOscilloFrame::()\n";

}
//_______________________________________________________________________________
void MFMOscilloFrame::SetBufferSize(int size, bool ifinferior) {
	MFMBasicFrame::SetBufferSize(size, ifinferior);
	MFMOscilloFrame::SetPointers();
}
//_______________________________________________________________________________
void MFMOscilloFrame::SetPointers(void * pt) {
	MFMBasicFrame::SetPointers(pt);
	pHeader = (MFM_topcommon_header*) pData;
	pData_char = (char*) pData;
}
//_______________________________________________________________________________
void MFMOscilloFrame::SetAttributs(void * pt) {

	SetPointers(pt);
	MFMBasicFrame::SetAttributs(pt);
}
//_______________________________________________________________________________
string MFMOscilloFrame::GetHeaderDisplay(char* infotext) {
/// Return a string containing infomation of MFM Header\n
	/// if infotext is not NULL replace the standart "MFM header" title
	stringstream ss;
	string display("");
	ss << MFMBasicFrame::GetHeaderDisplay(infotext);
	ss << "   Config  : Channel Number = " << GetChannelIdxNumber();
	ss << "  Signal = " << GetConfigSignal();
	ss << "  Trig = " << GetConfigTrig();
	ss << "  Basetime = " << GetConfigTimeBase();
	ss << "  On/Off = " << GetConfigOnOff();
	ss << "\n";
	ss << "   Channel : Channel Number= " << GetConfigChannelIdx();
	ss << "  Board Number = " <<GetChannelIdxBoard();
	ss << "\n";
	display = ss.str();
	return display;
}
//_______________________________________________________________________________
void MFMOscilloFrame::OscilloGetParameters(int i,uint16_t *value) {
	OscilloGetParametersByItem((MFM_OscilloItem *) GetItem(i), value);
}
//_______________________________________________________________________________
void MFMOscilloFrame::OscilloGetParametersByItem(MFM_OscilloItem *item,uint16_t *value) {
	uint16_t tmp=item->Value;
	if (fLocalIsBigEndian!=fFrameIsBigEndian)
		SwapInt16(&tmp);
		*value = tmp;
}
//_______________________________________________________________________________
void MFMOscilloFrame::SetConfig(uint16_t config){
  /// Set all Config
	((MFM_OscilloHeader*) pHeader)->OscilloEvtInfo.Config = config;
}
//_______________________________________________________________________________
uint16_t MFMOscilloFrame::GetConfig(){
	/// Set all Config (on/off,time base, trig, signal, channel idx)
	return  (((MFM_OscilloHeader*) pHeader)->OscilloEvtInfo.Config);
}
//_______________________________________________________________________________
void MFMOscilloFrame::SetConfigOnOff(uint16_t onoff){
  /// Set  Config ON or OFF ( 1 or 0) in config
	onoff=	onoff & MFM_CHANNELID_NUMBER_MSK;
((MFM_OscilloHeader*) pHeader)->OscilloEvtInfo.Config =
		(((MFM_OscilloHeader*) pHeader)->OscilloEvtInfo.Config & (~MFM_CONFIG_ONOFF_MSK))
		| (onoff<<15);
}
//_______________________________________________________________________________
uint16_t MFMOscilloFrame::GetConfigOnOff(){
	/// Get ON / OFF  bit from config
	return ((((MFM_OscilloHeader*) pHeader)->OscilloEvtInfo.Config & MFM_CONFIG_ONOFF_MSK)>>15);
}
//_______________________________________________________________________________
void MFMOscilloFrame::SetConfigTimeBase(uint16_t time){
	/// Set Time base in config
	time = (time <<11)& MFM_CONFIG_TIMEBASE_MSK;
	((MFM_OscilloHeader*) pHeader)->OscilloEvtInfo.Config =
			(((MFM_OscilloHeader*) pHeader)->OscilloEvtInfo.Config & (~MFM_CONFIG_TIMEBASE_MSK))
			| time;
}
//_______________________________________________________________________________
void MFMOscilloFrame::SetConfigTrig(uint16_t trig){
	/// Set Tring in config
	((MFM_OscilloHeader*) pHeader)->OscilloEvtInfo.Config =
			(((MFM_OscilloHeader*) pHeader)->OscilloEvtInfo.Config & (~MFM_CONFIG_TRIG_MSK))
			| ((trig<<8) & MFM_CONFIG_TRIG_MSK);
}
//_______________________________________________________________________________
void MFMOscilloFrame::SetConfigSignal(uint16_t signal){
	/// set Signal in config
	((MFM_OscilloHeader*) pHeader)->OscilloEvtInfo.Config =
			(((MFM_OscilloHeader*) pHeader)->OscilloEvtInfo.Config & (~MFM_CONFIG_SIGNAL_MSK))
			| ((signal<<5) & MFM_CONFIG_SIGNAL_MSK);
}
//_______________________________________________________________________________
void MFMOscilloFrame::SetConfigChannelIdx(uint16_t idx){
	/// return Channel idx from config
	idx = idx & MFM_CHANNELID_NUMBER_MSK;
	((MFM_OscilloHeader*) pHeader)->OscilloEvtInfo.Config =
			(((MFM_OscilloHeader*) pHeader)->OscilloEvtInfo.Config & (~MFM_CHANNELID_NUMBER_MSK))
			| idx ;
}
//_______________________________________________________________________________
uint16_t MFMOscilloFrame::GetConfigTimeBase(){
	/// return Time Base formconfig

	return ((((MFM_OscilloHeader*) pHeader)->OscilloEvtInfo.Config & MFM_CONFIG_TIMEBASE_MSK)>>11);
}
//_______________________________________________________________________________
uint16_t MFMOscilloFrame::GetConfigTrig(){
	/// return trigger from config
	return ((((MFM_OscilloHeader*) pHeader)->OscilloEvtInfo.Config & MFM_CONFIG_TRIG_MSK)>>8);
}
//_______________________________________________________________________________
uint16_t MFMOscilloFrame::GetConfigSignal(){
	/// return Signal from config
	return ((((MFM_OscilloHeader*) pHeader)->OscilloEvtInfo.Config & MFM_CONFIG_SIGNAL_MSK)>>5);
}
//_______________________________________________________________________________
uint16_t MFMOscilloFrame::GetConfigChannelIdx(){
	/// return ChannelIdx from config
	return ((((MFM_OscilloHeader*) pHeader)->OscilloEvtInfo.Config & MFM_CONFIG_IDXCHAN_MSK));
}
//_______________________________________________________________________________
uint16_t MFMOscilloFrame::GetChannelIdxNumber(){
    /// return channel number(on/off,time base, trig, signal, channel idx)
	return (MFM_CHANNELID_NUMBER_MSK & ((MFM_OscilloHeader*) pHeader)->OscilloEvtInfo.ChannelIdx);
}
//_______________________________________________________________________________
uint16_t MFMOscilloFrame::GetChannelIdxBoard(){
    /// return channel board index
	return ((MFM_CHANNELID_BOARD_MSK& ((MFM_OscilloHeader*) pHeader)->OscilloEvtInfo.ChannelIdx)>>4);
}
//_______________________________________________________________________________
uint16_t MFMOscilloFrame::GetChannelIdx(){
    /// return all channel index  (board index + channel number)
	return (((MFM_OscilloHeader*) pHeader)->OscilloEvtInfo.ChannelIdx);
}
//_______________________________________________________________________________
void MFMOscilloFrame::SetChannelIdxNumber(uint16_t idx){
    /// return channel number
	((MFM_OscilloHeader*) pHeader)->OscilloEvtInfo.ChannelIdx=
			(((MFM_OscilloHeader*) pHeader)->OscilloEvtInfo.ChannelIdx & (~MFM_CHANNELID_NUMBER_MSK))
			| (idx & MFM_CHANNELID_NUMBER_MSK);
}
//_______________________________________________________________________________
void MFMOscilloFrame::SetChannelIdxBoard(uint16_t idx){
    /// return channel number
	cout <<" idx "<<idx<<" channelidx "<<((MFM_OscilloHeader*) pHeader)->OscilloEvtInfo.ChannelIdx<<"\n";
	((MFM_OscilloHeader*) pHeader)->OscilloEvtInfo.ChannelIdx=
		(((MFM_OscilloHeader*) pHeader)->OscilloEvtInfo.ChannelIdx & (~MFM_CHANNELID_BOARD_MSK))
			| ((idx & MFM_CHANNELID_BOARD_MSK)<<4);
}
//_______________________________________________________________________________
void MFMOscilloFrame::SetChannelIdx(uint16_t idx){
	  /// Set channel number ( board index + channel number)
	((MFM_OscilloHeader*) pHeader)->OscilloEvtInfo.ChannelIdx = idx;
}
//_______________________________________________________________________________
void MFMOscilloFrame::OscilloSetParameters(int i,uint16_t value) {
	OscilloSetParametersByItem((MFM_OscilloItem *) GetItem(i), value);
}
//_______________________________________________________________________________
void MFMOscilloFrame::OscilloSetParametersByItem(MFM_OscilloItem *item, uint16_t value) {
	item->Value = value;
}

//_______________________________________________________________________________
void MFMOscilloFrame::FillEventWithRamp(uint16_t channelIdx, uint16_t Config) {
	SetChannelIdxBoard(channelIdx);
	SetChannelIdxNumber(channelIdx);

	SetConfigChannelIdx(channelIdx);
	SetConfigOnOff(channelIdx%2);
	SetConfigTimeBase(15);
	SetConfigTrig(1);
	SetConfigSignal(2);

	uint16_t j = 0;
	int i = 0;
	if  (GetNbItems()>0)
	for (i = 0; i < GetNbItems(); i++) {
		OscilloSetParameters(i,j++);
	}
}
//____________________________________________________________________________
