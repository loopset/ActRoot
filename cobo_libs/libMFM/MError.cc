// File : MError.C
// Author: Luc Legeard
//////////////////////////////////////////////////////////////////////////////
//
// Class MError
//
// Manager Errors
//
////////////////////////////////////////////////////////////////////////////

// ---------------------------------------------------------------------------
// ***************************************************************************
// *                                                                         *
// *   This program is free software; you can redistribute it and/or modify  *
// *   it under the terms of the GNU General Public License as published by  *
// *   the Free Software Foundation; either version 2 of the License, or     *
// *   (at your option) any later version.                                   *
// *                                                                         *
// ***************************************************************************/

// In this Class
// somme time , fonction "cout" doen't work so we have to use printf
// to solve this probleme.

#include "MError.h"
#include <stdlib.h>

//______________________________________________________________________________


MError::MError(void) {
	fNbInfo = 0; // number of  info
	fNbError = 0; // number of  errors
	fNbCriticalError = 0; // number of critical errors
	fNbCriticalErrorMax = 20; // number of errors
	fVerbose = 0; // Level of verbose (0-10)
	fNbWarning= 0; // nb of warning
	fBidon =0;
	fNbDebug = fBidon; // nb of debug
}

//_____________________________________________________________________________

MError::~MError() {
	// destructor of MError object
}
//_____________________________________________________________________________
void MError::Infos(const char *message, const char *message2,
		const char* comment) {
	// call :TreatError(int level,int status , char *message,char* comment )

	TreatError(0, 0, message, message2, comment);
}
//_____________________________________________________________________________
void MError::Barre() {
	// print bar
	cout<<"-----------------------------------------------------------------------------------\n";
}
//_____________________________________________________________________________
void MError::Infos(char *message, string *message2, string* comment) {
	// call :TreatError(int level,int status , char *message,char* comment )
	TreatError(0, 0, message, message2, comment);
}
//_____________________________________________________________________________
void MError::Infos(string *message, string *message2, string* comment) {
	// call :TreatError(int level,int status , char *message,char* comment )
	TreatError(0, 0, message, message2, comment);
}
//_____________________________________________________________________________
void MError::TreatError(int level, int status, string *message,
		string *message2, string* comment) {
	// call :TreatError(int level,int status , char *message,char* comment )
	TreatError(level, status, message, message2,
			comment);
}

//_____________________________________________________________________________

void MError::TreatError(int level, int status, char *message,
		string *message2, string* comment) {
	// call :TreatError(int level,int status , string *message,string* comment )
	TreatError(level, status, message, message2, comment);
}
//_____________________________________________________________________________

void MError::SetDebugVerbose(int level) {
	// Set Level of verbose  0<=level<=10
	if (level<0 || level> 10)cout<<"---> Warning , strange debug level :"<< level <<"\n";
	else
	fVerbose = level;
}
//_____________________________________________________________________________

void MError::TreatError(int level, int status, const char *message,
		const char *message2, const char* comment) {
	// Level = 0 info
	//         1 warnig
	//         2 Error
	//         3 Critical error ( exit if nb error is toot hig)
	//         4 Fatal error ( exit)
	string WorE;
	string tempo;
	WorE="";


	if ((level<0 || level> 5))printf("--> Warning , strange error level : %d\n",level);
/*
	if ( level ==0) {
		WorE = "--> Info   : ";
		fNbInfo ++;
	}

	if ( level ==1) {
		WorE = "--> Warning : ";
		fNbWarning ++;
	}
	if ( level ==2) {
		WorE = "--> Error  : ";
		fNbError++;
	}
	if ( level ==3) {
		WorE = "--> Critical Error : ";
		fNbCriticalError++;
	}
	if ( level ==4)
	{
		WorE = "--> Fatal Error : ";
	}
	*/
	if ( level ==0) {
		WorE  = BLUE_BOLD ;
		WorE += "--> Info   : ";
		WorE += COLOR_STD;// restaure standart color
		fNbInfo ++;
	}

	if ( level ==1) {
		WorE  = YELLOW_STD;// change color in orange
		WorE += "--> Warning: ";
		WorE += COLOR_STD ;
		fNbWarning ++;
	}
	if ( level ==2) {
		WorE =  RED_BOLD;
		WorE += "--> Error  : ";
		WorE += COLOR_STD;// change color in red
		fNbError++;
	}
	if ( level ==3) {
		WorE = RED_BOLD;
		WorE += "--> Critical Error : ";
		WorE += COLOR_STD;// change color in red
		fNbCriticalError++;
	}
	if ( level ==4)
	{
		WorE  =  RED_BOLD;
		WorE += "--> Fatal Error : ";
		WorE += COLOR_STD;
	}


	cout<<WorE;
	if ((level !=0 )&& ( status!=0)) printf("%x ",status);

	printf("%s",message);
	if (message2!=NULL) printf("  %s", message2);
	printf( "\n");
	if (comment!=NULL) {
		if (fVerbose) {printf("%s\n", comment);
		}
	}

	if ( fNbCriticalError> fNbCriticalErrorMax) {
		printf(" Maximun of critical error reached,  exit\n");
		fflush (stdout);
		exit(0);
	}
	fflush (stdout);
	if ( level ==4 ) exit(0);

}

//_____________________________________________________________________________
void MError::TreatDebug(int level, int status, string *message,
		string *message2, string* comment) {
	// call :TreatDebug(int level,int status , string *message,string* comment )
	TreatDebug(level, status, message, message2,
			comment);
}

//_____________________________________________________________________________

void MError::TreatDebug(int level, int status, char *message,
		string *message2, string *comment) {
	// call :TreatDebug(int level,int status , char *message,char* comment )
	TreatDebug(level, status, message, message2, comment);
}
//_____________________________________________________________________________
void MError::TreatDebug(int level, int status, const char *message,
		const char *message2, const char* comment) {
	//         1 ->10

	string WorE;
	WorE = "--> Debug : ";
	string tempo="--> Warning, Strange debug level : ";

	if ( (level<0 || level> 10)) cout << tempo << level<< endl  ;

	if ( level >9 && level <21)
	{
		WorE = "--> Debug : ";
	}

	if ( (fVerbose >= level)) {
		cout <<  WorE <<  message;
		if (message2!=NULL) {
			cout <<"--->          "<<message2<<endl;
		}
		if (fVerbose)
		{
			if(comment!=NULL) {
				cout <<comment << endl;
			}
		}
	}
	fflush (stdout);
}
//_____________________________________________________________________________
