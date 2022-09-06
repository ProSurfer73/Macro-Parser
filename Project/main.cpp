/**
  ******************************************************************************
  * @file    main.cpp
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

#include <iostream>
#include <fstream>

#include "hexa.hpp"
#include "stringeval.hpp"
#include "command.hpp"
#include "options.hpp"

using namespace std;




int main()
{
    // Welcoming message
    std::cout << "WELCOME TO MACRO PARSER." << endl;
    std::cout << "Type 'help' to see the available commands.\n" << endl;

    // Macro database (stored in volatile memory)
    MacroContainer macroContainer;

    // Options of the program
    Options configuration;

    // Let's start the command prompt
    dealWithUser(macroContainer, configuration);

    return 0;
}


