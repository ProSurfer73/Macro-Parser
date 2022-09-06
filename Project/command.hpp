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

#include "filesystem.hpp"
#include "stringeval.hpp"
#include "options.hpp"
#include "container.hpp"

using namespace std;




bool runCommand(string str, MacroContainer& macroContainer, Options& configuration);

void dealWithUser(MacroContainer& macroContainer, Options& configuration);

#endif // COMMAND_HPP
