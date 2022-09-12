#ifndef MCOBOASAD_H
#define MCOBOASAD_H

#include <stdio.h>
#include <iostream>
#include <cstdlib>
#include <vector>
#include <TObject.h>

#include "MChannel.h"

using namespace std;

class MCoboAsad
{
	public:
	int cobo_number;
	int asad_number;
	int global_asad_number;
	int EN;
// 	uint64_t TS;
// 	uint16_t last_cell[NB_AGET];
// 	uint16_t multiplicity[NB_AGET];
	unsigned long int TS;
	unsigned int last_cell[NB_AGET];
	unsigned int multiplicity[NB_AGET];
	std::vector<unsigned short> hit_pattern;
  unsigned int hit_pattern_tab[NB_CHANNEL*NB_AGET];
	MChannel Channel[NB_CHANNEL*NB_AGET];
	
	MCoboAsad();
	~MCoboAsad();
};

#endif
