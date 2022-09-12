
//////////////////////////////////////////////////////////////////////////

//
// ---------------------------------------------------------------------------
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef DataScal_H
#define DataScal_H

#include <string.h>
#include <iostream>

using namespace std;

//______________________________________________________________________________
class DataScal {

public:
  DataScal (string name,uint32_t label,uint64_t count=0,uint64_t frequency=0,
		  	  int32_t status=0,uint64_t tics=0,int32_t acqstatus=0){

	fLabel=label;
	fCount= count;
	fFrequency= frequency;
	fStatus=status;
	fTics=tics;
	fAcqStatus=acqstatus;
	fName=name;
	}
	uint32_t Label(void) const {
		return (fLabel);
	}
	uint64_t Count(void) const {
			return (fCount);
		}
	uint64_t Frequency(void) const {
		return (fFrequency);
	}
	int32_t Status(void) const {
		return (fStatus);
	}
	uint64_t Tics(void) const {
		return (fTics);
	}
	int32_t AcqStatus(void) const {
		return (fAcqStatus);
	}
	string Name(void) const {
		return (fName);
	}
private:
	uint32_t fLabel;// Scaler Label
	uint64_t fCount;// Value of scaler
	uint64_t fFrequency;//Frequency
	int32_t  fStatus; // Status Channel
	uint64_t fTics;  //Time
	int32_t  fAcqStatus; // status acq
	int64_t  fBidon1 //// to fill 64 memory allignement
	int32_t  fBidon2 //// to fill 64 memory allignement
	string   fName; // name of parameter
};

#endif
