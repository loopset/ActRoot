/*
 MFMExogamFrame.cc
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

#include "MFMExogamFrame.h"

//_______________________________________________________________________________
MFMExogamFrame::MFMExogamFrame(int unitBlock_size, int dataSource,
		int frameType, int revision, int frameSize, int headerSize) {
	/// Constructor for a exogam frame . the header is filled with unitblock_size, data source , frame type , revision , frame, size and header size value
	SetPointers();
}

//_______________________________________________________________________________
MFMExogamFrame::MFMExogamFrame() {
	/// Constructor for a empty exogam frame

}
//_______________________________________________________________________________
MFMExogamFrame::~MFMExogamFrame() {
/// destructor of Exogam frame
}
//_______________________________________________________________________________
void MFMExogamFrame::SetBufferSize(int size, bool ifinferior) {
	MFMBlobFrame::SetBufferSize(size, ifinferior);
	MFMExogamFrame::SetPointers();
}
//_______________________________________________________________________________
void MFMExogamFrame::SetPointers(void * pt) {
	MFMBlobFrame::SetPointers(pt);
	pHeader = (MFM_topcommon_header*) pData;
	pData_char = (char*) pData;
}
//_______________________________________________________________________________
void MFMExogamFrame::SetAttributs(void * pt) {
	SetPointers(pt);
	MFMBlobFrame::SetAttributs(pt);
	pUserData_char=(char*)&(((MFM_exo_header*) pHeader)->ExoData);
}
//_______________________________________________________________________________

uint64_t MFMExogamFrame::GetTimeStamp() {
	/// Computer, set attibut and return value of time stamp from  frame
	fTimeStamp = 0;
	uint64_t * timeStamp = &(fTimeStamp);
	memcpy(((char*) (&fTimeStamp)),
			((MFM_exo_header*) pHeader)->ExoEventInfo.EventTime, 6);
	if (fLocalIsBigEndian != fFrameIsBigEndian)
		SwapInt64((timeStamp), 6);
	return fTimeStamp;
}
//_______________________________________________________________________________
uint64_t MFMExogamFrame::GetTimeStampAttribut() {
	/// Return attibut of time stamp
	return fTimeStamp;
}

//_______________________________________________________________________________

int MFMExogamFrame::GetEventNumber() {
	/// Computer, set attibut and return value of event number from  frame
	fEventNumber = 0;
	char * eventNumber = (char*) &(fEventNumber);
	//memcpy(&fEventNumber,((char*)((MFM_exo_header*)pHeader)->exoEvtInfo.eventIdx),4);
	fEventNumber = ((MFM_exo_header*) pHeader)->ExoEventInfo.EventIdx;
	if (fLocalIsBigEndian != fFrameIsBigEndian)
		SwapInt32((uint32_t *) (eventNumber), 4);
	return fEventNumber;
}

//_______________________________________________________________________________
int MFMExogamFrame::GetEventNumberAttribut() {
	/// Return attibut of event number
	return fEventNumber;
}
//_______________________________________________________________________________
void MFMExogamFrame::SetTimeStamp(uint64_t timestamp) {
	/// Set value of Time Stamp in frame

	char* pts = (char*) &timestamp;
	timestamp = timestamp & 0x0000ffffffffffff;
	memcpy(((MFM_exo_header*) pHeader)->ExoEventInfo.EventTime, pts, 6);
}
//_______________________________________________________________________________
void MFMExogamFrame::SetEventNumber(int eventnumber) {
	/// Set Event Number of frame
	((MFM_exo_header*) pHeader)->ExoEventInfo.EventIdx = eventnumber;
}
//_______________________________________________________________________________

uint16_t MFMExogamFrame::ExoGetCristalId() {
	uint16_t cristal = 0;
/// Compute and return the 2 bytes of CristalId()
	cristal = ((MFM_exo_header*) pHeader)->ExoData.CristalId;
	if (fLocalIsBigEndian != fFrameIsBigEndian)
		SwapInt16((&cristal), 2);
	return ((cristal));
}
//_______________________________________________________________________________
uint16_t MFMExogamFrame::ExoGetTGCristalId() {
	/// Compute and return extracted Trigger Request value of CristalId
	return (ExoGetCristalId() & EXO_TRIG_REQ_CRYS_ID_MASK);
}

//_______________________________________________________________________________
void MFMExogamFrame::ExoSetCristalId(uint16_t cristalId) {
	/// Set 16 bits of CristalIdx
	((MFM_exo_header*) pHeader)->ExoData.CristalId = cristalId;
}

//_______________________________________________________________________________

void MFMExogamFrame::ExoSetStatus(int i, uint16_t status) {
	if (i < 0 and i > EXO_NB_STATUS)
		cout << "MFMExogamFrame::ExoSetStatus Error of status index\n";
	if (i == 0)
		((MFM_exo_header*) pHeader)->ExoData.Status1 = status;
	if (i == 1)
		((MFM_exo_header*) pHeader)->ExoData.Status2 = status;
	if (i == 2)
		((MFM_exo_header*) pHeader)->ExoData.Status3 = status;
}
//_______________________________________________________________________________

uint16_t MFMExogamFrame::ExoGetStatus(int i) {
	/// Set Status (0,1 or 2)
	uint16_t status;
	if (i < 0 and i > EXO_NB_STATUS) {
		cout << "MFMExogamFrame::ExGetStatus Error of status index\n";
		return 0;
	} else {
		if (i == 0)
			status = (((MFM_exo_header*) pHeader)->ExoData.Status1);
		if (i == 1)
			status = (((MFM_exo_header*) pHeader)->ExoData.Status2);
		if (i == 2)
			status = (((MFM_exo_header*) pHeader)->ExoData.Status3);
	}
	if (fLocalIsBigEndian != fFrameIsBigEndian)
		SwapInt16(&status);
	return status;
}
//_______________________________________________________________________________

void MFMExogamFrame::ExoSetDetaT(uint16_t deltaT) {
	/// Set Deta Time in frame header
	((MFM_exo_header*) pHeader)->ExoData.DeltaT = deltaT;
}
//_______________________________________________________________________________

uint16_t MFMExogamFrame::ExoGetDeltaT() {
	/// Compute and Return Deta Time from frame header
	uint16_t deltat;
	deltat = (((MFM_exo_header*) pHeader)->ExoData.DeltaT);
	if (fLocalIsBigEndian != fFrameIsBigEndian)
		SwapInt16(&deltat);
	return deltat;
}
//_______________________________________________________________________________

void MFMExogamFrame::ExoSetInnerM(int i, uint16_t inner) {
	/// Set Inner data , 6 MeV (i=0) or inner 20 MeV (i=1)
	if (i < 0 and i >= EXO_NB_INNER_M) {
		cout << "MFMExogamFrame::ExoSetInner6_20M Error of inner index\n";
		return;
	}
	if (i == 0)
		(((MFM_exo_header*) pHeader)->ExoData.Inner6M) = inner;
	if (i == 1)
		(((MFM_exo_header*) pHeader)->ExoData.Inner20M) = inner;

}
//_______________________________________________________________________________


uint16_t MFMExogamFrame::ExoGetInnerM(int i) {
	/// Compute and return Inner data , 6 MeV (i=0) or inner 20 MeV (i=1)
	uint16_t inner;
	if (i < 0 and i >= EXO_NB_INNER_M) {
		cout << "MFMExogamFrame::ExoGetInnerM Error of inner index\n";
		return 0;
	}
	if (i == 0)
		inner = (((MFM_exo_header*) pHeader)->ExoData.Inner6M);
	if (i == 1)
		inner = (((MFM_exo_header*) pHeader)->ExoData.Inner20M);
	if (fLocalIsBigEndian != fFrameIsBigEndian)
		SwapInt16(&inner);
	return inner;
}
//_______________________________________________________________________________

void MFMExogamFrame::ExoSetOuter(int i, uint16_t outer) {
	/// Set Outer data in frame. ( outer A B C D)
	if (i < 0 and i >= EXO_NB_OUTER) {
		cout << "MFMExogamFrame::ExoSetOuter, Error of outer index\n";
		return;
	}
	if (i == 0)
		(((MFM_exo_header*) pHeader)->ExoData.Outer1) = outer;
	if (i == 1)
		(((MFM_exo_header*) pHeader)->ExoData.Outer2) = outer;
	if (i == 2)
		(((MFM_exo_header*) pHeader)->ExoData.Outer3) = outer;
	if (i == 3)
		(((MFM_exo_header*) pHeader)->ExoData.Outer4) = outer;
}
//_______________________________________________________________________________

uint16_t MFMExogamFrame::ExoGetOuter(int i) {
	/// Compute and return  Outer (6 Mev) data  (6 Mev)  (Outer 1 if i=0 .... outer 4 if i=3))
	uint16_t outer;
	if (i < 0 and i >= EXO_NB_OUTER) {
		cout << "MFMExogamFrame::ExoSetOuter, Error of outer index\n";
		return 0;
	}
	if (i == 0)
		outer = (((MFM_exo_header*) pHeader)->ExoData.Outer1);
	if (i == 1)
		outer = (((MFM_exo_header*) pHeader)->ExoData.Outer2);
	if (i == 2)
		outer = (((MFM_exo_header*) pHeader)->ExoData.Outer3);
	if (i == 3)
		outer = (((MFM_exo_header*) pHeader)->ExoData.Outer4);

	if (fLocalIsBigEndian != fFrameIsBigEndian)
		SwapInt16(&outer);
	return outer;
}
//_______________________________________________________________________________

void MFMExogamFrame::ExoSetBGO(uint16_t bgo) {
	/// Set BGO in frame
	(((MFM_exo_header*) pHeader)->ExoData.BGO) = bgo;
}
//_______________________________________________________________________________

uint16_t MFMExogamFrame::ExoGetBGO() {
	/// computer and return  BGO value from frame
	uint16_t bgo;
	bgo = (((MFM_exo_header*) pHeader)->ExoData.BGO);
	if (fLocalIsBigEndian != fFrameIsBigEndian)
		SwapInt16(&bgo);
	return bgo;
}
//_______________________________________________________________________________

void MFMExogamFrame::ExoSetCsi(uint16_t csi) {

	/// Set Csi data in frame
	(((MFM_exo_header*) pHeader)->ExoData.Csi) = csi;
}
//_______________________________________________________________________________

uint16_t MFMExogamFrame::ExoGetCsi() {
	/// Compute and return Csi data from frame
	uint16_t csi;
	csi = (((MFM_exo_header*) pHeader)->ExoData.Csi);
	if (fLocalIsBigEndian != fFrameIsBigEndian)
		SwapInt16(&csi);
	return csi;
}
//_______________________________________________________________________________

void MFMExogamFrame::ExoSetInnerT(int i, uint16_t inner) {
	/// Set Inner Time data in frame ( i=0 for T=30%,  i=1 for T=60%,  i=2 for T=90%)
	if (i < 0 and i >= EXO_NB_INNER_T) {
		cout << "MFMExogamFrame::ExoSetInnerT, Error of innerT index\n";
		return;
	}

	if (i == 0)
		(((MFM_exo_header*) pHeader)->ExoData.InnerT30) = inner;
	if (i == 1)
		(((MFM_exo_header*) pHeader)->ExoData.InnerT60) = inner;
	if (i == 2)
		(((MFM_exo_header*) pHeader)->ExoData.InnerT90) = inner;
	return;

}
//_______________________________________________________________________________

uint16_t MFMExogamFrame::ExoGetInnerT(int i) {
	/// Compute and return Innet time data from frame ( i=0 for T=30%,  i=1 for T=60%,  i=2 for T=90%)
	uint16_t innerT;
	if (i < 0 and i >= EXO_NB_INNER_T) {
		cout << "MFMExogamFrame::ExoGetInnerT, Error of innerT index\n";
		return 0;
	}
	if (i == 0)
		innerT = (((MFM_exo_header*) pHeader)->ExoData.InnerT30);
	if (i == 1)
		innerT = (((MFM_exo_header*) pHeader)->ExoData.InnerT60);
	if (i == 2)
		innerT = (((MFM_exo_header*) pHeader)->ExoData.InnerT90);

	if (fLocalIsBigEndian != fFrameIsBigEndian)
		SwapInt16(&innerT);
	return innerT;

}
//_______________________________________________________________________________
void MFMExogamFrame::ExoSetPara(int i, uint16_t value) {
	/// Generic method to set data in frame
	if (i < 0 and i >= EXO_NB_MAX_PARA) {
		cout << "MFMExogamFrame::ExoSetPara, Error of para index\n";
		return;
	}
	uint16_t * pvalue;

	//    pvalue =	(uint16_t *) (pHeader)+ EXO_HEADERFRAMESIZE;
	pvalue = (uint16_t *) (&(((MFM_exo_header*) pHeader)->ExoData));
	pvalue[i] = value;
	//cout<<(int*)&(pvalue[i])<<"\n";;
	return;
}

//_______________________________________________________________________________

uint16_t MFMExogamFrame::ExoGetPara(int i) {
	/// Generic method to get data in frame
	uint16_t value;

	if (i < 0 and i >= EXO_NB_MAX_PARA) {
		cout << "MFMExogamFrame::ExoSetPara,Error of para index\n";
		return 0;
	}

	uint16_t *pvalue = (uint16_t *) (&(((MFM_exo_header*) pHeader)->ExoData));
	value = pvalue[i];

	if (fLocalIsBigEndian != fFrameIsBigEndian)
		SwapInt16(&value);
	return value;

}
//_______________________________________________________________________________

void MFMExogamFrame::ExoSetPadding(uint16_t padding) {
	/// Set Padding data
	(((MFM_exo_header*) pHeader)->ExoData.Padding) = padding;
}
//_______________________________________________________________________________

uint16_t MFMExogamFrame::ExoGetPadding() {
	/// Compute and return value of Padding
	uint16_t padding;
	padding = (((MFM_exo_header*) pHeader)->ExoData.Padding);
	if (fLocalIsBigEndian != fFrameIsBigEndian)
		SwapInt16(&padding);
	return padding;
}

//_______________________________________________________________________________
void MFMExogamFrame::FillEventRandomConst(uint64_t timestamp,
		uint32_t enventnumber) {

	/// Fill all data of frame with random values to do test
	/// And report time stamp and event number
	int value = random();
	int i = 0;
	uint32_t uivalue = (uint32_t) (4294967296 * value / RAND_MAX);
	for (i = 0; i < EXO_NB_MAX_PARA; i++) {
		ExoSetPara(i, uivalue);

	}

	SetEventNumber(enventnumber);
	SetTimeStamp(timestamp);
}
//_______________________________________________________________________________
string MFMExogamFrame::GetHeaderDisplay(char* infotext) {
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
