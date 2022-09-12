/*
 MFMBlobFrame.cc
 Copyright Acquisition group, GANIL Caen, France
 */

#include <iostream>
#include <cmath>
#include <string.h>
#include <iostream>
#include <stdio.h>
#include <sstream>
using namespace std;

#include "MFMBlobFrame.h"

//_______________________________________________________________________________
MFMBlobFrame::MFMBlobFrame(int unitBlock_size, int dataSource,
		int frameType, int revision, int frameSize){

    MFMCommonFrame::MFM_make_header(unitBlock_size, dataSource, frameType,
			revision ,frameSize);
}
//_______________________________________________________________________________
MFMBlobFrame::MFMBlobFrame() {

}
//_______________________________________________________________________________
MFMBlobFrame::~MFMBlobFrame(){

}
//_______________________________________________________________________________
