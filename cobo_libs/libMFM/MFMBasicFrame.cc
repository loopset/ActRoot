/**
 MFMBasicFrame.cc
 Copyright Acquisition group, GANIL Caen, France
 **/

#include <iostream>
#include <cmath>
#include <string.h>
#include <iostream>
#include <stdio.h>
#include <sstream>
using namespace std;

#include "MFMBasicFrame.h"

//_______________________________________________________________________________
MFMBasicFrame::MFMBasicFrame(int unitBlock_size, int dataSource, int frameType,
		int revision, int frameSize, int headerSize, int itemSize, int nItems) {
/// Constructor of frame with a memory space\n
/// fill header information : unitBlock_size,dataSource,....

	fTimeStamp=0;
    fEventNumber=0;
    fItemSize=0;
    pCurrentItem =NULL;
	MFM_make_header(unitBlock_size, dataSource, frameType, revision, frameSize,
			headerSize, itemSize, nItems);
	SetPointers();
}

//_______________________________________________________________________________
MFMBasicFrame::MFMBasicFrame() {
	/// Constructor of a empty frame object
	fTimeStamp=0;
	fEventNumber=0;
	fItemSize=0;
	pCurrentItem =NULL;
//	cout << "debug constructor of MFMBasicFrame::MFMBasicFrame()\n";
}
//_______________________________________________________________________________
MFMBasicFrame::~MFMBasicFrame() {

}
//_______________________________________________________________________________
int MFMBasicFrame::GetItemSize() {
	// compute and set fItemSize attibut. Return fItemSize
	uint16_t tmp;
	tmp = ((MFM_basic_header*)pHeader)->ext.itemSize;
	if (fLocalIsBigEndian!=fFrameIsBigEndian)
	SwapInt16(&tmp);
	fItemSize =tmp;
	return fItemSize;
}
//_______________________________________________________________________________
int MFMBasicFrame::GetItemSizeAttribut() {
	/// Return value of Item size without computing it
	return fItemSize;

}
//_______________________________________________________________________________
void MFMBasicFrame::SetHeaderBasic(MFM_basic_header* header) {
	/// Set pointer pHeader
	pHeader = (MFM_topcommon_header*)header;
}
//_______________________________________________________________________________
int MFMBasicFrame::GetNbItems() {
	/// Compute and return number of items
	unsigned int tmp;
	tmp = (int)(((MFM_basic_header*)pHeader)->ext.nItems);
	if (fLocalIsBigEndian!=fFrameIsBigEndian)
	SwapInt32(&tmp);
	fNumberItems=tmp;
	return fNumberItems;
}
//_______________________________________________________________________________
int MFMBasicFrame::GetNbItemsAttribut(){
	/// Return value of fNumberItems without computing it
	return fNumberItems;
}
//_______________________________________________________________________________
void MFMBasicFrame::SetHeaderSize(int headersize) {
	/// Set frame header size
	((MFM_basic_header*)pHeader)->ext.headerSize = (uint16_t) headersize;
	fHeaderSize = headersize;

}
//_______________________________________________________________________________
void MFMBasicFrame::SetItemSize(int itemsize) {
	/// Set frame item size
	fItemSize = itemsize;
	((MFM_basic_header*)pHeader)->ext.itemSize = itemsize;
}
//_______________________________________________________________________________
void MFMBasicFrame::SetNbItem(int nbitems) {
	/// Set frame items number
	fNumberItems = nbitems;
	((MFM_basic_header*)pHeader)->ext.nItems = nbitems;
}

//_______________________________________________________________________________
string MFMBasicFrame::GetHeaderDisplay(char* infotext) {
/// Return a string containing infomation of MFM Header\n
	/// if infotext is not NULL replace the standart "MFM header" title
	stringstream ss;
	string display("");
	ss << MFMCommonFrame::GetHeaderDisplay(infotext);
	ss << "   HeaderSize = " << GetHeaderSize();
	ss << "  ItemSize = " << GetItemSize();
	ss << "  NbItems = " << GetNbItems();
	ss << "\n";

	display = ss.str();
	return display;
}

//_______________________________________________________________________________

void MFMBasicFrame::MFM_fill_header(int unitBlock_size, int dataSource,
		int frameType, int revision, int frameSize, int headerSize,
		int itemSize, int nItems) {
	/// Fill a Header with a list of parameters:\n
	/// unitBlock_size, dataSource, frameType, revision, frameSize,headerSize,itemSize,nItems
	MFMCommonFrame::MFM_fill_header(unitBlock_size, dataSource, frameType,
			revision, frameSize);
	SetHeaderSize(headerSize);
	fHeaderSize = headerSize*unitBlock_size;
	SetItemSize(itemSize);
	SetNbItem(nItems);
}

//_______________________________________________________________________________
void MFMBasicFrame::MFM_make_header(int unitBlock_size, int dataSource,
		int frameType, int revision, int frameSize, int headerSize,
		int itemSize, int nItems) {
	/// Do memory allocation for frame and\n
	/// fill its Header with a list of parameters:\n
    SetBufferSize(frameSize*unitBlock_size);
    if (!pHeader)cout << " Error of header null\n";;
	MFM_fill_header(unitBlock_size, dataSource, frameType, revision, frameSize,
			headerSize, itemSize, nItems);
	SetPointers();

}
//_______________________________________________________________________________
void MFMBasicFrame::SetBufferSize(int size, bool ifinferior) {
	/// Do memory allocation or a reallacation for frame\n
	/// if ifinferior==true the allocaton is forced to size event if the acutal size is bigger\n
	MFMCommonFrame::SetBufferSize(size, ifinferior);
	MFMBasicFrame::SetPointers();
}
//_______________________________________________________________________________
void MFMBasicFrame::SetPointers(void * pt){
	/// Initialize pointers of frame\n
	/// if pt==NULL initialization is with current value of main pointer of frame (pData)\n
	/// else initialization is done with pData = pt\n
	/// pData must be the reference;
	MFMCommonFrame::SetPointers(pt);
	(pHeader) = (MFM_topcommon_header*)pData;
	pData_char = (char*) pData;
}
//_______________________________________________________________________________
void * MFMBasicFrame::GetItem(int i) {
    /// return pointer on i item
	pCurrentItem = (MFMBasicFrame *)(pData_char + (fHeaderSize) + (fItemSize*i));
	return  pCurrentItem;
}
//_______________________________________________________________________________
void MFMBasicFrame::SetAttributs(void * pt){
	/// Initialize a set of attributs (frame size, endianess, type ...) and pointers of frame\n
	/// reading and computing data comming from header of frame\n
	/// if pt==NULL initialization is done with current value of main pointer of frame (pData)\n
	/// else initialization is done with pData = pt
	SetPointers( pt);
	MFMCommonFrame::SetAttributs( pt);
	GetNbItems();
	GetHeaderSize();
	GetItemSize();
	pUserData_char=pData_char+fHeaderSize;
}
//_______________________________________________________________________________
int  MFMBasicFrame::GetHeaderSize(){
	/// Compute and set value fHeaderSize, return fHeaderSize
	uint16_t tmp=
			((MFM_basic_header*)pHeader)->ext.headerSize;
	if (fLocalIsBigEndian!=fFrameIsBigEndian)
	SwapInt16(&tmp);
	fHeaderSize = (int)tmp*fSizeOfUnitBlock;
	return fHeaderSize;
}
//_______________________________________________________________________________
int MFMBasicFrame::GetHeaderSizeAttribut(){
	///Return value of HeaderSize without computing
	return fHeaderSize;
}
//_______________________________________________________________________________
int MFMBasicFrame::GetEventNumberAttribut() {
	///Return value of EventNumber without computing
	return fEventNumber;
}
//_______________________________________________________________________________
int MFMBasicFrame::GetEventNumber() {
	/// Return value of EventNumber without computing
	return fEventNumber;
}
//_______________________________________________________________________________
uint64_t MFMBasicFrame::GetTimeStampAttibut(){
	/// Return value of TimeStamp Attibut without computing
	return fTimeStamp;
}
//_______________________________________________________________________________
uint64_t MFMBasicFrame::GetTimeStamp(){
	/// Return value of TimeStamp Attibut without computing
	return fTimeStamp;
}
//_______________________________________________________________________________
