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

    CommandManager cmd;

    // Macro database (stored in volatile memory)
    MacroContainer macroContainer;

    // Options of the program
    Options configuration;

    // Let's start the command prompt
    cmd.dealWithUser();

    return 0;
}


