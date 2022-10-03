#ifndef ACTANALYZER_H
#define ACTANALYZER_H

#include "ActSRIM.h"
#include "ActKinematics.h"
#include "ActStructs.h"

#include <TH1.h>
#include <TH2.h>
#include <THStack.h>
#include <TTree.h>
#include <TCanvas.h>
#include <TCutG.h>

#include <map>
#include <memory>
#include <string>
#include <vector>

class ActAnalyzer
{
private:
	//to read ttree
	TTree* fTree { nullptr};
	Silicons* fSilicons {nullptr};
	TriggersAndGates* fTriggers {nullptr};
	std::vector<TrackPhysics>* fTracks {nullptr};

	//canvas and histograms
	std::unique_ptr<TCanvas> fCanvTrackID {nullptr};
	std::unique_ptr<TH2D> fHistTrackID {nullptr};
	std::unique_ptr<TCanvas> fCanvRecoilEnergy {nullptr};
	std::unique_ptr<TH1D> fHistRecoilEnergy {nullptr};
	std::unique_ptr<TCanvas> fCanvExcitation {nullptr};
	std::map<std::string, std::unique_ptr<TH1D>> fHistosExcitation {};
	std::unique_ptr<THStack> fStackExcitation {nullptr};

	//read graphical cuts, mainly for track particle ID
	//enable graphical cuts service
	bool fEnableGraphicalCuts { false};
	//skip custom particles
	//WARNING: keys should be the same as ActSRIM
	std::vector<std::string> fSkipParticles {};
	std::map<std::string, std::unique_ptr<TCutG>> fGCuts {};

	//set parameters of reaction
	int fTracksPerEvent { 1};//default to binary reactions
	
public:
	ActAnalyzer(TTree* tree, std::unique_ptr<TH2D> histTrackID,
				std::unique_ptr<TH1D> histRecoilEnergy,
				std::unique_ptr<TH1D> histExcitation,
				std::vector<std::string> excitationKeys = {"p"});
	~ActAnalyzer() = default;

	//read TTree and set branches to our private members
	//execute all computations
	void ReadTree(ActSRIM& srim, ActKinematics& kinematics);
	//draw canvas at the end of execution
	void DrawCanvas();

	//read graphical cuts
	void SetEnableGraphicalCuts(bool val){ fEnableGraphicalCuts = val; }
	bool GetEnableGraphicalCuts() const { return fEnableGraphicalCuts; }
	void ReadGraphicalCuts(std::string key, std::string fileName);
	void SetSkipParticles(std::vector<std::string> vec);
	std::vector<std::string> GetSkipParticles() const { return fSkipParticles; }

	//setters and getter for reaction parameters
	void SetTracksPerEvent(int val) { fTracksPerEvent = val; }
	int GetTracksPerEvent() const { return fTracksPerEvent; }

private:
	void ProcessTrackID();
	void ProcessRecoilEnergy(ActSRIM& srim, ActKinematics& kinematics);
	//function to get energy from silicons, keeping in mind multiplicity, side and so on
	double GetGatedSiliconEnergy(TrackPhysics& track, std::string frontPanel = "0");
	std::string IdentifyRecoilInGraphCuts(TrackPhysics& track);
	
	template<typename T>
	inline bool isInVector(T val, std::vector<T> vec)
	{
		if (vec.size() == 0)
			return false;
		return std::find(vec.begin(), vec.end(), val) != vec.end();
	}
	inline bool isInExcitationMap(std::string val)
	{
		if(fHistosExcitation.find(val) == fHistosExcitation.end())
			return false;
		else
			return true;
	}

};

#endif //ACTANALIZER_H
