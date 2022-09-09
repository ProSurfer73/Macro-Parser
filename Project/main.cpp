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

    try {

    // Command line manager
    CommandManager cmd;

    // Let's start the command prompt
    cmd.dealWithUser();

    }
    catch(std::exception const& ex)
    {
        std::cout << "Fatal exception: " << ex.what() << endl;
    }

    return 0;
}


