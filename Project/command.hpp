#ifndef COMMAND_HPP
#define COMMAND_HPP

#include <iostream>
#include <vector>
#include <sstream>
#include <map>

#include "filesystem.hpp"
#include "stringeval.hpp"
#include "options.hpp"
#include "macrospace.hpp"

using namespace std;

class MacroSpaceContainer;


bool runCommand(string str, MacroContainer& macroContainer, Options& configuration);

void dealWithUser(MacroSpaceContainer& macroContainer, Options& configuration);

#endif // COMMAND_HPP
