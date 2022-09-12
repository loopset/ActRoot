/*
 MFMCoboFrame.cc
 Copyright Acquisition group, GANIL Caen, France
 */

#include <iostream>
#include <cmath>
#include <string.h>
#include <iostream>
#include <stdio.h>
#include <sstream>
#include <cstdlib>
using namespace std;

#include "MFMCoboFrame.h"

//_______________________________________________________________________________
MFMCoboFrame::MFMCoboFrame(int unitBlock_size, int dataSource, int frameType,
		int revision, int frameSize, int headerSize, int itemSize, int nItems) {
	fNbMaxItem = COBO_NB_AGET * COBO_NB_AGET_CHANNEL * COBO_NB_SAMPLES;
	SetPointers();

}
//_______________________________________________________________________________
MFMCoboFrame::MFMCoboFrame() {
	fNbMaxItem = COBO_NB_AGET * COBO_NB_AGET_CHANNEL * COBO_NB_SAMPLES;
}
//_______________________________________________________________________________
MFMCoboFrame::~MFMCoboFrame() {
	//cout << "debug destructor of MFMCoboFrame::()\n";
}

//_______________________________________________________________________________
void MFMCoboFrame::SetBufferSize(int size, bool ifinferior) {
	MFMBasicFrame::SetBufferSize(size, ifinferior);
	MFMCoboFrame::SetPointers();
}
//_______________________________________________________________________________
void MFMCoboFrame::SetPointers(void * pt) {
	MFMBasicFrame::SetPointers(pt);
	pHeader = (MFM_topcommon_header*) pData;
	pData_char = (char*) pData;
}
//_______________________________________________________________________________
void MFMCoboFrame::SetAttributs(void * pt) {
	SetPointers(pt);
	MFMBasicFrame::SetAttributs(pt);
}

//_______________________________________________________________________________

uint64_t MFMCoboFrame::GetTimeStamp() {
	// Compute and return Time Stamp
	fTimeStamp = 0;
	uint64_t * timeStamp = &(fTimeStamp);
	memcpy(((char*) (&fTimeStamp)),
			((MFM_cobo_header*) pHeader)->coboEvtInfo.eventTime, 6);
	if (fLocalIsBigEndian != fFrameIsBigEndian)
		SwapInt64((timeStamp), 6);
	return fTimeStamp;
}
//_______________________________________________________________________________
uint64_t MFMCoboFrame::GetTimeStampAttribut() {
	// return Time Stamp attribut without computing
	return fTimeStamp;
}

//_______________________________________________________________________________
string MFMCoboFrame::GetHeaderDisplay(char* infotext) {
	stringstream ss;
	string display("");
	display = ss.str();

	ss << MFMBasicFrame::GetHeaderDisplay(infotext);
	ss << "   EN = " << GetEventNumber();
	ss << " TS = " << GetTimeStamp();
	ss << " CoboIdx = " << CoboGetCoboIdx();
	ss << " CoboAsaIdx = " << CoboGetAsaIdx();
	display = ss.str();
	return display;
}

//_______________________________________________________________________________

int MFMCoboFrame::GetEventNumber() {
	// compute and get Event Number
	fEventNumber = 0;
	char * eventNumber = (char*) &(fEventNumber);
	//memcpy(&fEventNumber,((char*)((MFM_cobo_header*)pHeader)->coboEvtInfo.eventIdx),4);
	fEventNumber = ((MFM_cobo_header*) pHeader)->coboEvtInfo.eventIdx;
	if (fLocalIsBigEndian != fFrameIsBigEndian)
		SwapInt32((uint32_t *) (eventNumber), 4);
	return fEventNumber;

}

//_______________________________________________________________________________
int MFMCoboFrame::GetEventNumberAttribut() {
	// return fEventNumber without computing it
	return fEventNumber;
}

//_______________________________________________________________________________
void MFMCoboFrame::SetTimeStamp(uint64_t timestamp) {
	char* pts = (char*) &timestamp;
	timestamp = timestamp & 0x0000ffffffffffff;
	memcpy(((MFM_cobo_header*) pHeader)->coboEvtInfo.eventTime, pts, 6);
}

//_______________________________________________________________________________
void MFMCoboFrame::SetEventNumber(int eventnumber) {
	((MFM_cobo_header*) pHeader)->coboEvtInfo.eventIdx = eventnumber;
}
//_______________________________________________________________________________
//COBO
int MFMCoboFrame::CoboGetCoboIdx() {
	return ((int) (((MFM_cobo_header*) pHeader)->coboBoard.coboIdx));
}
//_______________________________________
int MFMCoboFrame::CoboGetAsaIdx() {
	return (int) (((MFM_cobo_header*) pHeader)->coboBoard.asaIdx);
}
//_______________________________________________________________________________
int MFMCoboFrame::CoboGetReadOffset() {
	return (int) (((MFM_cobo_header*) pHeader)->coboBoard.readOffset);
}
//_______________________________________________________________________________
int MFMCoboFrame::CoboGetStatus() {
	return (int) (((MFM_cobo_header*) pHeader)->coboBoard.status);
}

//_______________________________________________________________________________
char* MFMCoboFrame::CoboGetHitPat(int i) {
	// 0<=i<=3
	return (char*) ((MFM_cobo_header*) pHeader)->coboHit.hitPat_0 + 9 * i;
}
//_______________________________________________________________________________
char* MFMCoboFrame::CoboGetMultip(int i) {
	// 0<=i<=3
	return (char*) ((MFM_cobo_header*) pHeader)->coboHit.hitPat_0 + 9 * 4 + 2* i;
}

//_______________________________________________________________________________
uint32_t MFMCoboFrame::CoboGetWindowOut() {
	return (uint32_t)(((MFM_cobo_header*) pHeader)->coboOut.windowOut);
}
//_______________________________________________________________________________
char* MFMCoboFrame::CoboGetLastCell(int i) {
	// 0<=i<=3
	return(char*)((MFM_cobo_header*) pHeader)->coboOut.lastCell_0+2*i;
}
//_______________________________________________________________________________
void MFMCoboFrame::CoboSetCoboIdx(int coboidx) {
	((MFM_cobo_header*) pHeader)->coboBoard.coboIdx = coboidx;
}
//_______________________________________
void MFMCoboFrame::CoboSetAsaIdx(int asaidx) {
	((MFM_cobo_header*) pHeader)->coboBoard.asaIdx = asaidx;
}
//_______________________________________________________________________________
void MFMCoboFrame::CoboSetReadOffset(int offset) {
	((MFM_cobo_header*) pHeader)->coboBoard.readOffset = offset;
}
//_______________________________________________________________________________
void MFMCoboFrame::CoboSetStatus(int status) {
	((MFM_cobo_header*) pHeader)->coboBoard.status = status;
}

//_______________________________________________________________________________
void MFMCoboFrame::CoboSetHitPat(int i, char * hitpat) {
	memcpy(((MFM_cobo_header*) pHeader)->coboHit.hitPat_0 + 9 * i, hitpat, 9);
}

//_______________________________________________________________________________
void MFMCoboFrame::CoboSetMultip(int i, char* multip) {
	memcpy(((MFM_cobo_header*) pHeader)->coboHit.hitPat_0 + 9 * 4 + 2 * i,
			multip, 2);
}
//_______________________________________________________________________________
void MFMCoboFrame::CoboSetWindowOut(uint32_t windowsout) {
((MFM_cobo_header*) pHeader)->coboOut.windowOut = windowsout;
}

//_______________________________________________________________________________
void MFMCoboFrame::CoboSetLastCell(int i, char* cell) {
	memcpy(((MFM_cobo_header*) pHeader)->coboOut.lastCell_0+ 2 * i,cell, 2);
}
//_______________________________________________________________________________
void MFMCoboFrame::CoboGetParameters(int i, uint32_t *sample,
		uint32_t *buckidx, uint32_t *chanidx, uint32_t *agetidx) {
if (i==0)CoboRazCounts();

	if (GetFrameTypeAttribut() == MFM_COBO_FRAME_TYPE) {
		CoboGetParametersByItem((MFM_coboItem *) GetItem(i), sample, buckidx,
				chanidx, agetidx);
	}

	if (GetFrameTypeAttribut() == MFM_COBOF_FRAME_TYPE) {
		CoboGetParametersByItem((MFM_cobofItem *) GetItem(i), sample,  agetidx);

		// first method
		*chanidx = (((uint32_t)(i / (COBO_NB_AGET *2)))*2 + (i % 2))% COBO_NB_AGET_CHANNEL;
		*buckidx = ((uint32_t)(i / (COBO_NB_AGET * COBO_NB_AGET_CHANNEL)));

		//Second method
		//*chanidx =channelcount[*agetidx]++;
		//*buckidx= bucketcount[*agetidx];

		if (channelcount[*agetidx]==(COBO_NB_AGET_CHANNEL)){
			 channelcount[*agetidx]=0;
			 bucketcount[*agetidx]++;
			 }
	}
}
//_______________________________________________________________________________
void MFMCoboFrame::CoboGetParametersByItem(MFM_coboItem *item,uint32_t * sample,
		uint32_t *buckidx, uint32_t *chanidx,uint32_t *agetidx) {

	int type = GetFrameTypeAttribut();
	if (type == MFM_COBO_FRAME_TYPE) {
		uint32_t ui, ui2;
		ui2 = 0;
		char * pt_ui = (char*) &ui;
		memcpy(pt_ui, (char*) item, fItemSize);
		if (fLocalIsBigEndian != fFrameIsBigEndian)
			SwapInt32((uint32_t*) pt_ui, 4);

		//sample form 0 to 11
		ui2 = ui;
		ui2 = ui2 & COBO_SAMPLE_MASK;
		*sample = ui2;

		//buckdx from 14 to 22
		ui2 = ui;
		ui2 = ui2 >> 14;
		ui2 = ui2 & COBO_BUCKIDX_MASK;
		*buckidx = ui2;

		//chanidx from 23 to 29
		ui2 = ui;
		ui2 = ui2 >> 23;
		ui2 = ui2 & COBO_CHANIDX_MASK;
		*chanidx = ui2;

		//agetidx from 30 to 31
		ui2 = ui;
		ui2 = ui2 >> 30;
		ui2 = ui2 & COBO_AGETIDX_MASK;
		*agetidx = ui2;

	}else {
		fError.TreatError(2, 0,
				"MFMCoboFrame::CoboGetParametersByItem FrameType not MFM_COBO_FRAME_TYPE");
	}
}
//_______________________________________________________________________________
void MFMCoboFrame::CoboGetParametersByItem(MFM_cobofItem *item, uint32_t * sample, uint32_t *agetidx) {

	int type = GetFrameTypeAttribut();
	if (type == MFM_COBOF_FRAME_TYPE) {
		uint16_t ui;
		uint32_t ui2;
		ui2 = 0;
		*agetidx = 0;
		*sample = 0;
		char * pt_ui = (char*) &ui;
		memcpy(pt_ui, (char*) item, fItemSize);
		if (fLocalIsBigEndian != fFrameIsBigEndian)
			SwapInt16((uint16_t*) pt_ui);
		//sample form 0 to 11
		ui2 = ui;
		ui2 = ui2 & COBO_SAMPLE_MASK;
		*sample = ui2;

		//agetidx from  14 to 15
		ui2 = ui;
		ui2 = ui2 >> 14;
		ui2 = ui2 & COBO_AGETIDX_MASK;
		*agetidx = ui2;
	}else{
		fError.TreatError(2, 0,
				"MFMCoboFrame::CoboGetParametersByItem FrameType not MFM_COBO_FRAME_TYPE");
	}
}
//_______________________________________________________________________________
void MFMCoboFrame::CoboRazCounts(){
for (int i=0;i>COBO_NB_AGET;i++){
	  channelcount[i]=0;
	  bucketcount[i]=0;
	}
}
//_______________________________________________________________________________
void MFMCoboFrame::CoboSetParameters(uint32_t sample, uint32_t buckidx,
		uint32_t chanidx, uint32_t agetidx) {
	// to use only with MFM_COBOF_FRAME_TYPE

	int i = buckidx * (COBO_NB_AGET * COBO_NB_AGET_CHANNEL) + chanidx % 2
			+ (chanidx / 2) * (COBO_NB_AGET * 2) + 2 * agetidx;

	if (i >= fNbMaxItem) {
		std::ostringstream tempos;
		tempos << "Index to hight " << i << " >= " << fNbMaxItem
				<< "  agetidx = " << agetidx << "  chanidx = " << chanidx
				<< " " << "  buckidx = " << buckidx;
		fError.TreatError(2, 0, (tempos.str()).data());
	}
	CoboSetParametersByItem((MFM_cobofItem *) GetItem(i), sample, agetidx);
}
//_______________________________________________________________________________
void MFMCoboFrame::CoboSetParameters(int i, uint32_t sample, uint32_t buckidx,
		uint32_t chanidx, uint32_t agetidx) {
	int type = GetFrameTypeAttribut();

	if (type == MFM_COBO_FRAME_TYPE) {
		CoboSetParametersByItem((MFM_coboItem *) GetItem(i), sample, buckidx,
				chanidx, agetidx);
	}
	if (type == MFM_COBOF_FRAME_TYPE) {
		CoboSetParametersByItem((MFM_cobofItem *) GetItem(i), sample, agetidx);
	}
}
//_______________________________________________________________________________
void MFMCoboFrame::CoboSetParametersByItem(MFM_coboItem *item, uint32_t sample,
		uint32_t buckidx, uint32_t chanidx, uint32_t agetidx) {
	int type = GetFrameTypeAttribut();

	if (type == MFM_COBO_FRAME_TYPE) {

				// Methode to fill item in case of MFM_COBO_FRAME_TYPE frame
				uint32_t ui, ui2;
				ui2 = 0;

				//char * pt_ui = (char*) &ui;
				char * pt_ui2 = (char*) &ui2;
				//sample form 0 to 11
				ui2 = sample & COBO_SAMPLE_MASK;

				//buckdx from 14 to 22
				ui = buckidx & COBO_BUCKIDX_MASK;
				ui = ui << 14;
				ui2 = ui2 | ui;

				//chanidx from 23 to 29
				ui = chanidx & COBO_CHANIDX_MASK;
				ui = ui << 23;
				ui2 = ui2 | ui;

				//agetidx from 30 to 31
				ui = agetidx & COBO_AGETIDX_MASK;
				ui = ui << 30;
				ui2 = ui2 | ui;

				memcpy((char*) item, pt_ui2, fItemSize);
			}else{
				fError.TreatError(2,0,"MFMCoboFrame::CoboSetParametersByItem not a MFM_COBO_FRAME_TYPE ");
			}

}
//_______________________________________________________________________________
void MFMCoboFrame::CoboSetParametersByItem(MFM_cobofItem *item,
		uint32_t sample,  uint32_t agetidx) {
	int type = GetFrameTypeAttribut();

	if (type == MFM_COBOF_FRAME_TYPE) {
		uint16_t ui, ui2;
		ui2 = 0;

		char * pt_ui2 = (char*) &ui2;
		//sample form 0 to 11
		ui2 = sample & COBO_SAMPLE_MASK;

		//agetidx from 14 to 15
		ui = agetidx & COBO_AGETIDX_MASK;
		ui = ui << 14;
		ui2 = ui2 | ui;

		memcpy((char*) item, pt_ui2, fItemSize);

	}else{
		fError.TreatError(2,0,"MFMCoboFrame::CoboSetParametersByItem not a MFM_COBOF_FRAME_TYPE ");
	}
}

//_______________________________________________________________________________
void MFMCoboFrame::FillEventWithRamp(uint32_t samplemax, uint32_t channelmax,
		uint32_t agetmax, uint64_t timestamp, uint32_t enventnumber,
		uint32_t type) {

	uint32_t count;
	count = 0;
	SetEventNumber(enventnumber);
	SetTimeStamp(timestamp);
	int i, j;
	uint32_t value;
	uint32_t asad;
	asad = CoboGetAsaIdx();
		for (i = 0; i < COBO_NB_AGET; i++) {
			for (j = 0; j < COBO_NB_AGET_CHANNEL; j++) {
				nbentries[i][j] = 0;
				coef[i][j] = (int) (rand()%23)+1;
			}
		}


	for (uint32_t ageti = 0; ageti < agetmax; ageti++) {
		for (uint32_t channeli = 0; channeli < channelmax; channeli++) {
			for (uint32_t bucketi = 0; bucketi < samplemax; bucketi++) {
				value = bucketi;
				value = (int)((((float)(bucketi))/((float)(coef[ageti][channeli]))));
				if (bucketi == 0)
					value = asad;
				if (bucketi == 1)
					value = ageti;
				if (bucketi == 2)
					value = channeli;
				if (bucketi == samplemax-1)
					value=samplemax;
				nbentries[ageti][channeli]++;
				if (type == MFM_COBO_FRAME_TYPE) {
					CoboSetParameters(count, value, bucketi, channeli, ageti);
				}
				if (type == MFM_COBOF_FRAME_TYPE) {
					CoboSetParameters(value, bucketi, channeli, ageti);
				}
				count++;
			}
		}
	}

	for (i = 0; i < COBO_NB_AGET; i++) {
		for (j = 0; j < COBO_NB_AGET_CHANNEL; j++) {
			if ((uint32_t)nbentries[i][j] > samplemax)
				cout << "Error count " << asad << " " << i << " " << j << " = "
						<< nbentries[i][j] << "\n";
		}
	}

}
//_______________________________________________________________________________
void MFMCoboFrame::GenerateACoboExample(int type, int eventnumber,
		int asadnumber) {
	// generate a example of coba frame

	uint32_t cobo_nb_sample = COBO_NB_SAMPLES;
	//cobo_nb_sample=128;
	uint32_t cobo_aget_channel = COBO_NB_AGET_CHANNEL;
	uint32_t cobo_nb_aget = COBO_NB_AGET;
	uint32_t unitBlock_size = 64;
	uint32_t headersize = 128;
	uint32_t itemsize = 4;
	uint32_t nbitem = cobo_nb_sample * cobo_aget_channel * cobo_nb_aget;
	uint32_t revision = 4;


	if (type == MFM_COBO_FRAME_TYPE)
		itemsize = 4;
	if (type == MFM_COBOF_FRAME_TYPE)
		itemsize = 2;
	uint32_t framesize = nbitem * itemsize + headersize;
	// generation of MFM header , in this case, MFM is same for all MFM frames
	MFM_make_header(unitBlock_size, 0, type, revision, (int) (framesize
			/ unitBlock_size), (headersize / unitBlock_size), itemsize, nbitem);
	CoboSetAsaIdx(asadnumber);
	CoboSetReadOffset(COBO_NB_SAMPLES-cobo_nb_sample);
	FillEventWithRamp(cobo_nb_sample, cobo_aget_channel, cobo_nb_aget,
			GetTimeStampUs(), eventnumber, type);

}
//_______________________________________________________________________________
void MFMCoboFrame::InitStat() {
	MFMCommonFrame::InitStat();
	fCountCoboFrame.reserve(8);
	fCountEmptyCoboFrame.reserve(8);
	int i;
    for ( i= 0;i<8;i++){
	        	fCountCoboFrame[i]=0;
	        	fCountEmptyCoboFrame[i]=0;
	        }
}
//____________________________________________________________________
void MFMCoboFrame::FillStat() {
	MFMCommonFrame::FillStat();
    int cobo= CoboGetCoboIdx();
	int asad= CoboGetAsaIdx();
	int indice;
	indice = cobo*4*cobo +asad ;
	fCountCoboFrame[indice]++;
	int framesize =GetFrameSize();
	int headersize= GetHeaderSize();
	if (framesize==headersize)fCountEmptyCoboFrame[indice]++;
}
//____________________________________________________________________
string MFMCoboFrame::GetStat(string info  ){
	string display("");
	stringstream ss;
	ss <<MFMCommonFrame::GetStat( info);
	if (GetCountFrame()!=0)
	for ( int i=0;i<8; i++){
							ss << "Number of CoboFrame (cobo = "<<i/4<<", asad = "<<i%4<<") = "<<fCountCoboFrame[i]<<endl;
							ss << " included number of empty cobo frame ( headersize = framesize)"<<fCountEmptyCoboFrame[i]<<endl;
						}
	display = ss.str();
	return display;
}
//____________________________________________________________________
void MFMCoboFrame::PrintStat(string info ){
cout << (GetStat(info));
}
//____________________________________________________________________
