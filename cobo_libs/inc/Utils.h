///////////////////////////////////////
//                                   //
// T. Roger - GANIL 2015             //
// email: roger@ganil.fr             //
//                                   //
// Utils functions definition        //
//                                   //
///////////////////////////////////////

#include <stdio.h>
#include <iostream>
#include <Parameters.h>
#include <MTrack.h>

#include <TH1.h>
#include <TH2.h>
#include <TSpectrum.h>
#include <TF1.h>
#include <TGraph.h>

#define R2D 57.2957795
#define D2R 0.01745329

using namespace std;


void Calibrate(TH2I*);
void CleanPad(float[NPADY][NPADX],float[NPADY][NPADX],float[NPADY][NPADX]);
bool GetVertexFromRMS(TH2F*,int&,int&);
bool GetVerticalConsistensy(MTrack*,MTrack*);
float AverageEloss(MTrack*,float[NPADY][NPADX]);
void FitMat(float[NPADY][NPADX],int,int,int,int,float,float&,float&);
float FitMat3D(float[NPADY][NPADX], float[NPADY][NPADX], int, int, int, int, float, MTrack*);
float DEdX(MTrack*, float[NPADY][NPADX], float, TH1D*);
void MakeConfigFileThr(char*,char*,float[NB_COBO*NB_ASAD*NB_AGET*NB_CHANNEL],float);

double PulseFit(double*, double*);
float Kalman(float*, float*);
