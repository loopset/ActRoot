/*
 MFMMutantFrame.cc
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

#include "MFMMutantFrame.h"

//_______________________________________________________________________________
MFMMutantFrame::MFMMutantFrame(int unitBlock_size, int dataSource,
		int frameType, int revision, int frameSize, int headerSize) {
	/// Constructor for a Mutant frame . the header is filled with unitblock_size, data source , frame type , revision , frame, size and header size value
	SetPointers();
}

//_______________________________________________________________________________
MFMMutantFrame::MFMMutantFrame() {
	/// Constructor for a empty Mutant frame

}
//_______________________________________________________________________________
MFMMutantFrame::~MFMMutantFrame() {
	/// destructor of Mutant frame
}
//_______________________________________________________________________________
void MFMMutantFrame::SetBufferSize(int size, bool ifinferior) {
	MFMBlobFrame::SetBufferSize(size, ifinferior);
	MFMMutantFrame::SetPointers();
}
//_______________________________________________________________________________
void MFMMutantFrame::SetPointers(void * pt) {
	MFMBlobFrame::SetPointers(pt);
	pHeader = (MFM_topcommon_header*) pData;
	pData_char = (char*) pData;
}
//_______________________________________________________________________________
void MFMMutantFrame::SetAttributs(void * pt) {
	SetPointers(pt);
	MFMBlobFrame::SetAttributs(pt);
	pUserData_char = (char*) &(((MFM_mut_header*) pHeader)->MutData);
}
//_______________________________________________________________________________

uint64_t MFMMutantFrame::GetTimeStamp() {
	/// Computer, set attibut and return value of time stamp from  frame
	fTimeStamp = 0;
	uint64_t * timeStamp = &(fTimeStamp);
	memcpy(((char*) (&fTimeStamp)),
			((MFM_mut_header*) pHeader)->MutEventInfo.EventTime, 6);
	if (fLocalIsBigEndian != fFrameIsBigEndian)
		SwapInt64((timeStamp), 6);
	return fTimeStamp;
}
//_______________________________________________________________________________
uint64_t MFMMutantFrame::GetTimeStampAttribut() {
	/// Return attibut of time stamp
	return fTimeStamp;
}

//_______________________________________________________________________________

int MFMMutantFrame::GetEventNumber() {
	/// Computer, set attibut and return value of event number from  frame
	fEventNumber = 0;
	char * eventNumber = (char*) &(fEventNumber);
	//memcpy(&fEventNumber,((char*)((MFM_mut_header*)pHeader)->mutEvtInfo.eventIdx),4);
	fEventNumber = ((MFM_mut_header*) pHeader)->MutEventInfo.EventIdx;
	if (fLocalIsBigEndian != fFrameIsBigEndian)
		SwapInt32((uint32_t *) (eventNumber), 4);
	return fEventNumber;
}

//_______________________________________________________________________________
int MFMMutantFrame::GetEventNumberAttribut() {
	/// Return attibut of event number
	return fEventNumber;
}
//_______________________________________________________________________________
void MFMMutantFrame::SetTimeStamp(uint64_t timestamp) {
	/// Set value of Time Stamp in frame

	char* pts = (char*) &timestamp;
	timestamp = timestamp & 0x0000ffffffffffff;
	memcpy(((MFM_mut_header*) pHeader)->MutEventInfo.EventTime, pts, 6);
}
//_______________________________________________________________________________
void MFMMutantFrame::SetEventNumber(int eventnumber) {
	/// Set Event Number of frame
	((MFM_mut_header*) pHeader)->MutEventInfo.EventIdx = eventnumber;
}

//_______________________________________________________________________________
void MFMMutantFrame::SetTriggerInfo(uint16_t trig) {
	(((MFM_mut_header*) pHeader)->MutData.TriggerInfo) = trig;
}
//_______________________________________________________________________________
uint16_t MFMMutantFrame::GetTriggerInfo() {
	uint16_t trig;
	trig = (((MFM_mut_header*) pHeader)->MutData.TriggerInfo);
	if (fLocalIsBigEndian != fFrameIsBigEndian)
		SwapInt16(&trig);
	return trig;
}
//______________________________________________________________________________
void MFMMutantFrame::SetMultiplicity(int i, uint16_t mult) {

	if (i < 0 and i > MUT_NB_MULTIPLICITY) {
		cout << "MFMMutantFrame::SetMultiplicity Error of index\n";
	} else {
		cout << " debug FillEventRandomConstSetMultiplicity "<<mult<<"\n";
		(((MFM_mut_header*) pHeader)->MutData.Mutiplicity[i]) = mult;
	}
}
//_______________________________________________________________________________
uint16_t MFMMutantFrame::GetMultiplicity(int i) {

	uint16_t mutiblicity = 0;
	if (i < 0 and i > MUT_NB_MULTIPLICITY) {
		cout << "MFMMutantFrame::GetMultiplicity Error of index\n";
		return 0;
	} else {
		mutiblicity = (((MFM_mut_header*) pHeader)->MutData.EvtCount[i]);
	}
	if (fLocalIsBigEndian != fFrameIsBigEndian)
		SwapInt16(&mutiblicity);
	return mutiblicity;

}
//_______________________________________________________________________________
void MFMMutantFrame::SetEvtCount(int i, uint32_t count) {
	if (i < 0 and i > MUT_NB_EVTCOUNT)
		cout << "MFMMutantFrame::SetEvtCount Error of status index\n";
	else
		((MFM_mut_header*) pHeader)->MutData.EvtCount[i] = count;
}
//_______________________________________________________________________________
uint32_t MFMMutantFrame::GetEvtCount(int i) {
	uint32_t count;
	if (i < 0 and i > MUT_NB_EVTCOUNT) {
		cout << "MFMMutantFrame::GetEvtCount Error of index\n";
		return 0;
	} else {
		count = (((MFM_mut_header*) pHeader)->MutData.EvtCount[i]);
	}
	if (fLocalIsBigEndian != fFrameIsBigEndian)
		SwapInt32(&count);
	return count;
}
//_______________________________________________________________________________
void MFMMutantFrame::SetScaler(int i, uint32_t scaler) {
	if (i < 0 and i > MUT_NB_SCALER)
		cout << "MFMMutantFrame::SetScaler Error of index\n";
	else
		((MFM_mut_header*) pHeader)->MutData.Scaler[i] = scaler;
}
//_______________________________________________________________________________
uint32_t MFMMutantFrame::GetScaler(int i) {
	uint32_t scaler = 0;
	if (i < 0 and i > MUT_NB_SCALER) {
		cout << "MFMMutantFrame::GetScaler Error of index\n";
		return 0;
	} else {
		scaler = (((MFM_mut_header*) pHeader)->MutData.Scaler[i]);
	}
	if (fLocalIsBigEndian != fFrameIsBigEndian)
		SwapInt32(&scaler);
	return scaler;
}

//_______________________________________________________________________________
void MFMMutantFrame::SetD2pTime(uint32_t d2ptime) {
	((MFM_mut_header*) pHeader)->MutData.D2pTime = d2ptime;
}

//_______________________________________________________________________________
uint32_t MFMMutantFrame::GetD2pTime() {
	uint32_t d2ptime = 0;
	d2ptime = (((MFM_mut_header*) pHeader)->MutData.D2pTime);
	if (fLocalIsBigEndian != fFrameIsBigEndian)
		SwapInt32(&d2ptime);
	return d2ptime;
}

//_______________________________________________________________________________
void MFMMutantFrame::FillEventRandomConst(uint64_t timestamp,
		uint32_t enventnumber) {

	/// Fill all data of frame with random values to do test
	/// And report time stamp and event number
	int value = random();
	cout << " debug FillEventRandomConst\n";
	int i = 0;
	uint32_t uivalue = (uint32_t) (4294967296 * value / RAND_MAX);
	for (i = 0; i < MUT_NB_MULTIPLICITY; i++){
		SetMultiplicity(i, 01);
		cout << " debug 1FillEventRandomConst\n";
	}
	for (i = 0; i < MUT_NB_SCALER; i++)
		SetScaler(i, uivalue + i);
	for (i = 0; i < MUT_NB_EVTCOUNT; i++)
		SetEvtCount(i, uivalue + i);
	SetD2pTime(01);
	SetTriggerInfo(01);
	SetEventNumber(enventnumber);
	SetTimeStamp(timestamp);
}
//_______________________________________________________________________________
string MFMMutantFrame::GetHeaderDisplay(char* infotext) {
	stringstream ss;
	string display("");
	display = ss.str();
	ss << MFMCommonFrame::GetHeaderDisplay(infotext);
	ss << "   EN = " << GetEventNumber();
	ss << "   TS = " << GetTimeStamp();
	display = ss.str();
	return display;
}
//_______________________________________________________________________________
string MFMMutantFrame::DumpData(char mode, bool nozero) {
	// Dump parameter Label and parameter value of the current event.
	// if enter parameter is true (default value), all zero parameter of event aren't dumped
	// mode = 'd' for decimal, 'b' for binary, 'h' for hexa, 'o' for octal

	stringstream ss;
	string display("");

	int i;
	char tempo[255];
	sprintf(tempo, "\n|Tigger= %u|\n",GetTriggerInfo());
	ss << tempo;
	sprintf(tempo, "|GetD2pTime = %u|\n", GetD2pTime());
	ss << tempo;
	sprintf(tempo, "|Multiplicity A and B = ");
	ss << tempo;
	for (i = 0; i < MUT_NB_MULTIPLICITY; i++){
		sprintf(tempo, " %u ", GetMultiplicity(i));
		ss << tempo;
	}
	ss << " |\n| Scalers = ";
	for (i = 0; i < MUT_NB_SCALER; i++){
		sprintf(tempo, " %u ",GetScaler(i));
		ss << tempo;
	}
	ss << " |\n| EvCount = ";
	for (i = 0; i < MUT_NB_EVTCOUNT; i++){
			sprintf(tempo, " %u ",GetEvtCount(i));
			ss << tempo;
	}ss << " |\n";

	display = ss.str();
	return display;
}
