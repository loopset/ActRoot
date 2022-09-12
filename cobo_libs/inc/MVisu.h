///////////////////////////////////////
//                                   //
// T. Roger - GANIL 2015             //
// email: roger@ganil.fr             //
//                                   //
// MVisu class definition            //
//                                   //
///////////////////////////////////////

#ifndef MVISU_H
#define MVISU_H

#include <stdio.h>
#include <iostream>
#include <cstdlib>

#include <TApplication.h>
#include <TCanvas.h>
#include <TList.h>
#include <TObjArray.h>
#include <TH2S.h>

#include <MEvent.h>


using namespace std;



class MVisu
{
	public:
	MVisu(int,char **);
	~MVisu();

	void Draw(MEvent*,int);
	void SetListToDraw(char*);
	void End();

	TApplication* App;
	
	TList* ListToDraw;
	TList* ListOfCanvas;
	
	TCanvas* Canv;
	TH2S* hBLShape[2];
	TH2F* PadSummary;
	
};

#endif
