#include "ActRunManager.h"
#include "TString.h"

#include "ActParameters.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <filesystem>
#include <vector>

void ActRunManager::ReadFile(const std::string &fileName)
{
    std::ifstream streamer {fileName.c_str()};
    if(!streamer)
    {
        throw std::runtime_error("Error reading PileUp configuration from file!");
    }
    std::string line {};
    int row {0};
    std::vector<int> localRuns;
    while(std::getline(streamer, line, '\n'))
    {
        std::istringstream lineStreamer {line};
        std::string value {};
        int column {0};
        while(std::getline(lineStreamer,value, ' '))
        {
            if(column == 1)
            {
                //std::cout<<"Row: "<<row<<" Column: "<<column<<" Content: "<<value<<'\n';
                switch (row)
                {
                case 0://first line
                    if(TString(value).Contains("..."))
                    {
                        auto init = value.find_first_of(",");
                        auto end  = value.find_last_of(",");
                        int initRun = std::stoi(value.substr(0, init));
                        int endRun  = std::stoi(value.substr(end + 1));
                        //std::cout<<"InitRun: "<<initRun<<" EndRun: "<<endRun<<'\n';
                        for(unsigned int run = initRun; run <= endRun; run++)
                        {
                            if(isInVector(run, fRuns))
                            {
                                throw std::runtime_error(("Run " + std::to_string(run) + " repeaded in file " + fileName));
                            }
                            fRuns.push_back(run);
                            localRuns.push_back(run);
                        }
                    }
                    else
                    {
                        std::istringstream valueStreamer {value};
                        std::string runString {};
                        while(std::getline(valueStreamer, runString, ','))
                        {
                            fRuns.push_back(std::stoi(runString));
                            localRuns.push_back(std::stoi(runString));
                        }
                    }
                    break;
                case 1://PileUp file
                    for(const auto& run : localRuns)
                        fPileUpFile[run] = value;
                    break;
                case 2://Auto Drift file
                    for(const auto& run : localRuns)
                        fAutoDriftFile[run] = value;
                    break;
                case 3://MANUAL drift file
                    for(const auto& run : localRuns)
                        fManualDriftFile[run] = value;
                    break;
                case 4://PID uncorrected
                    for(const auto& run : localRuns)
                        fPIDUncorrectedFile[run] = value;
                    break;
                case 5:
                    for(const auto& run : localRuns)
                        fPIDCorrectedFile[run] = value;
                    break;
                case 6:
                    for(const auto& run : localRuns)
                        fPIDCorrectionSlope[run] = std::stod(value);
                default:
                    break;
                }
            }
            column++;
        }
        row++;
    }
}

void ActRunManager::ReadDir(const std::string &path)
{
    std::cout<<BOLDCYAN<<"==== Initializing ActRunManager in path "<<path <<" ===="<<RESET<<'\n';
    for(const auto& file : std::filesystem::directory_iterator(path))
    {
        if(!TString(file.path()).Contains(".dat"))
            continue;
        std::cout<<BOLDCYAN<<"Reading file: "<<file.path()<<RESET<<'\n';
        ReadFile(file.path());
    }
    std::cout<<BOLDCYAN<<"=============================="<<RESET<<'\n';
    std::cout<<std::endl<<std::flush;
}
