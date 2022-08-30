#ifndef COMMAND_HPP
#define COMMAND_HPP

#include <iostream>
#include <vector>
#include <sstream>
#include <map>

#include "filesystem.hpp"
#include "stringeval.hpp"
#include "options.hpp"
#include "container.hpp"

using namespace std;




bool runCommand(string str, MacroContainer& macroContainer, Options& configuration);

void dealWithUser(MacroContainer& macroContainer, Options& configuration);

#endif // COMMAND_HPP
