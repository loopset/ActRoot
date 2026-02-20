#ifndef ActVTask_h
#define ActVTask_h

#include <string>

// forward declarations
namespace ActRoot
{
class MergerData;
class MergerParameters;
class SilData;
class ModularData;
} // namespace ActRoot

namespace ActAlgorithm
{
class VTask
{
protected:
    // Pointers to data and parameters
    ActRoot::MergerData* fMergerData {};
    ActRoot::MergerParameters* fMergerPars {};
    ActRoot::SilData* fSilData {};
    ActRoot::ModularData* fModData {};

    // Basic members of task
    std::string fTaskID {};
    bool fIsEnabled {true};
    bool fIsVerbose {};

public:
    VTask(const std::string& taskID = "Base");
    virtual ~VTask() = default;

    // Setters
    void SetMergerData(ActRoot::MergerData* data) { fMergerData = data; }
    void SetMergerParameters(ActRoot::MergerParameters* pars) { fMergerPars = pars; }
    void SetSilData(ActRoot::SilData* data) { fSilData = data; }
    void SetModularData(ActRoot::ModularData* data) { fModData = data; }

    // Getters
    ActRoot::MergerData* GetMergerData() const { return fMergerData; }
    ActRoot::MergerParameters* GetMergerParameters() const { return fMergerPars; }
    const std::string& GetTaskID() const { return fTaskID; }
    bool GetIsEnabled() const { return fIsEnabled; }
    bool GetIsVerbose() const { return fIsVerbose; }

    // Base
    virtual void ReadConfiguration();
    virtual bool Run();
    virtual void Print();
};
} // namespace ActAlgorithm

#endif // !ActVTask_h
