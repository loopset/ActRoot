#include <MCoboAsad.h>

using namespace std;

MCoboAsad::MCoboAsad()
{
	hit_pattern.clear();
  for(int i=0;i<NB_CHANNEL*NB_AGET;i++)
    hit_pattern_tab[i]=0;
}


MCoboAsad::~MCoboAsad()
{
	
}
