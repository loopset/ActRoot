///////////////////////////////////////
//                                   //
// T. Roger - GANIL 2015             //
// email: roger@ganil.fr             //
//                                   //
// MTrack class definition           //
//                                   //
///////////////////////////////////////

#ifndef MTRACK_H
#define MTRACK_H

#include <TLine.h>
#include <Parameters.h>

class MTrack
{
	public:
	MTrack();
	MTrack(char*);
	~MTrack();
	void ResetLines();
	
	int zx_s;
	int zx_e;
	int zy_s;
	int zy_e;	
	
	float Xm;
	float Ym;
	float Zm;
	float Xh;
	float Yh;
	float Zh;
	
	TLine* L2DXY;
	TLine* L2DXZ;
	TLine* L2DYZ;
	TLine* L3D;
	
	float ElossTable[NPADX][2];
};

#endif
