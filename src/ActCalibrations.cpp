#include "ActCalibrations.h"

#include "ActParameters.h"

#include <fstream>
#include <vector>
#include <string>

ActCalibrations::ActCalibrations()
	: fTABLE(6, std::vector<int>(ActParameters::g_NB_COBO *
								 ActParameters::g_NB_ASAD *
								 ActParameters::g_NB_AGET *
								 ActParameters::g_NB_CHANNEL, 0)),
	  fPadAlignCoefs(ActParameters::NCoefRows, std::vector<double>(3 , 0.))
{
}

void ActCalibrations::ReadTABLE(std::string& tableFile)
{
	std::ifstream streamer(tableFile.c_str());
	int aux0, aux1, aux2, aux3, aux4, aux5;
	for(int i = 0; i < fTABLE[0].size(); i++)
	{
		streamer >> aux0 >> aux1 >> aux2 >> aux3 >> aux4 >> aux5;
		fTABLE[0][i] = aux0; fTABLE[1][i] = aux1;
		fTABLE[2][i] = aux2; fTABLE[3][i] = aux3;
		fTABLE[4][i] = aux4; fTABLE[5][i] = aux5;
	}
	streamer.close();
}

void ActCalibrations::ReadPadAlignCoefs(std::string &coefsFile)
{
	std::ifstream streamer(coefsFile.c_str());
	double aux0, aux1, aux2;
	for(int i = 0; i < ActParameters::NCoefRows; i++)
	{
		streamer >> aux0 >> aux1 >> aux2;
		fPadAlignCoefs[i][0] = aux0;
		fPadAlignCoefs[i][1] = aux1;
		fPadAlignCoefs[i][2] = aux2;
	}
	streamer.close();
}
