///////////////////////////////////////
//                                   //
// T. Roger - GANIL 2015             //
// email: roger@ganil.fr             //
//                                   //
// Main file:                        //
//     - directories definitions     //
//     - frame merging               //
//                                   //
///////////////////////////////////////

#include <math.h>
#include <stdio.h>
#include <time.h>
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <fcntl.h>
#include <unistd.h>
#include <MExperiment.h>
//#include <DataParameters.h>

using namespace std;

int main(int argc,char *argv[])
{
	int runf, runl, cobo;
	system("rm runlist.d 2>/dev/null");
	
	
	MExperiment* Experiment=new MExperiment(argc,argv);
	
	Experiment->TreatRuns();
	Experiment->EndOfExperiment();		

	bool isOK;


	return(0);
}
