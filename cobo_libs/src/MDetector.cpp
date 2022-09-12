#include "MDetector.h"
#include "Errors.h"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <math.h>

using namespace std;

///////////////////////////:///////////////////////////////////
// CALIBRATION FILE SYNTAX:
// calib_order		%d
// pied pow_0 ... pow_calib_order %f %f ... %f (repeat nb times)
///////////////////////////////////////////////////////////////

MDetector::MDetector(char* type, int nb, char* parn, char* calpath)
{
	SetTitle(type);
	IsCalib=0;
	CurChNb=0;
	nparam=nb;
	if(nb)
	{
		parname=new string(parn);
		number=nb;
	}
	else
	{
		parname=new string(parn);
		number=1;
		for(int i=0;i<parname->size();i++)
			if(parn[i]=='|') number++;
	}
	
	val_r=(float*)malloc(abs(number)*sizeof(float));
	val_c=(float*)malloc(abs(number)*sizeof(float));


	if(calpath!="")
	{
		FILE* calib;
		calib=fopen(calpath,"r");
		if(calib!=NULL)
		{
			fscanf(calib,"%d",&cal_order);
			pied=(float*)malloc(abs(number)*sizeof(float));
			cal= (float**)malloc(abs(number)*sizeof(float*));
			for(int i=0;i<abs(number);i++)
				cal[i]=(float*)malloc((cal_order+1)*sizeof(float));
			for(int i=0;i<abs(number);i++)
			{
				fscanf(calib,"%f",&pied[i]);
				for(int j=0;j<=cal_order;j++)
					fscanf(calib,"%f",&cal[i][j]);
			}
			IsCalib=1;		
		}
		else
		{
			cout << ERR_FILE << calpath << endl;
			exit(1);
		}
		fclose(calib);
	}
}
	

MDetector::MDetector(MDetector& D)
{
	SetTitle(D.GetTitle());
	number=D.number;
	IsCalib=D.IsCalib;
	CurChNb=D.CurChNb;
	val_r=(float*)malloc(abs(number)*sizeof(float));
	val_c=(float*)malloc(abs(number)*sizeof(float));
	parname=D.parname;
	for(int i=0;i<abs(number);i++)
	{
		val_r[i]=D.val_r[i];
		val_c[i]=D.val_c[i];
	}
}


MDetector::~MDetector()
{
	free(val_r);
	free(val_c);
	
	if(IsCalib)
	{
		for(int i=0;i<cal_order;i++)
			free(cal[i]);
		free(cal);
	}
	free(pied);
	
	delete parname;
}


void MDetector::Calibrate(float suppied)
{	
	if(IsCalib)
	{
		for(int i=0;i<abs(number);i++)
		{
			val_c[i]=0;
			if(val_r[i]>pied[i]+suppied)
				for(int j=0;j<=cal_order;j++)
					val_c[i]+=cal[i][j]*pow(val_r[i],j);
		}
	}
	else cout << "No calibration available for detector " << GetTitle() << endl;
}

void MDetector::Merge(int n, ...)
{
	va_list args;
	va_start(args,n);
	int ch=0;
	
	for(int i=0;i<n;i++)
	{
		MDetector* D=new MDetector(*(MDetector*)va_arg(args,MDetector*));
		int start=va_arg(args,int);
		int stop=va_arg(args,int);
		for(int j=start;j<=min(stop,abs(D->GetNumber())-1);j++)
		{
			val_r[ch]=D->val_r[j];
			val_c[ch]=D->val_c[j];
	// 		if(val_r[ch]!=0 || val_c[ch]!=0) IsNotEmpty=1;
			ch++;
			if(ch>=abs(this->GetNumber())) break;
		}
	}
	va_end(args);
}


void MDetector::Print(FILE* f,char c)
{
	if(c=='c' || c=='C')
		for(int i=0;i<abs(number);i++)
			fprintf(f,"%x\t%d\t%.5f\n",this,i,val_c[i]);
			
	if(c=='r' || c=='R')
		for(int i=0;i<abs(number);i++)
			fprintf(f,"%x\t%d\t%.1f\n",this,i,val_r[i]);
}

void MDetector::Reset()
{
	IsNotEmpty=0;
	CurChNb=0;
	for(int i=0;i<abs(number);i++)
		val_r[i]=val_c[i]=-1000;
}
