#include "Silicons.h"

#include "ActParameters.h"

Silicons::Silicons()
	: fSi0(ActParameters::NrowsSi01, 0.), fSi0_cal(ActParameters::NrowsSi01, 0.),
	  fSi1(ActParameters::NrowsSi01, 0.), fSi1_cal(ActParameters::NrowsSi01, 0.),
	  fSiS(ActParameters::NrowsSiS, 0.),  fSiS_cal(ActParameters::NrowsSiS, 0.),
	  fSiF(ActParameters::NrowsSi01, 0.), fSiF_cal(ActParameters::NrowsSi01, 0.),
	  fSiBeam(ActParameters::NrowsSiBeam, 0.), fSiBeam_cal(ActParameters::NrowsSiBeam, 0.)
{
}
