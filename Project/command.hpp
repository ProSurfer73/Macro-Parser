#ifndef COMMAND_HPP
#define COMMAND_HPP

#include <iostream>
#include <vector>
#include <sstream>
#include <map>

#include "filesystem.hpp"
#include "stringeval.hpp"
#include "options.hpp"

using namespace std;

struct MacroContainer
{
    // Set large default presize
    // avoid reallocating small amount of memory each time
    MacroContainer()
    {
        defines.reserve(10000);
        redefinedMacros.reserve(1000);
        incorrectMacros.reserve(1000);
    }

    // Attributes
    std::vector< std::pair< std::string, std::string> > defines;
    std::vector< std::string > redefinedMacros;
    std::vector< std::string > incorrectMacros;
};


bool runCommand(string str, MacroContainer& macroContainer, Options& configuration);

void dealWithUser(MacroContainer& macroContainer, Options& configuration);

#endif // COMMAND_HPP
