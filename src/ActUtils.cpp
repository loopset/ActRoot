#include "ActUtils.h"

#include <algorithm>
#include <cctype>

void ActUtils::CleanWhitespaces(std::string &str)
{
    str.erase(std::remove_if(str.begin(), str.end(),
                             [](unsigned char x){return std::isspace(x);}),
              str.end());
}
