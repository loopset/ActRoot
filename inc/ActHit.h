#ifndef ACTHIT_H
#define ACTHIT_H

#include <Math/Point3D.h>
#include <Math/Point3Dfwd.h>
#include <Math/Vector3D.h>
#include <Math/Vector3Dfwd.h>
#include <Rtypes.h>
#include <RtypesCore.h>
#include <TObject.h>//inheritance from base class of ROOT

//std includes
#include <algorithm>
#include <memory>
#include <vector>

//define of actar tpc hit inside active chamber, following AT-TPC
//https://github.com/ATTPC/ATTPCROOTv2/blob/develop/AtData/AtHit.h
class ActHit : public TObject
{
public:
	using XYZPoint = ROOT::Math::XYZPoint;
	using XYZVector = ROOT::Math::XYZVector;

protected:
	Double_t fCharge{-1};
	XYZPoint fPosition{XYZPoint(0, 0, -1000)};
	Int_t fHitID{-1}; //unique identifier of hit in chamber

	
public://member functions
	ActHit() = default;//default
	ActHit(Int_t hitID, XYZPoint location, Double_t charge);
	ActHit(const ActHit& ) = default; //copy constructor
	ActHit(ActHit&& ) = default; //move constructor
	ActHit &operator=(const ActHit& ) = default; // copy assignment
	ActHit &operator=(ActHit&& ) = default; //move assignment
	virtual ~ActHit() = default; //destructor
	virtual std::unique_ptr<ActHit> Clone(); //create clone as TObject derived class

	void SetCharge(Double_t charge){ fCharge = charge; }
	void SetPosition(const XYZPoint& pos){ fPosition = pos; }
	void SetHitID(Int_t hitID){ fHitID = hitID; }

	Int_t GetHitID() const { return fHitID; }
	Double_t GetCharge() const { return fCharge; }
	const XYZPoint& GetPosition() const { return fPosition; }
	
	//for ROOT class definiton
	ClassDef(ActHit, 1);
   
};

#endif //ACTHIT_H
