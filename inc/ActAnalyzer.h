#ifndef ACTANALYZER_H
#define ACTANALYZER_H

#include "ActSRIM.h"
#include "ActKinematics.h"
#include "ActStructs.h"

#include <Math/Point3Dfwd.h>
#include <Math/Vector3Dfwd.h>
#include <TH1.h>
#include <TH2.h>
#include <THStack.h>
#include <TTree.h>
#include <TCanvas.h>
#include <TCutG.h>

#include <cstring>
#include <map>
#include <memory>
#include <string>
#include <vector>

class ActAnalyzer
{
public:
	using XYZPoint = ROOT::Math::XYZPoint;
	using XYZVector = ROOT::Math::XYZVector;
private:
	//to read ttree
	TTree* fTree { nullptr};
	int fRunID {};
	int fEventID {};
	Silicons* fSilicons {nullptr};
	TriggersAndGates* fTriggers {nullptr};
	std::vector<TrackPhysics>* fTracks {nullptr};

	//canvas and histograms
	std::unique_ptr<TCanvas> fCanvTrackID {nullptr};
	std::unique_ptr<TH2D> fHistTrackID {nullptr};
	std::unique_ptr<TCanvas> fCanvWall {nullptr};
	std::unique_ptr<TH2D> fHistWall {nullptr};
	std::unique_ptr<TCanvas> fCanvRecoilEnergy {nullptr};
	std::unique_ptr<TH1D> fHistRecoilEnergy {nullptr};
	std::unique_ptr<TCanvas> fCanvExcitation {nullptr};
	std::map<std::string, std::unique_ptr<TH1D>> fHistosExcitation {};
	std::unique_ptr<THStack> fStackExcitation {nullptr};
	//check kinematics
	std::unique_ptr<TCanvas> fCanvKinematics {nullptr};
	std::map<std::string, std::unique_ptr<TH2D>> fHistosKinematics {};
	std::map<std::string, std::unique_ptr<TH2D>> fHistosTheoreticalKinematics {};
	std::unique_ptr<THStack> fStackKinematics {nullptr};
	std::unique_ptr<THStack> fStackTheoreticalKinematics {nullptr};

	//read graphical cuts, mainly for track particle ID
	//enable graphical cuts service
	bool fEnableGraphicalCuts { false};
	//skip custom particles
	//WARNING: keys should be the same as ActSRIM
	std::vector<std::string> fSkipParticles {};
	std::map<std::string, std::unique_ptr<TCutG>> fGCuts {};

	//silicon wall cuts
	std::map<std::string, std::unique_ptr<TCutG>> fWallCuts {};
	bool fEnableWallCuts {false};

	//custom TCutG to save indexes
	std::unique_ptr<TCutG> fAuxiliarCut {nullptr};
	bool fEnableAuxiliarCut {false};
	bool fWriteToStreamer {false};
	std::string fAuxiliarCutOutputFile {""};

	//set parameters of reaction
	int fTracksPerEvent { 1};//default to binary reactions

	//set mode of analysis
	std::string fSiliconMode {"side"};
	
public:
	//constructor taking histograms as templates
	ActAnalyzer(std::unique_ptr<TH2D> histTrackID,
				std::unique_ptr<TH2D> histWall,
				std::unique_ptr<TH1D> histRecoilEnergy,
				std::unique_ptr<TH1D> histExcitation,
				std::unique_ptr<TH2D> histKinematics,
				std::vector<std::string> excitationKeys = {"p"});
	~ActAnalyzer() = default;

	//set ttree to analyze
	void SetTree(TTree* tree){ fTree = tree; };
	//read TTree and set branches to our private members
	//execute all computations
	void ReadTree(ActSRIM& srim, ActKinematics& kinematics);
	//draw canvas at the end of execution
	void DrawCanvas(int canvasForm = 1);

	//read graphical cuts
	void SetEnableGraphicalCuts(bool val){ fEnableGraphicalCuts = val; }
	bool GetEnableGraphicalCuts() const { return fEnableGraphicalCuts; }
	void ReadGraphicalCuts(std::string key, std::string fileName);
	void SetEnableWallCuts(bool val){ fEnableWallCuts = val; }
	bool GetEnableWallCuts() const { return fEnableWallCuts; }
	void ReadWallCuts(std::string key, std::string fileName);
	void SetEnableAuxiliarCut(bool val){ fEnableAuxiliarCut = val; }
	void ReadAuxiliarCut(std::string fileName);
	void SetAuxiliarCutOutputFile(std::string fileName){ fAuxiliarCutOutputFile = fileName; }
	void SetSkipParticles(std::vector<std::string> vec);
	std::vector<std::string> GetSkipParticles() const { return fSkipParticles; }

	//setters and getter for reaction parameters
	void SetTracksPerEvent(int val) { fTracksPerEvent = val; }
	int GetTracksPerEvent() const { return fTracksPerEvent; }
	void SetSiliconMode(std::string mode){ fSiliconMode = mode; }
	std::string GetSiliconMode() const { return  fSiliconMode; }

private:
	void ProcessTrackID();
	void ProcessRecoilEnergy(ActSRIM& srim, ActKinematics& kinematics);
	//function to get energy from silicons, keeping in mind multiplicity, side and so on
	double GetGatedSiliconEnergy(const TrackPhysics& track, std::string frontPanel = "0");
	std::string IdentifyEjectileInGraphCuts(const TrackPhysics& track);
	void PropagateBeamInChamber(const TrackPhysics& track, ActSRIM& srim, ActKinematics& kinematics);
	
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
