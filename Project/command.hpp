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
#include <vector>
#include <string>
#include "stringeval.hpp"
#include "options.hpp"


struct CommandManager
{

    CommandManager();
    bool runCommand(std::string str);
    void dealWithUser();


    Options configuration;
    MacroContainerr macroContainer;
};

#endif // COMMAND_HPP
