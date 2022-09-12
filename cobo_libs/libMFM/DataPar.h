
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

#ifndef DataPar_H
#define DataPar_H

#include <string.h>
#include <iostream>

using namespace std;

//______________________________________________________________________________
class DataPar {

public:
  DataPar (string name, const int label, int nbbits=16, int depth=1,int sizeinmemory=16)  {
	    fName = name;
		fLabel=label;
		fNbits=nbbits;
		fUserLabel=0;
		fDepth=depth;
		fSizeInMemory =sizeinmemory;
		fBidon1 =0;
		fBidon2 =0;
		fBidon3 =0;
	}
	int Label(void) const {
		return (fLabel);
	}
	
	void SetUserLabel(int userlabel)  {
		fUserLabel= userlabel;
	}

	int Nbits(void) const {
		return (fNbits);
	}
	;
	int UserLabel(void) const {
		// a label that user can use if 
			return (fUserLabel);
	}
	;
	int Depth(void) const {
		return (fDepth);
	}
	;
	string Name(void) const {
		return (fName);
	}
	;
private:
	int fLabel; // Parameter Label 
	int fNbits; // Parameter size  in bit number
	int fUserLabel; // User Label. User can use it and fill it if he wants ( can be usefull in case of muti detector)
	int fDepth; // Parameter depth
	int fSizeInMemory;// Size in bit number  of parameter taken in memory  > fNbits
	int fBidon1;// to fill 64 memory allignement
	int fBidon2;// to fill 64 memory allignement
	int fBidon3;// to fill 64 memory allignement
	string fName; // name of parameter

};

#endif
