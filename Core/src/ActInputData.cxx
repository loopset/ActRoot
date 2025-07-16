#include "ActInputData.h"

#include "ActColors.h"
#include "ActInputParser.h"

#include "TChain.h"
#include "TFile.h"
#include "TString.h"
#include "TTree.h"

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>


void ActRoot::InputData::ParseBlock(ActRoot::BlockPtr block)
{
    // Each block should consist of:
    // 1-> TTree name
    // 2-> Path to folder (abs or relative)
    // 3-> File name BEGIN
    // 4-> File name END (optional)
    // The input will read a file: /path/to/folder/BEGIN{%.4d of run}END.root

    // 1
    fTreeNames.push_back(block->GetString("TreeName"));
    // 2
    std::filesystem::path path {block->GetString("Path")};
    if(path.is_relative()) // correct relative paths
        path = SolveRelativePath(path);
    fPaths.push_back(path);
    // 3
    fBegins.push_back(block->GetString("Begin"));
    // 4
    if(block->CheckTokenExists("End", true))
        fEnds.push_back(block->GetString("End"));
    else
        fEnds.push_back("");
}

void ActRoot::InputData::CheckFileExists(const std::string& file)
{
    if(!std::filesystem::exists(file))
        throw std::runtime_error("InputData: file " + file + " does not exist");
}

void ActRoot::InputData::CheckTreeExists(std::shared_ptr<TTree> tree, int i)
{
    if(!tree)
        throw std::runtime_error("InputData: tree " + fTreeNames[i] + " could not be opened");
}

void ActRoot::InputData::Init(const std::set<int>& runs, bool print)
{
    fRuns = runs;
    // Assert that we have at least 1 input
    if(fTreeNames.size() < 1)
        throw std::runtime_error("InputData::Init(): size of internal vectors < 1 -> No inputs to read!");
    for(int in = 0; in < fTreeNames.size(); in++)
    {
        for(const auto& run : runs)
        {
            std::string filename {fPaths[in] + fBegins[in] + TString::Format("%04d", run) + fEnds[in] + ".root"};
            CheckFileExists(filename);
            // Print!
            if(print)
            {
                std::cout << BOLDYELLOW << "InputData: reading " << fTreeNames[in] << " tree in file " << '\n';
                std::cout << "  " << filename << RESET << '\n';
            }
            // Init
            if(in == 0) // Init pointers, this is set as the main input!
            {
                fFiles[run] = std::make_shared<TFile>(filename.c_str()); // READ mode by default
                fTrees[run] = std::shared_ptr<TTree>(fFiles[run]->Get<TTree>(fTreeNames[in].c_str()));
                CheckTreeExists(fTrees[run], in);
            }
            else // add as friend!
                fTrees[run]->AddFriend(fTreeNames[in].c_str(), filename.c_str());
        }
    }
}

void ActRoot::InputData::InitChain(const std::set<int>& runs)
{
    fRuns = runs;
    // Assert that we have at least 1 input
    if(fTreeNames.size() < 1)
        throw std::runtime_error("InputData::InitChain(): size of internal vectors < 1 -> No inputs to read!");
    for(int in = 0; in < fTreeNames.size(); in++)
    {
        fChain = std::make_shared<TChain>(fTreeNames[in].c_str());
        for(const auto& run : runs)
        {
            std::string filename {fPaths[in] + fBegins[in] + TString::Format("%04d", run) + fEnds[in] + ".root"};
            CheckFileExists(filename);
            // Print! (disabled for chain)
            // std::cout << BOLDYELLOW << "InputData: reading " << fTreeNames[in] << " to chain in file " << '\n';
            // std::cout << "  " << filename << RESET << '\n';
            // Init
            if(in == 0)
            {
                fChain->Add(filename.c_str());
            }
            else // add as friend!
                fChain->AddFriend(fTreeNames[in].c_str(), filename.c_str());
        }
    }
}

void ActRoot::InputData::GetEntry(int run, int entry)
{
    fTrees[run]->GetEntry(entry);
}

void ActRoot::InputData::AddManualEntries(const std::string& file)
{
    if(file.length() == 0)
        return;
    // Read file
    std::ifstream streamer {file.c_str()};
    if(!streamer)
        throw std::runtime_error("InputData::AddManualEntries(): could not open file " + file);
    std::cout << BOLDGREEN << "InputData::AddManualEntries(): from file " + file << RESET << '\n';
    int run {};
    int entry {};
    while(streamer >> run >> entry)
    {
        // Check if run is in list
        bool isKnown {fFiles.count(run) > 0};
        if(!isKnown)
            continue;
        // Add to map
        fManualEntries[run].push_back(entry);
    }
    streamer.close();
    // Sort them in increasing order of entry
    for(auto& [run, vec] : fManualEntries)
        std::sort(vec.begin(), vec.end());
}

void ActRoot::InputData::Close(int run)
{
    // Reset with use_count = 1 calls destructor, which for TFile calls Close()
    fTrees[run].reset();
    fFiles[run].reset();
}

std::string ActRoot::InputData::SolveRelativePath(const std::string& path)
{
    // If it is relative then search for a configs/ subdir where to locate the data.conf
    auto cwd {std::filesystem::current_path()};
    std::filesystem::path configs {};
    // Up three dir levels
    for(int i = 0; i < 3; i++)
    {
        auto base {cwd};
        for(int j = 0; j < i; j++)
        {
            base = base.parent_path();
        }
        auto candidate {base / "configs"};
        if(std::filesystem::exists(candidate) && std::filesystem::is_directory(candidate))
            configs = candidate;
    }
    // Once located, go to project's home directory, where relative paths in data.conf are referred to
    auto home {configs.parent_path()};
    // Then combine with current path
    auto sum {home / path};
    sum = sum.lexically_normal();
    return sum;
}
