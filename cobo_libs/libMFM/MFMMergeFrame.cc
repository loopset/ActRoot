/*
 MFMMergeFrame.cc
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

#include "MFMMergeFrame.h"

//_______________________________________________________________________________
MFMMergeFrame::MFMMergeFrame(int unitBlock_size, int dataSource, int frameType,
		int revision, int frameSize, int headerSize, int itemSize, int nItems) {
/// Constructor of Merge frame . The Header is filled with unitBlock_size ... nItems data.

	SetPointers();

}

//_______________________________________________________________________________
MFMMergeFrame::MFMMergeFrame() {
	/// Constructor of a empty frame
	pCurrentPointerForAdd  = NULL;
	pCurrentPointerForRead = NULL;
	//	cout << "debug constructor of MFMMergeFrame::MFMMergeFrame()\n";
}
//_______________________________________________________________________________
MFMMergeFrame::~MFMMergeFrame() {
	///destructor of MFMMergeFrame
}
//_______________________________________________________________________________
void MFMMergeFrame::SetBufferSize(int size, bool ifinferior) {
 /// Do memory allocation or a reallacation for frame\n
 /// if ifinferior==true the allocaton is forced to size event if the acutal size is bigger\n
	MFMBasicFrame::SetBufferSize(size, ifinferior);
	MFMMergeFrame::SetPointers();
}
//_______________________________________________________________________________
void MFMMergeFrame::SetPointers(void * pt) {
/// Initialize pointers of frame\n
	/// if pt==NULL initialization is with current value of main pointer of frame (pData)\n
	/// else initialization is done with pData = pt\n
	/// pData must be the reference;
	MFMBasicFrame::SetPointers(pt);
	pHeader = (MFM_topcommon_header*) pData;
	pData_char = (char*) pData;

}
//_______________________________________________________________________________
void MFMMergeFrame::SetAttributs(void * pt) {
	/// Initialize a set of attributs (frame size, endianess, type ...) and pointers of frame\n
	/// reading and computing data comming from header of frame\n
	/// if pt==NULL initialization is done with current value of main pointer of frame (pData)\n
	/// else initialization is done with pData = pt
	SetPointers(pt);
	MFMBasicFrame::SetAttributs(pt);
	ResetAdd();
	ResetReadInMem();
}
//_______________________________________________________________________________
void MFMMergeFrame::ResetReadInMem(){
	/// reset read pointer to the begin of first inside frame
pCurrentPointerForRead  = pData_char+fHeaderSize;
}
//_______________________________________________________________________________
void MFMMergeFrame::ResetAdd(){
	/// reset add pointer  to the begin of memorize of Merge frame dedicated to inside frame
pCurrentPointerForAdd  = pData_char+fHeaderSize;
}
//_______________________________________________________________________________

uint64_t MFMMergeFrame::GetTimeStamp() {
	/// Compute time stamp and fill fTimeStamp attribut. return value of TimeStamp
	fTimeStamp = 0;
	uint64_t * timeStamp = &(fTimeStamp);

	if (GetFrameTypeAttribut() == MFM_MERGE_TS_FRAME_TYPE) {
		memcpy(((char*) (&fTimeStamp)),
				((MFM_Merge_TSheader*) pHeader)->MergeEvtInfo.eventTime, 6);
		if (fLocalIsBigEndian != fFrameIsBigEndian)
			SwapInt64((timeStamp), 6);
	}
	if (GetFrameTypeAttribut() == MFM_MERGE_EN_FRAME_TYPE) {
		//nothing to do and we leave fTimeStamp to 0
		fTimeStamp = 0;
	}
	return fTimeStamp;
}
//_______________________________________________________________________________

uint32_t MFMMergeFrame::GetDeltaTime() {
	/// Compute time stamp and fill fTimeStamp attribut. return value of TimeStamp
	fDeltaTime = 0;
	uint32_t * deltatime = &(fDeltaTime);

	if (GetFrameTypeAttribut() == MFM_MERGE_TS_FRAME_TYPE) {
		fDeltaTime = ((MFM_Merge_TSheader*) pHeader)->MergeEvtInfo.deltaTime;

		if (fLocalIsBigEndian != fFrameIsBigEndian)
			SwapInt32((deltatime), 4);
	}
	if (GetFrameTypeAttribut() == MFM_MERGE_EN_FRAME_TYPE) {
		//nothing to do and we leave fTimeStamp to 0
		fDeltaTime = 0;
	}
	return fDeltaTime;
}
//_______________________________________________________________________________
uint64_t MFMMergeFrame::GetTimeStampAttribut() {
	/// return time stamp attribut without any computing
	return fTimeStamp;
}
//_______________________________________________________________________________
uint32_t MFMMergeFrame::GetDeltaTimeAttribut() {
	/// return Deta time attribut without any computing
	return fDeltaTime;
}
//_______________________________________________________________________________

int MFMMergeFrame::GetEventNumber() {

	fEventNumber = 0;
	char * eventNumber = (char*) &(fEventNumber);

	if (GetFrameTypeAttribut() == MFM_MERGE_TS_FRAME_TYPE) {
		//nothing to do and we leave fEventNumber to 0
		fEventNumber = 0;
	}

	if (GetFrameTypeAttribut() == MFM_MERGE_EN_FRAME_TYPE) {
		fEventNumber = ((MFM_Merge_ENheader*) pHeader)->MergeEvtInfo.eventIdx;
		if (fLocalIsBigEndian != fFrameIsBigEndian)
			SwapInt32((uint32_t *) (eventNumber), 4);
	}
	return fEventNumber;
}

//_______________________________________________________________________________
int MFMMergeFrame::GetEventNumberAttibut() {
	/// Return Event number attibut without any computing
	return fEventNumber;
}

//_______________________________________________________________________________
void MFMMergeFrame::SetTimeStamp(uint64_t timestamp) {
	/// Set Time stamp data in frame
	char* pts = (char*) &timestamp;
	timestamp = timestamp & 0x0000ffffffffffff;

	if (GetFrameTypeAttribut() == MFM_MERGE_TS_FRAME_TYPE)
		memcpy(((MFM_Merge_TSheader*) pHeader)->MergeEvtInfo.eventTime, pts, 6);
	if (GetFrameTypeAttribut() == MFM_MERGE_EN_FRAME_TYPE) {//do nothing
	}
}
//_______________________________________________________________________________
void MFMMergeFrame::SetDeltaTime(uint32_t deltatime) {
	/// Set Delta Time data in frame
	char* pts = (char*) &deltatime;
	deltatime = deltatime & 0x0000ffffffffffff;

	if (GetFrameTypeAttribut() == MFM_MERGE_TS_FRAME_TYPE)
		memcpy(((MFM_Merge_TSheader*) pHeader)->MergeEvtInfo.eventTime, pts, 6);
	if (GetFrameTypeAttribut() == MFM_MERGE_EN_FRAME_TYPE) {//do nothing
	}
}

//_______________________________________________________________________________
void MFMMergeFrame::SetEventNumber(int eventnumber) {
/// Set Event Number data in frame
	if (GetFrameTypeAttribut() == MFM_MERGE_TS_FRAME_TYPE) {//do nothing

	}
	if (GetFrameTypeAttribut() == MFM_MERGE_EN_FRAME_TYPE)
		((MFM_Merge_ENheader*) pHeader)->MergeEvtInfo.eventIdx = eventnumber;
}
//_______________________________________________________________________________
void MFMMergeFrame::AddFrame(MFMCommonFrame* frame) {
/// add a "frame" in free memory of Merge Frame
	int size = frame->GetFrameSizeAttribut();
	uint32_t availablesize = fFrameSize -((pCurrentPointerForAdd -pData_char)+ size);
	if (availablesize < (uint32_t)fFrameSize  ){
		memcpy(pCurrentPointerForAdd, frame->GetPointHeader(), size);
		pCurrentPointerForAdd += size;
	} else {
		cout << " Error in MFMMergeFrame::AddFrame , no more place ";
		cout << " pCurrentPointerForAdd = "<< (int*)pCurrentPointerForAdd << " pData_char = "<<(int*)pData_char<<"availablesize "<<availablesize<<"\n";
	}
}
//_______________________________________________________________________________
void MFMMergeFrame::ReadInFrame(MFMCommonFrame* frame) {
	// Read inside frame in Merge frame and return data in "frame"
	// if frame is not enough big , frame is automaticaly resized
	int readsize = frame->ReadInMem(&pCurrentPointerForRead);
	if (readsize <= 0)
		cout << "Error in MFMMergeFrame::ReadFrame , no more place ";
}
//____________________________________________________________________________
string MFMMergeFrame::GetHeaderDisplay(char* infotext) {
	stringstream ss;
	string display("");
	display = ss.str();
	ss << MFMBasicFrame::GetHeaderDisplay(infotext);
	if (GetFrameTypeAttribut() == MFM_MERGE_TS_FRAME_TYPE) {//do nothing
		ss << "  TS = " << GetTimeStamp();
	}
	if (GetFrameTypeAttribut() == MFM_MERGE_EN_FRAME_TYPE){
		ss << "  EN = " << GetEventNumber();
	}
	display = ss.str();
	return display;

}
//____________________________________________________________________________
