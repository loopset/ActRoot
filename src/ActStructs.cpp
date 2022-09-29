#include "ActStructs.h"

#include "ActParameters.h"

Silicons::Silicons()
	: fSi0(ActParameters::NrowsSi01, 0.), fSi0_cal(ActParameters::NrowsSi01, 0.),
	  fSi1(ActParameters::NrowsSi01, 0.), fSi1_cal(ActParameters::NrowsSi01, 0.),
	  fSiS(ActParameters::NrowsSiS, 0.),  fSiS_cal(ActParameters::NrowsSiS, 0.),
	  fSiF(ActParameters::NrowsSi01, 0.), fSiF_cal(ActParameters::NrowsSi01, 0.),
	  fSiBeam(ActParameters::NrowsSiBeam, 0.), fSiBeam_cal(ActParameters::NrowsSiBeam, 0.)
{
}

void TrackPhysics::Print()
{
	std::cout<<BOLDGREEN<<"===== Track "<<fTrackID<<" ====="<<RESET<<'\n';
	std::cout<<" Total charge: "<<fTotalCharge<<'\n';
	std::cout<<" Reaction point in mm at X: "<<fReactionPoint.X()<<" Y: "<<fReactionPoint.Y()<<" Z: "<<fReactionPoint.Z()<<'\n';
	std::cout<<" Silicon point "<<fSiliconPlace<<" with coordinates in mm"<<'\n';
	std::cout<<"  X: "<<fSiliconPoint.X()<<" Y: "<<fSiliconPoint.Y()<<" Z: "<<fSiliconPoint.Z()<<'\n';
	std::cout<<" Track length: "<<fTrackLength<< " mm and average charge: "<<fAverageCharge<<" / mm"<<'\n';
	std::cout<<" Theta: "<<fTheta<<" degrees and phi: "<<fPhi<<" degrees"<<'\n';
	std::cout<<BOLDGREEN<<"============"<<RESET<<'\n'; 
}
