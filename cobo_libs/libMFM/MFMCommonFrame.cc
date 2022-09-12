/*
 MFMCommonFrame.cc

 Copyright Acquisition group, GANIL Caen, France

 */

#include <iostream>
#include <cmath>
#include <string.h>
#include <iostream>
#include <stdio.h>
#include <sstream>
#include <cstdlib>
#include <sys/time.h>
using namespace std;
#include "MFMCommonFrame.h"

//_______________________________________________________________________________
MFMCommonFrame::MFMCommonFrame(int unitBlock_size, int dataSource,
		int frameType, int revision, int frameSize) {
	/// Constructor of frame with a memory space\n
	/// fill header information : unitBlock_size,dataSource,....
	Init();
	int minsize = MFM_BLOB_HEADER_SIZE;
	if (minsize > frameSize) {
		cout << "Error of frame size (" << frameSize << ") > size of header ("
				<< minsize << ")\n";
		throw(MFM_BLOCK_SIZE);
	}
	SetBufferSize(frameSize);
	MFM_make_header(unitBlock_size, dataSource, frameType, revision, frameSize);
	fFrameSize = frameSize;
}

//_______________________________________________________________________________
MFMCommonFrame::MFMCommonFrame() {
	/// Constructor of a empty frame object
	Init();
}
//_______________________________________________________________________________
MFMCommonFrame::~MFMCommonFrame() {
	/// Destructor
	if (pDataNew) {
		free(pDataNew);
		pDataNew = NULL;
		pData = NULL;
		pData_char = NULL;
		pUserData_char= NULL;
	}
}
//_______________________________________________________________________________

void MFMCommonFrame::Init() {
	///
	/// Initialization of MFMCommonFrame object\n
	///
	fFrameSize = 0;
	fTimeDiff = 0;
	SetTimeDiffUs();
	pData = NULL;
	pDataNew = NULL;
	pHeader = NULL;
	fHeaderSize = MFM_BLOB_HEADER_SIZE;
	pReserveHeader = NULL;
	pData_char = NULL;
	fIncrement = 0;
	fBufferSize = 0;
	fSizeOfUnitBlock = 0;
	fLocalIsBigEndian = (Endianness() == MFM_BIG_ENDIAN);
	fFrameIsBigEndian = false;
}

//_______________________________________________________________________________
void MFMCommonFrame::DumpRaw(int dumpsize, int increment) {
	///
	/// Display dump of frame realised by GetDumpRaw(...\n
	/// dumpsize  : size of dump\n
	/// increment : begin of dump
	///
	cout << (GetDumpRaw(dumpsize, increment)).data();
	return;
}

//_______________________________________________________________________________
void MFMCommonFrame::SetHeader(MFM_topcommon_header* header) {
	/// Set pointer of on header of frame\n
	pHeader = header;
}

//_______________________________________________________________________________

string MFMCommonFrame::GetDumpRaw(int dumpsize, int increment) {
	///
	/// Creat a string of  dump of frame\n
	/// dumpsize  : size of dump if dumpsize =0 , dumpsize = standard = 256\n
	/// increment : begin of dump
	///

	string mydump;
	int framesize = GetFrameSize();
	if (dumpsize == 0)
		dumpsize = GetFrameSizeAttribut();

	if ((increment > 0) && (increment > dumpsize))
		fIncrement = increment;

	if (increment < 0) {
		increment = 0;
		fIncrement = 0;
	}

	if ((increment > 0) && (increment <= framesize)) {
		if (dumpsize + fIncrement > framesize)
			dumpsize = framesize - fIncrement;

		if (dumpsize == 0) {
			dumpsize = 256;
			fIncrement = 0;
		}
	}

	GetDumpRaw((void*) pData, dumpsize, fIncrement, &mydump);

	//} else {
	//mydump += "\n\t end of bloc \n";
	//}
	return mydump;
}

//_______________________________________________________________________________

void MFMCommonFrame::GetDumpRaw(void *point, int dumpsize, int increment,
		string * mydump) {

	///  Creat a string of dump of memory space\n
	///  point : pointer bo dump begin\n
	///  if dumpsize =0 , dumpsize = standard = 256 and fGBbuf_increment raz\n
	///  if mydump==NULL dump is displayed else dump is retuen in  string * mydump

	string *mydumploc;

	if (mydump == NULL) {
		string st;
		mydumploc = &st;
	} else {
		mydumploc = mydump;
	}

	int i, k;
	int nbrcol = 16; // nb de colonnes affich�es

	int asciimin = 32; // range min of a char to be ascii character
	char tempo[128] = "";
	char chartmp = 0;

	int asciimax = 127; // idem but max
	int nbrperline = 16; // nbr of bytes per line
	int nbrline = 0; // nbr lines
	int dumpsize2 = dumpsize;
	int dumpsize3 = dumpsize;
	unsigned char *pChar = NULL;
	unsigned char *pChar2 = NULL;

	nbrline = (int) (dumpsize / nbrperline) + (int) ((dumpsize % nbrperline)
			!= 0);

	pChar = (unsigned char *) ((char*) point + fIncrement);
	pChar2 = pChar;

	if (nbrline < 1)
		nbrline = 1;
	if (nbrcol > dumpsize)
		nbrcol = dumpsize;
	if (increment == dumpsize)
		nbrline = 1;
	for (i = 0; i < nbrline; i++) {
		sprintf(tempo, "\n%5d %s ", increment, ": ");
		*mydumploc += tempo;
		for (k = 0; k < nbrcol; k++) {
			sprintf(tempo, "%02hX ", (unsigned short)((*(pChar2++))));
			*mydumploc += tempo;
			dumpsize3--;
			if (dumpsize3 == 0)
				break;
		}
		*mydumploc += "  ";

		for (k = 0; k < nbrcol; k++) {
			chartmp = pChar[0];
			//memcpy (&chartmp, pChar ,1);
			if ((chartmp >= asciimin) && (chartmp <= asciimax)) {
				sprintf(tempo, "%c", *pChar);
				*mydumploc += tempo;
			} else
				*mydumploc += ".";
			pChar++;
			dumpsize2--;
			if (dumpsize2 == 0)
				break;
		}
		increment += nbrperline;
	}
	*mydumploc += "\n";

	if (mydump == NULL)
		cout << (*mydumploc).data();
}

//_______________________________________________________________________________

bool MFMCommonFrame::isBigEndian() {
	/// return attribut containing bigendian information
	return fLocalIsBigEndian;
}

//_______________________________________________________________________________

int MFMCommonFrame::GetMetaType() {
	/// return MetaType byte of frame
	return (uint32_t) pHeader->hd.metaType;
}
//_______________________________________________________________________________
int MFMCommonFrame::GetUnitBlockSize() {
	/// Compute and return Unit BlockSize of frame
	int tmp = pHeader->hd.metaType & MFM_UNIT_BLOCK_SIZE_MSK;
	fSizeOfUnitBlock = pow((double) 2, tmp);
	return (fSizeOfUnitBlock);
}

//_______________________________________________________________________________
int MFMCommonFrame::GetDataSource() {
	/// Return Data Source information of Frame
	return (uint32_t) pHeader->hd.dataSource;
}
//_______________________________________________________________________________
uint16_t MFMCommonFrame::GetFrameTypeAttribut() {
	/// Return attribut containing frame type
	return fFrameType;
}
//_______________________________________________________________________________
uint16_t MFMCommonFrame::GetFrameType() {
	/// Compute, set attribut and return frame type\n
	/// the indianess conversion is realised if necessary

	uint16_t tmp;
	tmp = pHeader->hd.frameType;

	if (fLocalIsBigEndian != fFrameIsBigEndian)
		SwapInt16(&tmp);
	fFrameType = tmp;
	return fFrameType;
}
//_______________________________________________________________________________
int MFMCommonFrame::GetRevision() {
	/// Return revision frame information
	return ((uint32_t) pHeader->hd.revision);
}
//_______________________________________________________________________________
int MFMCommonFrame::GetBlobNess() {
	/// Return BlobNess frame information
	return (uint32_t) ((pHeader->hd.metaType & MFM_BLOBNESS_MSK) >> 6);
}
//_______________________________________________________________________________
int MFMCommonFrame::GetFrameSize() {
	/// Compute, set attribut and return frame size\n
	/// the indianess conversion is realised if necessary

	uint32_t tmp;
	tmp = pHeader->hd.frameSize;
	if (fLocalIsBigEndian != fFrameIsBigEndian)
		SwapInt32(&tmp, 3);
	if (fSizeOfUnitBlock == 0)
		fSizeOfUnitBlock = GetUnitBlockSize();
	tmp = tmp * (uint32_t) fSizeOfUnitBlock;
	fFrameSize = tmp;
	return (tmp);
}
//_______________________________________________________________________________

void MFMCommonFrame::SetMetaType(int unitBlock_size, bool isablob) {
	/// Set MetaType byte of frame\n
	/// realize all coding computation : unitBlock_size , blog and endianness
	unsigned char endianness;
	unsigned char blobness;
	fSizeOfUnitBlock = unitBlock_size;
	double j = log10(unitBlock_size) / log10(2);
	double l = pow((double) 2, (int) j);

	if (fLocalIsBigEndian)
		endianness = MFM_BIG_ENDIAN;
	else
		endianness = MFM_LITTLE_ENDIAN;

	fFrameIsBigEndian = fLocalIsBigEndian;

	if (isablob)
		blobness = MFM_BLOBNESS_MSK;
	else
		blobness = 0;
	if (l != unitBlock_size)
		throw MFM_ERR_UNIT_BLOCK_SIZE; // unitBlock_size is not a power**2

	uint32_t block_size_power_2 = (int) j;

	if (block_size_power_2 > 16)
		throw MFM_ERR_UNIT_BLOCK_SIZE; // unitBlock_size out of range

	pHeader->hd.metaType = (endianness | block_size_power_2 | blobness);

}
//_______________________________________________________________________________
void MFMCommonFrame::SetUnitBlockSize(int bsize) {
	/// Set UnitBlockSize inside metatype byte
	pHeader->hd.metaType = bsize & MFM_UNIT_BLOCK_SIZE_MSK;
}

//_______________________________________________________________________________
void MFMCommonFrame::SetDataSource(int source) {
	/// Set Data source of frame
	pHeader->hd.dataSource = source;
}

//_______________________________________________________________________________
void MFMCommonFrame::SetFrameType(int frametype) {
	/// Set type of frame
	fFrameType = frametype;
	pHeader->hd.frameType = frametype;
}

//_______________________________________________________________________________
void MFMCommonFrame::SetRevision(int revision) {
	/// Set revision of frame
	pHeader->hd.revision = revision;
}
//_______________________________________________________________________________
void MFMCommonFrame::SetFrameSize(int size) {
	/// Set Frame Size of frame
	uint32_t test = size & MFM_FRAME_SIZE_MASK;
	pHeader->hd.frameSize = test;
}
//______________________________________________________________________________
int MFMCommonFrame::TestType(char * pt) {
	/// Test and return type of frame\n
	/// if  pt == NULL the test is done on current frame (this*)\n
	/// if  pt != NULL the test id done with help of (this*) frame but on memory space begining in pt
	int type;
	char * local_data;
	if (pt == NULL)
		local_data = (char*) pData;
	else
		local_data = pt;
	SetPointers(local_data);
	GetFrameType();
	type = (int) GetFrameTypeAttribut();

	return type;
}

//_______________________________________________________________________________
void MFMCommonFrame::HeaderDisplay(char * infotext) {
	/// Display  Header information realised by GetHeaderDisplay()\n
	/// if infotext is not NULL replace the standart "MFM header" title
	cout << (GetHeaderDisplay(infotext));
}
//_______________________________________________________________________________
string MFMCommonFrame::GetHeaderDisplay(char* infotext) {
	/// Return a string containing infomation of MFM Header\n
	/// if infotext is not NULL replace the standart "MFM header" title
	string display("");
	bool blob;
	stringstream ss;
	if ((pHeader->hd.metaType & MFM_BLOBNESS_MSK) == 0)
		blob = false;
	else
		blob = true;
	int unitBlockSize = GetUnitBlockSize();
	GetFrameType();
	if (infotext == NULL)
		ss << "MFM header, Type :" << WhichFrame() << " ";
	if (infotext != NULL)
		ss << infotext;
	    ss << hex << endl
	    		<< "   MetaType = " << dec << GetMetaType() << hex << "(0x" << GetMetaType() << ")"
	    		<< "  Blob = " << blob
	    		<< "  unitBlockSize = " << dec<< unitBlockSize
	    		<< "  frameSize = " << dec << GetFrameSize() << hex << "(0x" << GetFrameSize() << ")"
	    		<< "  dataSource = "<< dec << GetDataSource() << hex << "(0x" << GetDataSource() << ")" << endl
	    		<< "   FrameType = " << dec << GetFrameTypeAttribut() << hex << "(0x" << GetFrameTypeAttribut() << ")"
	    		<< "  revision = "<< dec << GetRevision() << hex << "(0x" << GetRevision() << ")"
			    << " pointer = " << (int*) GetPointHeader() << "\n";
	display = ss.str();
	return display;
}//_______________________________________________________________________________
unsigned char MFMCommonFrame::GetFrameEndianness() {
	/// Return  BIG_ENDIAN or LITTLE_ENDIAN value of current computer for metaType format

	unsigned char tmp = pHeader->hd.metaType & MFM_ENDIANNESS_MSK;
	return (tmp);

}
//_______________________________________________________________________________
unsigned char MFMCommonFrame::Endianness(void) {
	/// Return  endianness value of current computer for metaType format
	unsigned char LsbFlag;
	union {
		uint16_t s;
		uint64_t l;
	} b;

	b.l = 0;
	b.s = 1;
	if (b.l == b.s)
		LsbFlag = MFM_LITTLE_ENDIAN; /* LSB (little endian) car short = long  ( case of my x86-64,)*/
	else
		LsbFlag = MFM_BIG_ENDIAN; /* MSB (big endian) car byte inverses */
	return LsbFlag;
}

//_______________________________________________________________________________

bool MFMCommonFrame::is_power_2(int i) {
	/// Test if i is a power of 2\n

	double j = log10(i) / log10(2);
	int k = (int) j;
	cout << i << " - " << i % 2 << " - " << j << " - " << k;

	double l = pow((double) 2, (int) j);
	if (l != i)
		return false;
	else
		return true;
}

//_______________________________________________________________________________

void MFMCommonFrame::MFM_fill_header(int unitBlock_size, int dataSource,
		int frameType, int revision, int frameSize, bool blob) {
	/// Fill a Header with a list of parameters:\n
	/// unitBlock_size, dataSource, frameType, revision, frameSize, blob\n
	if (pHeader) {
		SetMetaType(unitBlock_size, blob);
		SetFrameSize(frameSize);
		SetDataSource(dataSource);
		SetFrameType(frameType);
		SetRevision(revision);

	} else {
		cout << " Error of header null\n";
	}
}
//_______________________________________________________________________________
void MFMCommonFrame::MFM_make_header(int unitBlock_size, int dataSource,
		int frameType, int revision, int frameSize, bool blob) {
	/// Do memory allocation for frame and\n
	/// fill its Header with a list of parameters:\n
	/// unitBlock_size, dataSource, frameType, revision, frameSize, blob\n
	SetBufferSize(frameSize * unitBlock_size);
	if (!pHeader)
		cout << " Error of header null\n";
	MFM_fill_header(unitBlock_size, dataSource, frameType, revision, frameSize,
			blob);
	SetPointers();
}

//_______________________________________________________________________________
void MFMCommonFrame::SetBufferSize(int size, bool ifinferior) {
	/// Do memory allocation or a reallacation for frame\n
	/// if ifinferior==true the allocaton is forced to size event if the acutal size is bigger\n
	if (!ifinferior or (size > fBufferSize)) {

		pDataNew = (char*) (realloc((void*) pData, size));
		for (int i = fBufferSize; i < size; i++)
			((char*) pDataNew)[i] = 0;
		pData = pDataNew;
		SetPointers();
		pReserveHeader = NULL;
		fBufferSize = size;
	}
}
//_______________________________________________________________________________
void MFMCommonFrame::SetPointers(void * pt) {
	/// Initialize pointers of frame\n
	/// if pt==NULL initialization is with current value of main pointer of frame (pData)\n
	/// else initialization is done with pData = pt\n
	/// pData must be the reference;
	if (pt != NULL) {
		pData = pt;
	}
	pHeader = (MFM_topcommon_header*) pData;
	pData_char = (char*) pData;
	pUserData_char=pData_char+fHeaderSize;
}
//_______________________________________________________________________________
void MFMCommonFrame::SetAttributs(void * pt) {
	/// Initialize a set of attributs (frame size, endianess, type ...) and pointers of frame\n
	/// reading and computing data comming from header of frame\n
	/// if pt==NULL initialization is done with current value of main pointer of frame (pData)\n
	/// else initialization is done with pData = pt
	fLocalIsBigEndian = (Endianness() == MFM_BIG_ENDIAN);
	SetPointers(pt);
	fFrameIsBigEndian = (GetFrameEndianness() == MFM_BIG_ENDIAN);
	GetFrameType();
	GetUnitBlockSize();
	GetFrameSize();

}

//____________________________________________________________________

void MFMCommonFrame::SwapInt32(uint32_t *Buf, int nbByte, int repeat) {
	/// Swap a 32 bits(4 Bytes) integer to do endianess conversion\n
	///     Buf   : pointer on integer to convert\n
	///     nByte : number of bytes concerned by swaping (default =4)\n
	///     repeat: nb of repeat in case of a vector to convert. default =1\n
	unsigned char *tmp1, *tmp2;
	uint32_t tempo;
	tempo = 0;
	if (nbByte > 4)
		cout << "MFMCommonFrame::SwapInt32 with nb of Bytes > 4\n";

	uint32_t *Bufloc = Buf;
	for (int j = 0; j < repeat; j++) {
		Bufloc = Buf + j * nbByte;
		tmp1 = (unsigned char*) Bufloc;
		tmp2 = (unsigned char*) (&tempo);
		for (int i = 0; i < nbByte; i++) {
			tempo = ((uint32_t) (tempo) << 8);
			memcpy(tmp2, tmp1 + i, 1);
		}
		*Buf = tempo;
	}
}
//____________________________________________________________________

void MFMCommonFrame::SwapInt64(uint64_t *Buf, int nbByte, int repeat) {
	/// Swap a 64 bits(8 Bytes) integer to do endianess conversion \n
	///		Buf   : pointer on integer to convert\n
	///		nByte : number of bytes concerned by swaping (default = 8)\n
	///     repeat: nb of repeat in case of a vector to convert. default =1
	unsigned char *tmp1, *tmp2;
	uint64_t tempo;
	tempo = 0;
	tmp1 = (unsigned char*) Buf;
	tmp2 = (unsigned char*) (&tempo);
	uint64_t *Bufloc = Buf;
	if (nbByte > 8)
		cout << "MFMCommonFrame::SwapInt64 with nb of Bytes > 8\n";
	for (int j = 0; j < repeat; j++) {
		Bufloc = Buf + j * nbByte;
		tmp2 = (unsigned char*) (&tempo);
		tmp1 = (unsigned char*) Bufloc;
		for (int i = 0; i < nbByte; i++) {
			tempo = ((tempo) << 8);
			memcpy(tmp2, tmp1 + i, 1);
		}
		*Bufloc = tempo;
	}
}

//____________________________________________________________________

void MFMCommonFrame::SwapInt16(uint16_t *Buf, int repeat) {
	/// Swap a 16 bits(2 Bytes) integer to do endianess conversion
	///     Buf   : pointer on integer to convert
	///     repeat: nb of repeat in case of a vector to convert. default =1
	typedef struct mot16 {
		unsigned char Byte1;
		unsigned char Byte2;
	} SW_MOT16;
	SW_MOT16 Temp, *Mot16;

	Mot16 = (SW_MOT16 *) Buf;

	for (int i = 0; i < repeat; i++, Mot16++) {
		Temp.Byte1 = Mot16->Byte2;
		Temp.Byte2 = Mot16->Byte1;
		*Mot16 = Temp;
	}
}
//____________________________________________________________________
int MFMCommonFrame::ReadInFile(int *fLun, char** vector, int * vectorsize) {
	/// Get data from a file, and fill current frame and initialize its attributs and its pointer
	/// if size of actual frame is not enough, and new size is reallocated
	///      fLun   : descriptor of file (given by a previous open)
	///      vector : pointer on pointer will contain frame . if size isn't big, a new value of pointer
	///      vectorsize of this pointer
	///      return size of read frame.
	int count = 0;
	int framesize = 0;
	char* vectornew = NULL;
	int sizetoread = MFM_BLOB_HEADER_SIZE; // =8	count = read(fLun, (void*) vector, minsizeheader);
	count = read(*fLun, (void*) (*vector), sizetoread);
	if (count <= 0) {
// 		cout << " end of read file\n";
		return count;
	}
	if (count < sizetoread) {
		cout << " Error in read file\n";
		return count;
	}
	SetAttributs((*vector));
	framesize = GetFrameSize();
	if (*vectorsize < framesize) {
		vectornew = (char*) (realloc((void*) (*vector), framesize));
		if (vectornew != NULL) {
			(*vector) = vectornew;
			*vectorsize = framesize;
			SetAttributs((*vector));
		} else {
			cout
					<< " Error of memory allocation in MFMCommonFrame:: ReadInFile \n";
			return 0;
		}
	}
	sizetoread = framesize - sizetoread;
	count = read(*fLun, (void*) ((*vector) + MFM_BLOB_HEADER_SIZE), sizetoread);
	if (count != sizetoread) {
		cout << " Error in read file\n";
	}
	return framesize;
}
//____________________________________________________________________
int MFMCommonFrame::ReadInMem(char **pt) {
	/// Get data from a memory, and fill current frame and initialize its attributs and its pointer
	//  return size of read frame.
	int framesize = 0;
	SetAttributs((*pt));
	framesize = GetFrameSizeAttribut();
	(*pt) = (*pt) + framesize;
	return (framesize);
}
//____________________________________________________________________
uint64_t MFMCommonFrame::GetTimeStampUs(uint64_t diff) {
	/// give time in useconde from last SetTimeDiffUs() or since diff if diff >=0
	/// if diff =0 => since 1970
	/// GetTimeStampUs() is used  for simulation
	struct timeval tv;
	gettimeofday(&tv, NULL);
	uint64_t tstime;
	tstime = tv.tv_sec * (uint64_t) 1000000 + tv.tv_usec;
	if (diff == 0)
		tstime -= diff;
	else
		tstime -= fTimeDiff;
	return tstime;
}
//_______________________________________________________________________________
uint64_t MFMCommonFrame::GenerateATimeStamp() {
	///
	///generate a time stamp with computer clock for simulation
	///
	uint64_t ts= 0 ;
	clock_t t = clock();
	if (t<=0 )
		ts=(uint64_t)t;
	return ts;
}
//____________________________________________________________________
uint64_t MFMCommonFrame::SetTimeDiffUs() {
	/// give time in useconde from 1970
	/// SetTimeDiffUs() is used for simulation
	struct timeval tv;
	gettimeofday(&tv, NULL);
	fTimeDiff = (tv.tv_sec * (uint64_t) 1000000 + tv.tv_usec);
	return fTimeDiff;
}
//____________________________________________________________________
int MFMCommonFrame::GetHeaderSizeAttribut() {
	/// Return header size without computing it.
	return fHeaderSize;
}
//____________________________________________________________________
int MFMCommonFrame::GetBufferSizeAttribut() {
	/// Return buffer size without computing it.
	return fBufferSize;
}
//____________________________________________________________________
int MFMCommonFrame::GetFrameSizeAttribut() {
	/// Return frame size without computing it.
	return fFrameSize;
}
//____________________________________________________________________
void *MFMCommonFrame::GetPointHeader() {
	/// Return pointer of begining of frame.
	return pData;
}
//_______________________________________________________________________________
char* MFMCommonFrame::GetPointUserData() {
	/// Get pointer after header of frame\n
	return pUserData_char;
}
//____________________________________________________________________
int MFMCommonFrame::GetEventNumber() {
	/// Method supposed to be overloaded so return 0
	return 0;
}
//____________________________________________________________________
uint64_t MFMCommonFrame::GetTimeStamp() {
	/// Method supposed to be overloaded so return 0
	return ((uint64_t) 0);
}
//____________________________________________________________________
string MFMCommonFrame::WhichFrame(uint16_t type) {
	string tempos = "UNKNOWN_TYPE";
	uint16_t testype = 0;
	if (type == 0) {
		testype = fFrameType;
	}
	if (testype == MFM_COBO_FRAME_TYPE)
		tempos = "MFM_COBO_FRAME_TYPE";
	if (testype == MFM_COBOF_FRAME_TYPE)
		tempos = "MFM_COBOF_FRAME_TYPE";
	if (testype == MFM_EXO2_FRAME_TYPE)
		tempos = "MFM_EXO2_FRAME_TYPE";
	if (testype == MFM_OSCI_FRAME_TYPE)
		tempos = "MFM_OSCI_FRAME_TYPE";
	if (testype == MFM_RAWD_FRAME_TYPE)
		tempos = "MFM_RAWD_FRAME_TYPE";
	if (testype == MFM_EBY_EN_FRAME_TYPE)
		tempos = "MFM_EBY_EN_FRAME_TYPE";
	if (testype == MFM_EBY_TS_FRAME_TYPE)
		tempos = "MFM_EBY_TS_FRAME_TYPE";
	if (testype == MFM_EBY_EN_TS_FRAME_TYPE)
		tempos = "MFM_EBY_EN_TS_FRAME_TYPE";
	if (testype == MFM_SCALER_DATA_FRAME_TYPE)
		tempos = "MFM_SCALER_DATA_FRAME_TYPE";
	if (testype == MFM_MERGE_EN_FRAME_TYPE)
		tempos = "MFM_MERGE_EN_FRAME_TYPE";
	if (testype == MFM_XML_DATA_DESCRIPTION_FRAME_TYPE)
		tempos = "MFM_XML_DATA_DESCRIPTION_FRAME_TYPE";
	if (testype == MFM_RIBF_DATA_FRAME_TYPE)
		tempos = "MFM_RIBF_DATA_FRAME_TYPE";
	if (testype == MFM_MUTANT_FRAME_TYPE)
		tempos = "MFM_MUTANT_FRAME_TYPE";
	if (testype == MFM_XML_FILE_HEADER_FRAME_TYPE)
		tempos = "MFM_XML_FILE_HEADER_FRAME_TYPE";
	return tempos;
}
//____________________________________________________________________
bool MFMCommonFrame::IsAEbyedat(int type) {
	if (type ==0) type = fFrameType;
	if ((type == MFM_EBY_EN_FRAME_TYPE)||(type == MFM_EBY_TS_FRAME_TYPE)||(type == MFM_EBY_EN_TS_FRAME_TYPE)) {
		return true;
	}
	return false;
}
//____________________________________________________________________
bool MFMCommonFrame::IsAScaler(int type) {
	if (type ==0) type = fFrameType;
	if (type == MFM_SCALER_DATA_FRAME_TYPE) {
		return true;
	}
	return false;
}
//____________________________________________________________________
bool MFMCommonFrame::IsACobo(int type) {
	if (type ==0) type = fFrameType;
	if ((type == MFM_COBO_FRAME_TYPE) || (type== MFM_COBOF_FRAME_TYPE)) {
		return true;
	}
	return false;
}
//____________________________________________________________________
bool MFMCommonFrame::IsAMutant(int type) {
	if (type ==0) type = fFrameType;
	if ((type == MFM_MUTANT_FRAME_TYPE) || (type== MFM_MUTANT1_FRAME_TYPE)|| (type== MFM_MUTANT2_FRAME_TYPE)|| (type== MFM_MUTANT3_FRAME_TYPE)) {
		return true;
	}
	return false;
}
//_______________________________________________________________________________
void MFMCommonFrame::InitStat() {
	fCountFrame = 0;
}
//____________________________________________________________________
void MFMCommonFrame::FillStat() {
	fCountFrame++;
    int framesize =GetFrameSizeAttribut();
	fMeanFrameSize+=framesize;
}
//____________________________________________________________________
string  MFMCommonFrame::GetStat(string info){
	string display("");
	stringstream ss;
	ss <<"Number of " <<info<< " Frames : "<< fCountFrame << endl;
	if (fCountFrame!=0)
	ss <<"Mean Frame Size " <<(float)(fMeanFrameSize/fCountFrame)<< endl;
	display = ss.str();
	return display;
}
//____________________________________________________________________
void MFMCommonFrame::PrintStat(string info ){
	cout << (GetStat(info));
}
//_______________________________________________________________________________
int MFMCommonFrame::GetCountFrame(){
	return fCountFrame;
}
//_______________________________________________________________________________
