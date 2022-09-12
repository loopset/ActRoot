// File :  GCobo.h
// Author: Luc Legeard
//////////////////////////////////////////////////////////////////////////////
//
// Description of a Cobo card
//
/////////////////////////////////////////////////////////////////////////////

// ---------------------------------------------------------------------------
// ***************************************************************************
// *                                                                         *
// *   This program is free software; you can redistribute it and/or modify  *
// *   it under the terms of the GNU General Public License as published by  *
// *   the Free Software Foundation; either version 2 of the License, or     *
// *   (at your option) any later version.                                   *
// *                                                                         *
// ***************************************************************************/


#ifndef __GCobo__
#define __GCobo__

// definition of constant cobo cards
// a Cobo card have (NB_AGET_CHANNEL *  NB_AGET * NB_ASAD) signals  channel. Each signal is a trace of NB_SAMPLE samples

#define COBO_NB_ASAD    4  // nb of asad par card Cobo Card
#define COBO_NB_ASAD_REDUCED    1 // nb of asad par card reduced Cobo Card
#define COBO_NB_AGET    4 // Nb of AGET per ASAD Card
#define COBO_NB_AGET_CHANNEL    68 //( 64 + 4 tests) NB of Channels per AGET
#define COBO_NB_SAMPLES   512 // nb of bin in each channel (bucket)
#define COBO_NB_EVTTIME     6 // Nb of bytes of event time
#define COBO_NB_EVTNB       4 // Nb of bytes of event number

#endif


