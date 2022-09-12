
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
#include "DataScal.h"
#include "MError.h"
#include <string.h>
#include <stdint.h>
#include <tinyxml.h>
using namespace std;
#ifndef DataScaler_H
#define DataScaler_H
#define MAX_CAR 255
//______________________________________________________________________________
class DataScaler  {



public:
	
	DataScaler(void);
	virtual ~DataScaler(void);

	virtual int FillFromMfmFrame(const char *buffParam);
	virtual int FillFromXmlFile(const char *actionfile);
    virtual bool IsAActionFile(const char *actionfile);

	virtual int GetLabel(const string parName) ;
	virtual int GetLabel(const int index) ;
;
	virtual int GetUserLabelFromLabel(const int label) ;
	virtual void SetUserLabelFromIndex(const int index,int userlabel) ;

	virtual int GetIndex(const int label) ;
	virtual int GetIndex(const string parName) ;
	virtual int GetMaxLabel();
	virtual int GetNbits(const string parName) ;
	virtual int GetNbitsFromLabel(const int label) ;
	virtual int GetNbitsFromIndex(const int label) ;

	virtual const char* GetParNameFromIndex(const int index) ;
	virtual const char* GetParNameFromLabel(const int label) ;

	virtual void TestScalers();
	virtual void DumpListPara();
	virtual DataScal* FindObject(const string name);
	virtual int GetNbParameters(void) const {
		return fNb_Parameters;
	}
	virtual void DumpScalerName ();
	virtual int* creatLabelToExist(int* maxlabel);
	virtual TiXmlDocument * OpenXmlFile(const char* pFilename);





private:
	void creatLabelToIndexVectors();
	char* CopyScaler(char *Dest, char *Source) ;
	
	
private:

	vector<DataPar> fList; //! Scaler List, maybe a better structure could be made
	int fClassStatus; // Status error
	int fNb_Scaler; // nb of Scaler
	uint32_t* fLabelToIndex; //! vector for fast convertion of Label  to order index
	uint32_t* fLabelToUserLabel; //! vector for fast convertion of Label  to order index

	TiXmlDocument* fTiXmlDocument ;/// xlm objet to manage xml file

public:
	MError fError;
};

#endif
