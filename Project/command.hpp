/**
  ******************************************************************************
  * @file    command.hpp
  * @author  MCD Application Team
  * @brief   Macro-Parser
  *
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

#ifndef COMMAND_HPP
#define COMMAND_HPP

#include <iostream>
#include <vector>
#include <sstream>
#include <map>

using namespace std;

#include "container.hpp"

class CommandManager
{
public:
    // Constructors
    CommandManager();
    CommandManager(const Options& options);

    // Command line methods
    void dealWithUser();
    bool runCommand(string str);

    // Database command
    void addMacroSpace(const std::string& macrospaceName, const MacroContainer& macrospace);
    MacroContainer& getMacroSpace(const std::string& macrospaceName);

private:
    Options configuration;
    MacroContainer macroContainer;
    std::vector< std::pair<std::string, MacroContainer> > macrospaces;
};



#endif // COMMAND_HPP
