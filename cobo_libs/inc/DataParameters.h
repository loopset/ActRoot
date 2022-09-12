
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
#include <vector>
#include <iostream>
#include "DataPar.h"
#include "MError.h"
#include <string.h>
#include <stdint.h>
using namespace std;
#ifndef DataParameters_H
#define DataParameters_H
#define MAX_CAR 255
//______________________________________________________________________________
class DataParameters  {

public:
	
	DataParameters(void);
	virtual ~DataParameters(void);

	virtual int Fill(const char *buffParam);
	virtual int FillFromActionFile(const char *actionfile);
    virtual bool IsAActionFile(const char *actionfile);

	virtual int GetLabel(const string parName) ;
	virtual int GetLabel(const int index) ;

	virtual int GetUserLabelFromIndex(const int index) ;
	virtual int GetUserLabel(const string parName) ;
	virtual int GetUserLabelFromLabel(const int label) ;
	virtual void SetUserLabelFromIndex(const int index,int userlabel) ;

	virtual int GetIndex(const int label) ;
	virtual int GetIndex(const string parName) ;
	virtual int GetMaxLabel();
	virtual int GetNbits(const string parName) ;
	virtual int GetNbitsFromLabel(const int label) ;
	virtual int GetNbitsFromIndex(const int label) ;

	virtual int GetDepth(const string parName) ;
	virtual int GetDepthFromLabel(const int label) ;
	virtual int GetDepthFromIndex(const int index);

	virtual const char* GetParNameFromIndex(const int index) ;
	virtual const char* GetParNameFromLabel(const int label) ;

	virtual void TestParameters();
	virtual void DumpListPara();
	virtual DataPar* FindObject(const string name);
	virtual int GetNbParameters(void) const {
		return fNb_Parameters;
	}
	virtual void DumpParameterName ();
	virtual int* creatLabelToExist(int* maxlabel);
private:
	void creatLabelToIndexVectors();
	char* CopyParam(char *Dest, char *Source) ;
	
	
private:

	vector<DataPar> fList; //! Parameters List, maybe a better structure could be made
	int fClassStatus; // Status error
	int fNb_Parameters; // nb of Parameter
	uint32_t* fLabelToIndex; //! vector for fast convertion of Label  to order index
	uint32_t* fLabelToUserLabel; //! vector for fast convertion of Label  to order index




public:
	MError fError;

};

#endif
