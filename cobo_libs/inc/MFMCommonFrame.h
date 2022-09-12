///
/// MFM library manage event MFM format
///
/// MFMCommonFrame Class
/// \file MFMCommonFrame.h
/// \author Legeard Luc
/// \version 1.0
/// \date 2014 01 01
///
/// this  file
///
/*! \mainpage  MFM library
 *
 *
 * \Introduction
 *
 * This MFM package  manage The MultiFrame Metaformat \n
 * allowing read and write MFM frame
 *
 * \section install_sec Installation
 *
 * make clean \n
 * make
 *
 * \section usage_sec Usage
 *
 * A MFMtest.exe executable is generated and contains examples to generate different MFM frames \n
 * The execution of "MFMtest.exe" gives a help How to use MFMtest.exe \n
 * Have a look to main file MFMtest.cc to see how to use MFM library and its different classes \n
 *
 */
#ifndef _MFM_FRAME_COMMON_
#define _MFM_FRAME_COMMON_

#include <stdint.h>
#include "MFMTypes.h"
#include "MError.h"
#include "stdlib.h"
#pragma pack(push, 1)// allow strict alignment
#define MFM_ENDIANNESS_MSK 0x80
#define MFM_BLOBNESS_MSK 0x40
#define MFM_UNIT_BLOCK_SIZE_MSK 0x0F
#define MFM_BLOCK_SIZE 0x0C
#define MFM_BIG_ENDIAN 0
#define MFM_LITTLE_ENDIAN 0x80
#define MFM_FRAME_SIZE_MASK 0x00FFFFFF
#define MFM_BLOB_HEADER_SIZE 8

#ifdef CC_IN2P3
#include "XrdPosix.hh"
#endif


enum MFM_ERR { MFM_ERR_UNIT_BLOCK_SIZE = -100, MFM_ERR_FRAME_SIZE };


struct MFM_common_header {
  unsigned metaType : 8;
  unsigned frameSize : 24;
  unsigned dataSource : 8;
  unsigned frameType : 16;
  unsigned revision : 8;
};

struct MFM_ext_layered_header {
  unsigned headerSize : 16;
  unsigned itemSize : 16;
  unsigned nItems : 32;
};

struct MFM_ext_basic_header {
  unsigned headerSize : 16;
  unsigned itemSize : 16;
  unsigned nItems : 32;
};

struct MFM_ext_blob_header {
};
struct MFM_ext_common_header {
};

struct MFM_layered_header {
  MFM_common_header hd;
  MFM_ext_layered_header ext;
};

struct MFM_topcommon_header {
  MFM_common_header hd;
  MFM_ext_common_header ext;
};
struct MFM_basic_header {
  MFM_common_header hd;
  MFM_ext_basic_header ext;
};

struct MFM_blob_header {
  MFM_common_header hd;
  MFM_ext_layered_header ext;
};

//____________MFMCommonFrame___________________________________________________________

class MFMCommonFrame
{
public:

MFM_topcommon_header * pHeader; ///<  Pointer on frame header
uint32_t fHeaderSize;   		///< Header size in Bytes
void * pReserveHeader; 			///< Pointer on frame reserve header
void * pData;     				///< Pointer of beginning frame
char * pDataNew;  				///< Pointer of beginning frame  if local allocation.
char * pData_char; 				///< Pointer of beginning frame
char * pUserData_char; 		    ///< Pointer of User Data ( after header and timestamp,enventnumber..)
int    fSizeOfUnitBlock; 		///< Size of Unit block in Bytes
bool   fLocalIsBigEndian; 		///< Endianness or running computer
bool   fFrameIsBigEndian ; 		///< Endianness or Frame
uint16_t fFrameType; 			///< Frame type
int    fFrameSize; 				///< Frame size
private :
int    fIncrement;    			///< memory of incrementation in case of dump.
int    fBufferSize;   			///< available buffer size, it can be higher than frame size.
uint64_t	fTimeDiff;  		///< memorise time reference in microsecond
public :
char *   pCurrentPointerForAdd; ///< Pointer  to keep memory where we can add data or other frames in current frame
char *   pCurrentPointerForRead; ///<Pointer  to keep memory where we can read data or other frames in current frame
int      fCountFrame           ; /// counter to do statistic ( in case of same frame object is reused to read a file for example)
int      fMeanFrameSize        ; /// statistic : MeanFrameSize
MError   fError;
public :
MFMCommonFrame();
MFMCommonFrame(int unitBlock_size, int dataSource,
	 		 int frameType, int revision, int frameSize);
virtual ~MFMCommonFrame();
virtual	void Init();
virtual	string WhichFrame(uint16_t type =0);
virtual	void SetMetaType(int unitBlock_size,bool isablob=false);
virtual void SetUnitBlockSize(int size);
virtual void SetDataSource(int datasource);
virtual void SetRevision(int revision);
virtual void SetFrameSize(int size);
virtual void SetFrameType(int frametype);
virtual void SetHeader(MFM_topcommon_header* header);
virtual void SetBufferSize(int size,bool ifinferior=true);
virtual void SetPointers(void * pt =NULL);
virtual void SetAttributs(void * pt =NULL);
virtual unsigned char Endianness(void);
virtual bool isBigEndian();
virtual bool is_power_2(int i);
virtual	int	 GetBlobNess();
virtual	int	 GetMetaType();
virtual	int	 GetUnitBlockSize();
virtual unsigned char GetFrameEndianness(void);
virtual	int  GetDataSource();
virtual uint16_t  GetFrameTypeAttribut();
virtual uint16_t  GetFrameType();
virtual	int  GetRevision();
virtual int  TestType(char * pt=NULL);
virtual	int  GetFrameSize();
virtual	int  GetHeaderSizeAttribut();
virtual	int  GetBufferSizeAttribut();
virtual	int  GetFrameSizeAttribut();
virtual	void *GetPointHeader();
virtual char* GetPointUserData();
virtual	int  GetEventNumber();
virtual uint64_t GetTimeStamp();
virtual void DumpRaw(int dumpsize=0, int increment=256);
virtual string GetDumpRaw(int dumpsize=0, int increment=256);
virtual void   GetDumpRaw(void *point,int dumpsize, int increment,string * mydump = NULL);
virtual string GetHeaderDisplay(char* infotext=NULL);
virtual void   HeaderDisplay(char* infotext=NULL) ;
virtual void   MFM_make_header(int unitBlock_size, int dataSource,
			    int frameType, int revision, int frameSize,bool blob=false);
virtual void   MFM_fill_header( int unitBlock_size,
				   int dataSource, int frameType, int revision,
				   int frameSize,bool blob=false);

virtual void SwapInt32(uint32_t *Buf, int nbByte = 4, int repeat = 1);
virtual void SwapInt64(uint64_t *Buf, int nbByte = 8, int repeat = 1);
virtual void SwapInt16(uint16_t *Buf, int repeat =1);
virtual int  ReadInFile(int *fLun,char** vector, int * vectosize);
virtual int  ReadInMem(char** vector);
virtual uint64_t GetTimeStampUs(uint64_t diff= 0);
virtual uint64_t SetTimeDiffUs();
virtual uint64_t GenerateATimeStamp();
virtual bool IsAEbyedat(int type =0);
virtual bool IsACobo(int type =0);
virtual bool IsAMutant(int type =0);
virtual bool IsAScaler(int type=0);
virtual void FillStat();
virtual void InitStat();
virtual string GetStat(string info = "UnknowFrame" );
virtual void PrintStat(string ="UnknowFrame" );
virtual int GetCountFrame();
};

#pragma pack(pop) // free aligment
#endif
