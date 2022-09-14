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
    std::cout << "WELCOME TO MACRO PARSER.\n";
    std::cout << "Type 'help' to see the available commands.\n" << endl;

    try {

    // Command line manager
    CommandManager cmd;

    // load boot script
    if(cmd.loadScript("boot.txt"))
        std::cout << std::endl;

    // Let's start the command prompt
    cmd.dealWithUser();

    }
    catch(std::exception const& ex)
    {
        std::cout << "Fatal exception: " << ex.what() << endl;
    }

    return 0;
}


