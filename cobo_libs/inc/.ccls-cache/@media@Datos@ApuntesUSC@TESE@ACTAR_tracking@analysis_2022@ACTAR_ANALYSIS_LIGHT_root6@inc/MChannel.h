#ifndef MCHANNEL_H
#define MCHANNEL_H

#include <stdio.h>
#include <iostream>
#include <cstdlib>
#include <vector>
#include <TObject.h>

#include <Parameters.h>

using namespace std;

class MChannel
{
	public:
	int channel_number;
	int row_number;
	int col_number;
	std::vector<unsigned short> Sample_Number;
	unsigned int Raw_Sample[NB_SAMPLES];
//	float Cal_Sample[NB_SAMPLES];
	float Calib_Coef[3];
	
	MChannel();
	~MChannel();
};


#endif
