/*
 MFMEbyedatFrame.cc
 Copyright Acquisition group, GANIL Caen, France
 */

#include <iostream>
#include <cmath>
#include <string.h>
#include <iostream>
#include <stdio.h>
#include <sstream>
#include <stdlib.h>
#include <time.h>
using namespace std;

#include "MFMEbyedatFrame.h"

//_______________________________________________________________________________
MFMEbyedatFrame::MFMEbyedatFrame(int unitBlock_size, int dataSource,
		int frameType, int revision, int frameSize, int headerSize,
		int itemSize, int nItems) {
	/// Constructor of frame with a memory space\n
	/// fill header information : unitBlock_size,dataSource,....
	SetPointers();
	fLabelIndice =NULL;
	fNbLabels=NULL;
	fIndiceLabel=NULL;
}

//_______________________________________________________________________________
MFMEbyedatFrame::MFMEbyedatFrame() {
	/// Constructor of a empty frame object
	fLabelIndice =NULL;
	fNbLabels=NULL;
	fIndiceLabel=NULL;

}
//_______________________________________________________________________________
MFMEbyedatFrame::~MFMEbyedatFrame() {
	///Destructor
}
//_______________________________________________________________________________
void MFMEbyedatFrame::SetBufferSize(int size, bool ifinferior) {
	/// Do memory allocation or a reallacation for frame\n
	/// if ifinferior==true the allocaton is forced to size event if the acutal size is bigger\n
	MFMBasicFrame::SetBufferSize(size, ifinferior);
	MFMEbyedatFrame::SetPointers();
}
//_______________________________________________________________________________
void MFMEbyedatFrame::SetPointers(void * pt) {
	/// Initialize pointers of frame\n
	/// if pt==NULL initialization is with current value of main pointer of frame (pData)\n
	/// else initialization is done with pData = pt\n
	/// pData must be the reference;
	MFMBasicFrame::SetPointers(pt);
	pHeader = (MFM_topcommon_header*) pData;
	pData_char = (char*) pData;
}
//_______________________________________________________________________________
void MFMEbyedatFrame::SetAttributs(void * pt) {
	SetPointers(pt);
	MFMBasicFrame::SetAttributs(pt);
}
//_______________________________________________________________________________
string MFMEbyedatFrame::GetHeaderDisplay(char* infotext) {
	stringstream ss;
	string display("");
	display = ss.str();
	uint16_t type = GetFrameType();
	ss << MFMCommonFrame::GetHeaderDisplay(infotext);
	if ((type == MFM_EBY_EN_FRAME_TYPE) || (type == MFM_EBY_EN_TS_FRAME_TYPE)) {
		ss << "   EN = " << GetEventNumber();
	}
	if ((type == MFM_EBY_TS_FRAME_TYPE) || (type == MFM_EBY_EN_TS_FRAME_TYPE)) {
		ss << "   TS = " << GetTimeStamp();
	}
	ss << endl;
	display = ss.str();
	return display;
}
//_______________________________________________________________________________

uint64_t MFMEbyedatFrame::GetTimeStamp() {
	/// Compute time stamp and fill fTimeStamp attribut. return value of TimeStamp
	fTimeStamp = 0;
	uint64_t* timeStamp = &(fTimeStamp);

	if (GetFrameTypeAttribut() == MFM_EBY_EN_TS_FRAME_TYPE) {
		memcpy(((char*) (&fTimeStamp)),
				((MFM_Ebyedat_ENTSheader*) pHeader)->EbyedatEvtInfo.eventTime,
				6);
		if (fLocalIsBigEndian != fFrameIsBigEndian)
			SwapInt64((timeStamp), 6);
	}
	if (GetFrameTypeAttribut() == MFM_EBY_TS_FRAME_TYPE) {
		memcpy(((char*) (&fTimeStamp)),
				((MFM_Ebyedat_TSheader*) pHeader)->EbyedatEvtInfo.eventTime, 6);
		if (fLocalIsBigEndian != fFrameIsBigEndian)
			SwapInt64((timeStamp), 6);
	}
	if (GetFrameTypeAttribut() == MFM_EBY_EN_FRAME_TYPE) {
		//nothing to do and we leave fTimeStamp to 0
		fTimeStamp = 0;
	}
	return fTimeStamp;
}
//_______________________________________________________________________________
uint64_t MFMEbyedatFrame::GetTimeStampAttribut() {
	/// return time stamp without computing it
	return fTimeStamp;
}

//_______________________________________________________________________________

int MFMEbyedatFrame::GetEventNumber() {
	/// Compute and return envent number
	fEventNumber = 0;
	char * eventNumber = (char*) &(fEventNumber);

	if (GetFrameTypeAttribut() == MFM_EBY_EN_TS_FRAME_TYPE) {
		fEventNumber
				= ((MFM_Ebyedat_ENTSheader*) pHeader)->EbyedatEvtInfo.eventIdx;
		if (fLocalIsBigEndian != fFrameIsBigEndian)
			SwapInt32((uint32_t *) (eventNumber), 4);
	}

	if (GetFrameTypeAttribut() == MFM_EBY_TS_FRAME_TYPE) {
		//nothing to do and we leave fEventNumber to 0
		fEventNumber = 0;
	}

	if (GetFrameTypeAttribut() == MFM_EBY_EN_FRAME_TYPE) {
		fEventNumber
				= ((MFM_Ebyedat_ENheader*) pHeader)->EbyedatEvtInfo.eventIdx;
		if (fLocalIsBigEndian != fFrameIsBigEndian)
			SwapInt32((uint32_t *) (eventNumber), 4);

	}
	return fEventNumber;
}
//_______________________________________________________________________________
int MFMEbyedatFrame::GetEventNumberAttibut() {
	/// Return event number without computing it
	return fEventNumber;
}

//_______________________________________________________________________________
void MFMEbyedatFrame::SetTimeStamp(uint64_t timestamp) {
	// Set frame timestamp
	char* pts = (char*) &timestamp;
	timestamp = timestamp & 0x0000ffffffffffff;
	if (GetFrameTypeAttribut() == MFM_EBY_EN_TS_FRAME_TYPE)
		memcpy(((MFM_Ebyedat_ENTSheader*) pHeader)->EbyedatEvtInfo.eventTime,
				pts, 6);
	if (GetFrameTypeAttribut() == MFM_EBY_TS_FRAME_TYPE)
		memcpy(((MFM_Ebyedat_TSheader*) pHeader)->EbyedatEvtInfo.eventTime,
				pts, 6);
	if (GetFrameTypeAttribut() == MFM_EBY_EN_FRAME_TYPE) {//do nothing
	}
}

//_______________________________________________________________________________
void MFMEbyedatFrame::SetEventNumber(uint32_t eventnumber) {
	/// set frame event number
	if (GetFrameTypeAttribut() == MFM_EBY_EN_TS_FRAME_TYPE)
		((MFM_Ebyedat_ENTSheader*) pHeader)->EbyedatEvtInfo.eventIdx
				= eventnumber;

	if (GetFrameTypeAttribut() == MFM_EBY_TS_FRAME_TYPE) {//do nothing

	}
	if (GetFrameTypeAttribut() == MFM_EBY_EN_FRAME_TYPE)
		((MFM_Ebyedat_ENheader*) pHeader)->EbyedatEvtInfo.eventIdx
				= eventnumber;
}

//_______________________________________________________________________________
void MFMEbyedatFrame::EbyedatGetParameters(int i, uint16_t *label,
		uint16_t *value) {
	/// Compute and return the couple information of label /value of the i-th item
	EbyedatGetParametersByItem((MFM_EbyedatItem *) GetItem(i), label, value);
}
//_______________________________________________________________________________
void MFMEbyedatFrame::EbyedatGetParametersByItem(MFM_EbyedatItem *item,
		uint16_t * label, uint16_t *value) {
	/// Compute and return the couple information of label /value of  item

	if (fLocalIsBigEndian != fFrameIsBigEndian) {
		uint16_t tmp = item->Value;
		SwapInt16(&tmp);
		*value = tmp;
		tmp = item->Label;
		SwapInt16(&tmp);
		*label = tmp;
	} else {
		*value = item->Value;
		*label = item->Label;
	}

}
//_______________________________________________________________________________
void MFMEbyedatFrame::EbyedatSetParameters(int i, uint16_t label,
		uint16_t value) {
	/// set i-th item if frame with the couple information of label /value
	EbyedatSetParametersByItem((MFM_EbyedatItem *) GetItem(i), label, value);
}
//_______________________________________________________________________________
void MFMEbyedatFrame::EbyedatSetParametersByItem(MFM_EbyedatItem *item,
		uint16_t label, uint16_t value) {
	/// set "item" with the couple information of label /value
	item->Label = label;
	item->Value = value;
}
//_______________________________________________________________________________
void MFMEbyedatFrame::FillEventWithRamdomConst(uint64_t timestamp,
		uint32_t enventnumber) {
	/// Fill frame items with ramdom values
	float randval;
	uint16_t i = 0;
	if (GetNbItems() > 0)
		EbyedatSetParameters(0, 1, 1);

	for (i = 1; i < GetNbItems(); i++) {
		randval = random();
		uint16_t uivalue = (uint16_t) (65536 * randval / RAND_MAX);

		EbyedatSetParameters(i, i + 1, uivalue);
	}
	SetEventNumber(enventnumber);
	SetTimeStamp(timestamp);
}

//_______________________________________________________________________________

void MFMEbyedatFrame::GenerateAEbyedatExample(int type, int eventnumber) {
	/// Generate a example of frame containing random value\n
	/// usable for tests.
	if ((type != MFM_EBY_EN_FRAME_TYPE) and (type != MFM_EBY_TS_FRAME_TYPE)
			and (type != MFM_EBY_EN_TS_FRAME_TYPE)) {
		cout
				<< "Error in  MFMEbyedatFrame::GenerateAEbyedatExample type not understood\n";
		return;
	}

	uint32_t unitBlock_size = 0;
	uint32_t itemsize = 0;
	uint32_t nbitem = 0;
	uint32_t framesize = 0;
	uint32_t revision = 1;
	uint32_t headersize = 0;
	unitBlock_size = 2;
	itemsize = 4;
	nbitem = 20;
	if (type == MFM_EBY_EN_FRAME_TYPE) {
		headersize = EBYEDAT_EN_HEADERSIZE;
	}
	if (type == MFM_EBY_TS_FRAME_TYPE) {
		headersize = EBYEDAT_TS_HEADERSIZE;
	}
	if (type == MFM_EBY_EN_TS_FRAME_TYPE) {
		headersize = EBYEDAT_ENTS_HEADERSIZE;
	}
	framesize = headersize + nbitem * itemsize;
	revision = 1;
	// generation of MFM header , in this case, MFM is same for all MFM frames
	MFM_make_header(unitBlock_size, 0, type, revision, (int) (framesize
			/ unitBlock_size), (headersize / unitBlock_size), itemsize, nbitem);
	FillEventWithRamdomConst(GetTimeStampUs(), eventnumber);
}
//____________________________________________________________________________

string MFMEbyedatFrame::DumpData(char mode, bool nozero) {
	// Dump parameter Label and parameter value of the current event.
	// if enter parameter is true (default value), all zero parameter of event aren't dumped
	// mode = 'd' for decimal, 'b' for binary, 'h' for hexa, 'o' for octal

	stringstream ss;
	string display("");

	int i, j, maxbin, presentation = 0, max_presentation = 5;
	char tempo[255];
	char Bin[255];
	char Bin2[255];
	uint16_t value;
	uint16_t label;
	int reste;
	char one = '1', zero = '0';
	int DecNumber = 0;

	if (mode == 'b')
		max_presentation = 3;
	if (GetEventNumber() == -1) {
		ss << "No Event , so no event dump. Get a new event frame";
	} else {
		for (i = 0; i < GetNbItemsAttribut(); i++) {
			label = 0;
			value = 0;
			EbyedatGetParameters(i, &label, &value);
			if ((value != 0xFFFF) || (nozero == false)) {
				switch (mode) {
				case 'd':// decimal display
					sprintf(tempo, "|%5d = %5d", label, value);
					break;
				case 'o':// octal display
					sprintf(tempo, "|%5o = %5o", label, value);
					break;
				case 'h':// hexa display
					sprintf(tempo, "|%5x = %5x", label, value);
					break;
				case 'b': // binary display
					DecNumber = (int) value;
					Bin[0] = '\0';
					Bin[1] = zero;
					Bin2[1] = '\0';
					Bin2[0] = zero;
					j = 1;
					if (DecNumber != 0) {
						while (DecNumber >= 1) {
							reste = DecNumber % 2;
							DecNumber -= reste;
							DecNumber /= 2;
							if (reste)
								Bin[j] = one;
							else
								Bin[j] = zero;
							j++;
						}
						j--;
						maxbin = j;
						while (j >= 0) {
							Bin2[j] = Bin[maxbin - j];
							j--;
						}
					}
					sprintf(tempo, "|%5d = %16s", label, Bin2);
					break;
				}
				ss << tempo;
				presentation++;
			}
			if (presentation == max_presentation) {
				ss << "|\n";
				presentation = 0;
			}
		}
		if (presentation != 0)
			ss << "|\n";
	}
	display = ss.str();
	return display;
}
//_______________________________________________________________________________
void MFMEbyedatFrame::InitStat() {
	int i=0;
	int max= 65536;
	if (fLabelIndice ==NULL) fLabelIndice = new int[max];
	if (fIndiceLabel ==NULL) fIndiceLabel = new int[max];
	if (fNbLabels ==NULL)    fNbLabels    = new int[max];
	for (i=0;i<max;i++){
		fNbLabels[i]=0;
		fLabelIndice[i]=0;
		fIndiceLabel[i]=0;
	}
	MFMCommonFrame::InitStat();
	fNbPara = 0;
}
//____________________________________________________________________
void MFMEbyedatFrame::FillStat() {
	MFMCommonFrame::FillStat();
	int NbItems = GetNbItems();
	int i, indice;
	uint16_t label, value;
	for (i = 0; i < NbItems; i++) {
		EbyedatGetParameters(i, &label, &value);
		if (fLabelIndice[label] ==0) {
			fLabelIndice[label]=fNbPara;
			fIndiceLabel[fNbPara]=label;
		//	cout << GetCountFrame()<<" l= "<<label<<" nb para "<< fNbPara << endl;
			fNbPara++;
		}
		indice = fLabelIndice[label];
		fNbLabels[indice]++;
	}
}
//____________________________________________________________________
string MFMEbyedatFrame::GetStat(string info) {
	string display("");
	stringstream ss;
	ss <<"bonjour\n";

	ss << MFMCommonFrame::GetStat(info);
	display = ss.str();
	return display;
	ss << "Number of " << info << " Frames : " << fCountFrame << endl;
	if (fCountFrame != 0)
		ss << "Mean Frame Size " << (float) (fMeanFrameSize / fCountFrame)
				<< endl;
	uint16_t i ;
	for (i = 0;  i<  fNbPara;i++){
		ss << "Indice : "<< i << "   Label :"<< fIndiceLabel[i] <<"  Nb : "<< fNbLabels[i]<<endl;
	}
	display = ss.str();
	return display;
}
//____________________________________________________________________
void MFMEbyedatFrame::PrintStat(string info) {

	cout << (GetStat(info));
	if (fLabelIndice ){delete (fLabelIndice);fLabelIndice=NULL;};
	if (fIndiceLabel ){delete (fIndiceLabel);fIndiceLabel=NULL;};
	if (fNbLabels ){delete (fNbLabels);fNbLabels=NULL;};
}
//_______________________________________________________________________________


