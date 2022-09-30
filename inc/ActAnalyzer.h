#ifndef ACTANALYZER_H
#define ACTANALYZER_H

#include "ActSRIM.h"
#include "ActStructs.h"

#include <TH1.h>
#include <TH2.h>
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

	//read graphical cuts, mainly for track particle ID
	//enable graphical cuts service
	bool fEnableGraphicalCuts { false};
	//skip custom particles
	//WARNING: keys should be the same as ActSRIM
	std::vector<std::string> fSkipParticles {};
	std::map<std::string, std::unique_ptr<TCutG>> fGCuts {};

	//set parameters of reaction
	double fInitialBeamEnergy {35. * 20.004};//default-initialized to 20O as E796
	std::string fBeamIsotope {"20O"};
	int fTracksPerEvent { 1};//default to binary reactions
	
public:
	ActAnalyzer(TTree* tree, std::unique_ptr<TH2D> histTrackID,
				std::unique_ptr<TH1D> histRecoilEnergy);
	~ActAnalyzer() = default;

	//read TTree and set branches to our private members
	//execute all computations
	void ReadTree(ActSRIM& srim);
	//draw canvas at the end of execution
	void DrawCanvas();

	//read graphical cuts
	void SetEnableGraphicalCuts(bool val){ fEnableGraphicalCuts = val; }
	bool GetEnableGraphicalCuts() const { return fEnableGraphicalCuts; }
	void ReadGraphicalCuts(std::string key, std::string fileName);
	void SetSkipParticles(std::vector<std::string> vec);
	std::vector<std::string> GetSkipParticles() const { return fSkipParticles; }

	//setters and getter for reaction parameters
	void SetInitialBeamEnergy(double energy) { fInitialBeamEnergy = energy; }
	double GetInitialBeamEnergy() const { return fInitialBeamEnergy; }
	void SetBeamIsotope(std::string isotope) { fBeamIsotope = isotope; }
	std::string GetBeamIsotope() const { return fBeamIsotope; }
	void SetTracksPerEvent(int val) { fTracksPerEvent = val; }
	int GetTracksPerEvent() const { return fTracksPerEvent; }

private:
	void ProcessTrackID();
	void ProcessRecoilEnergy(ActSRIM& srim);
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
};

#endif //ACTANALIZER_H
