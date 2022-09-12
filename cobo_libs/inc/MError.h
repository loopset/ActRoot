// File : MError.h
// Author: Luc Legeard
//////////////////////////////////////////////////////////////////////////////
//
// Class MError
//
// This class manager Errors  
//
/////////////////////////////////////////////////////////////////////////////

// ---------------------------------------------------------------------------
// ***************************************************************************
// *                                                                         *
// *   This program is free software; you can redistribute it and/or modify  *
// *   it under the terms of the GNU General Public License as published by  *
// *   the Free Software Foundation; either version 2 of the License, or     *
// *   (at your option) any later version.                                   *
// *                                                                         *
// ***************************************************************************/


#ifndef __MError__
#define __MError__

#include <sstream>
#include <iostream>
#include <ostream>

#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include <sys/types.h>
#include <string>

#define COLOR_STD    "\033[0m"

#define BLACK_STD    "\033[0;30m"
#define RED_STD      "\033[0;31m"
#define GREEN_STD    "\033[0;32m"
#define YELLOW_STD   "\033[0;33m"
#define BLUE_STD     "\033[0;34m"
#define PURPLE_STD   "\033[0;35m"
#define CYAN_STD     "\033[0;36m"
#define GREY_STD     "\033[0;37m"

#define BLACK_BOLD    "\033[1;30m"
#define RED_BOLD      "\033[1;31m"
#define GREEN_BOLD    "\033[1;32m"
#define YELLOW_BOLD   "\033[1;33m"
#define BLUE_BOLD     "\033[1;34m"
#define PURPLE_BOLD   "\033[1;35m"
#define CYAN_BOLD     "\033[1;36m"
#define GREY_BOLD     "\033[1;37m"

using namespace std;
//_________________________________________________________________________________________


class MError{
 private:
  int fVerbose;
  int fNbError; // number of critical errors
  int fNbCriticalError; // number of critical errors
  int fNbCriticalErrorMax; // number of critical errors
  int fNbWarning; // number of warning
  int fNbInfo; // number of infon
  int fNbDebug; // number of debug
  int fBidon;// pour alignement

 public :
  MError();   // default constructor of MError object
  ~MError();   // destructor of MError
  virtual void Infos( const char *message,const char *message2=NULL, const char* comment=NULL); 
  virtual void Infos( char *message, string *message2=NULL, string * comment=NULL);
  virtual void Infos( string *message,  string *message2=NULL,string * comment=NULL);
  virtual void Barre();
  virtual void TreatError(int level,int status , const char *message, const char *message2=NULL, const char* comment =NULL);
  virtual void TreatError(int level,int status , string *message,  string    *message2=NULL, string *    comment =NULL);
  virtual void TreatError(int level,int status , char *message,     string    *message2=NULL, string *    comment =NULL);
  
  virtual void TreatDebug(int level,int status, const char *message, const char *message2=NULL, const char* comment =NULL);
  virtual void TreatDebug(int level,int status, string *message,    string    *message2=NULL, string *    comment =NULL);
  virtual void TreatDebug(int level,int status, char *message, string    *message2=NULL, string *    comment =NULL);
  
  virtual void SetDebugVerbose (int level);
  
  virtual void SetVerbose(int  i=0){fVerbose =i;};
};

#endif
