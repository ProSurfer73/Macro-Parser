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
    CommandManager(const Options& options, const MacroDatabase& database);

    // Main methods
    void dealWithUser();
    bool runCommand(string str);

private:
    Options configuration;
    MacroContainer macroContainer;
};



#endif // COMMAND_HPP
