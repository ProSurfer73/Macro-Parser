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
    MacroSpaceContainer macroSpaceContainer;

    // Options of the program
    Options configuration;

    // Let's start the command prompt
    dealWithUser(macroSpaceContainer, configuration);

    return 0;
}


