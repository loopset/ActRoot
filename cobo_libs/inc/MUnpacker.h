#ifndef MUNPACKER_H
#define MUNPACKER_H

#include <MFMCommonFrame.h>
#include <MFMCoboFrame.h>
#include <MFMMergeFrame.h>
#include <MFMMutantFrame.h>
#include <MFMEbyedatFrame.h>
#include <DataParameters.h>

#include <MEvent.h>

using namespace std;


class MUnpacker
{
	public:

	MUnpacker();
	~MUnpacker();
	
	long int Unpack(MFMCommonFrame*, MEvent*);
	bool CheckAndRepairCorruptedFrame(MFMCommonFrame*, MCoboAsad&);
	
	MFMCommonFrame* insideframe;
	MFMMergeFrame* mergeframe;
	MFMCoboFrame* coboframe;
	MFMMutantFrame* mutantframe;
	MFMEbyedatFrame* ebyedatframe;
	
	unsigned int nbitems;
	unsigned int sample;
	unsigned int buckidx;
	unsigned int chanidx;
	unsigned int agetidx;
	unsigned int asadidx;
	unsigned int coboidx;

	unsigned int SAMPLE[NB_AGET][NB_CHANNEL][NB_SAMPLES];
};



#endif
