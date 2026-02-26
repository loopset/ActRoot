#include "ActTaskManager.h"

#include "ActColors.h"
#include "ActOptions.h"
#include "ActVTask.h"

#include "TString.h"
#include "TSystem.h"

#include <dlfcn.h>

#include <algorithm>
#include <filesystem>
#include <iostream>
#include <iterator>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>

void ActAlgorithm::TaskManager::AddTask(const std::string& taskID, TaskFcn fcn)
{
    fTaskIDs.push_back(taskID);
    fTasks.push_back(std::move(fcn));
}

void ActAlgorithm::TaskManager::AddTask(const std::string& path, const std::string& taskID, const std::string& at)
{
    // Load from library file
    LoadPlugin(path, taskID);
    // Get ptr to VTask
    auto ptr {fPlugins.back()};
    // And add at given position
    AddAt(ptr, at);
}

void ActAlgorithm::TaskManager::LoadPlugin(const std::string& path, const std::string& taskID)
{
    // Same operations as in MultiAction::LoadUserAction
    // Build path + name
    auto file {ActRoot::Options::GetInstance()->GetProjectDir() + path + "lib" + taskID + "." + gSystem->GetSoExt()};
    // If it doesnt exist, throw error
    if(!std::filesystem::exists(file))
    {
        throw std::runtime_error("TaskManager::LoadPlugin(): cannot locate library " + taskID);
    }

    // Load the .so
    auto library {dlopen(file.c_str(), RTLD_NOW | RTLD_GLOBAL)};
    if(!library)
        throw std::runtime_error("TaskManager: failed to execute dlopen() " + std::string(dlerror()));
    auto creator {reinterpret_cast<VTask* (*)()>(dlsym(library, "Create"))};
    if(!creator)
    {
        dlclose(library);
        throw std::runtime_error("TaskManager::LoadPlugin(): cannot open Create() in library " + taskID);
    }
    // If success, call constructor in extern "C" function
    fPlugins.push_back(std::shared_ptr<VTask>(creator()));
}

void ActAlgorithm::TaskManager::AddAt(TaskPtr ptr, const std::string& at)
{
    // If at is empty, just push back
    if(at.empty())
    {
        AddTask(ptr->GetTaskID(), [ptr]() { return ptr->Run(); });
        return;
    }
    // Parse at
    // At should come in the form of: [Before/After]_OtherTaskID
    auto underscore {at.find("_")};
    std::string loc {at.substr(0, underscore)};
    std::string otherID {at.substr(underscore + 1)};
    // Attempt to locate otherID
    auto it {std::find(fTaskIDs.begin(), fTaskIDs.end(), otherID)};
    if(it == fTaskIDs.end())
    {
        std::cout << BOLDRED << "TaskManager::AddAt(): cannot find otherID " << otherID << " in vector : " << '\n'
                  << "  ";
        for(const auto& id : fTaskIDs)
            std::cout << id << ", ";
        std::cout << '\n';
        throw std::runtime_error("Check your passed taskID!");
    }
    // It exists
    auto idx {std::distance(fTaskIDs.begin(), it)};
    // Create wrapper
    auto lambda {[ptr]() { return ptr->Run(); }};
    // And add at the correct position
    auto tstr {TString(at)};
    tstr.ToLower();
    if(tstr.Contains("bef"))
    {
        // Add taskID
        fTaskIDs.insert(fTaskIDs.begin() + idx, ptr->GetTaskID());
        // Add task
        fTasks.insert(fTasks.begin() + idx, lambda);
    }
    else if(tstr.Contains("aft"))
    {
        // Add taskID
        fTaskIDs.insert(fTaskIDs.begin() + idx + 1, ptr->GetTaskID());
        // Add task
        fTasks.insert(fTasks.begin() + idx + 1, lambda);
    }
    else
        AddTask(ptr->GetTaskID(), [ptr]() { return ptr->Run(); });
}

void ActAlgorithm::TaskManager::Print() const
{
    std::cout << BOLDGREEN << "····· TaskManager ·····" << '\n';
    std::cout << "Registered tasks: " << '\n';
    for(const auto& id : fTaskIDs)
        std::cout << "  " << id << '\n';
    std::cout << "of which we own: " << '\n';
    for(const auto& plugin : fPlugins)
        std::cout << "  " << plugin->GetTaskID() << '\n';
    std::cout << "····················" << RESET << '\n';
    // And call the prints of each plugin
    for(const auto& plugin : fPlugins)
        plugin->Print();
}
