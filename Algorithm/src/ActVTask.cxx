#include "ActVTask.h"

#include "ActOptions.h"

#include <iostream>

ActAlgorithm::VTask::VTask(const std::string& taskID)
    : fTaskID(taskID),
      fIsVerbose(ActRoot::Options::GetInstance()->GetIsVerbose())
{
}

void ActAlgorithm::VTask::ReadConfiguration() {}

bool ActAlgorithm::VTask::Run()
{
    return false;
}

void ActAlgorithm::VTask::Print()
{
    std::cout << "····· Task : " << fTaskID << "·····" << '\n';
}
