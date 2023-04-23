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




int main()
{
    // Welcoming message
    std::cout << "WELCOME TO MACRO PARSER.\n";
    std::cout << "Type 'help' to see the available commands.\n" << std::endl;

    // let's show boolean values in plain letters, it's better :)
    std::cout << std::boolalpha;

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
        std::cout << "Fatal exception: " << ex.what() << std::endl;
        std::string ignorethis;
        std::getline(std::cin, ignorethis);
    }

    return 0;
}


