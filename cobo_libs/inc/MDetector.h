#ifndef MDETECTOR_H
#define MDETECTOR_H

#include <string>
#include <stdarg.h>
#include "TNamed.h"

using namespace std;

class MDetector:public TNamed
{
	public:
	
	MDetector(char*, int, char*, char*);
	MDetector(MDetector&);
	~MDetector();
	void Calibrate(float);
	void Merge(int, ...);
	void Print(FILE*,char);
	string GetParname(){return(*parname);}
	int GetNumber(){return(number);}
	float GetValR(int i){return(val_r[i]);}
	float GetValC(int i){return(val_c[i]);}
	void SetRaw(int p, float v){val_r[p]=v;}
	float GetRaw(int p){return(val_r[p]);}
	float GetCal(int p){return(val_c[p]);}
	float GetCalCoeff(int p,int o){return(cal[p][o]);}
	float GetCalPied(int p){return(pied[p]);}
	
	void Reset();
	int IsNotEmpty;
	int CurChNb;

	
	private:
	
	string* parname;
	int IsCalib;
	int number;
	int nparam;
	int cal_order;
	float* pied;
	float** cal;
	float* val_r;
	float* val_c;
};


#endif
