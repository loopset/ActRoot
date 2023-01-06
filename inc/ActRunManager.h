#ifndef ACTRUNMANAGER_H
#define ACTRUNMANAGER_H

#include <algorithm>
#include <map>
#include <string>
#include <vector>
class ActRunManager
{
public:
    std::vector<unsigned int> fRuns;
    std::map<unsigned int, std::string> fPileUpFile;
    std::map<unsigned int, std::string> fAutoDriftFile;
    std::map<unsigned int, std::string> fManualDriftFile;
    std::map<unsigned int, std::string> fPIDUncorrectedFile;
    std::map<unsigned int, std::string> fPIDCorrectedFile;
    std::map<unsigned int, double> fPIDCorrectionSlope;
public:
    ActRunManager() = default;
    ~ActRunManager() = default;

    void ReadFile(const std::string& fileName);

    void ReadDir(const std::string& path);

private:
    inline std::string RemoveWhiteSpaces(std::string input)
    {
        input.erase(std::remove_if(input.begin(), 
                                   input.end(),
                                   [](unsigned char x){ return std::isspace(x); }),
                    input.end());
        return input;
    }
    template<typename T>
    static inline bool isInVector(T val, std::vector<T> vec)
	{
		if (vec.size() == 0)
			return false;
		return std::find(vec.begin(), vec.end(), val) != vec.end();
	}
};

#endif
