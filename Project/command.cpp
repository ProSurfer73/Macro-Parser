#include "command.hpp"

static void printHelp()
{
    cout << "Here are the available commands:" << endl;
    cout << "- help : print this menu" << endl;
    cout << "- importfile [file] : import macros from a file to the program" << endl;
    cout << "- importfolder [folder] : import all macros from all header files from a folder" << endl;
    cout << "- stat : print the number of macros imported" << endl;
    cout << "- look [macro] : calculate the value of a macro given in input" << endl;
    cout << "- define [macro] [value] : add/replace a specific macro" << endl;
    cout << "- interpret [macro] : look and choose among possible definitions for this macro" << endl;
    cout << "- evaluate [expr] : evaluate an expression that may contain macros, boolean values.." << endl;
    cout << "- search [name] [...] : print all macros containing the string(s) given in their name" << endl;
    cout << "- list [all/ok/re/in] : print the list of all/okay/redefined/incorrect macros" << endl;
    cout << "- options : display the options used for file import and string evaluation" << endl;
    cout << "- changeoption [name] [value] : change an option name" << endl;
    cout << "- clear [all/ok/re/in] : empty the list of all/okay/redefined/incorrect macros" << endl;
    cout << "- where [macro] [folderpath] : look for a macro definition inside a folder" << endl;
    cout << "- cls : clear console" << endl;
    cout << "- exit : quit the program" << endl;
}

void dealWithUser(MacroContainer& macroContainer, Options& configuration)
{
    bool running=true;

    while(running)
    {
        cout << " > ";

        string userInput;
        getline(cin, userInput);

        running = runCommand(userInput, macroContainer, configuration);

        cout << endl;
    }
}

static void extractList(std::vector<std::string>& outputList, const std::string& initialString)
{
    istringstream iss(initialString);
    string str;

    while(iss >> str)
    {
        outputList.emplace_back(str);
    }
}

static bool isAllDigits(const std::string& str)
{
    for(char c: str)
    {
        if(!isdigit(c))
            return false;
    }
    return true;
}



bool runCommand(string str, MacroContainer& macroContainer, Options& configuration)
{
    // lower characters related to the name of the command
    for(unsigned i=0; i<str.size() && str[i]!=' '; ++i)
        str[i] = tolower(str[i]);

    if(str.empty())
        {}
    else if(str.substr(0,5) == "clear")
    {
        std::vector<std::string> parameters;
        extractList(parameters, str.substr(5));

        bool eraseRe=false;
        bool eraseIn=false;
        bool eraseOk=false;

        for(const std::string& param: parameters)
        {
            if(param=="ok")
                eraseOk=true;
            else if(param=="in")
                eraseIn=true;
            else if(param=="re")
                eraseRe=true;
            else if(param=="all")
                eraseOk=eraseIn=eraseRe=true;
            else {
                cout << "Incorrect option parameter '" << param << "'." << endl;
            }
        }
        if(!eraseRe && !eraseOk && !eraseIn)
        {
            cout << "No option parameter was given. No list was erased." << endl;
            cout << "Try 'clear all' or 'clear ok' or 'clear in'." << endl;
        }
        macroContainer.clearDatabase(eraseOk, eraseRe, eraseIn);
    }
    else if(str == "help")
        printHelp();
    else if(str == "stat"){
        cout << macroContainer.getDefines().size() << " macros were loaded." << endl;
        cout << "|-> " << macroContainer.getRedefinedMacros().size() << " macros have been redefined." << endl;
        cout << "|-> " << macroContainer.getIncorrectMacros().size() << " macros seem incorrect." << endl;
    }

    else if(str.substr(0,4) == "list")
    {
        std::vector<std::string> parameters;
        extractList(parameters, str.substr(4));

        bool listRe=false;
        bool listIn=false;
        bool listOk=false;

        for(const std::string& param: parameters)
        {
            if(param=="ok")
                listOk=true;
            else if(param=="in")
                listIn=true;
            else if(param=="re")
                listRe=true;
            else if(param=="all")
                listOk=listIn=listRe=true;
            else {
                cout << "Incorrect option parameter '" << param << "'." << endl;
            }
        }
        if(!listRe && !listOk && !listIn)
        {
            cout << "No option parameter was given. No list was erased." << endl;
            cout << "Try 'list all' or 'list ok' or 'list in' instead." << endl;
        }

        unsigned nbPrinted=0;

        for(const auto& p : macroContainer.getDefines())
        {
            if(listRe)
            {
                for(const string& str: macroContainer.getRedefinedMacros())
                {
                    if(p.first == str){
                        cout << " - " << p.first << " => " << p.second << endl;
                        ++nbPrinted;
                        continue;
                    }
                }
            }

            if(listIn)
            {
                for(const string& str: macroContainer.getIncorrectMacros())
                {
                    if(p.first == str){
                        cout << " - " << p.first << " => " << p.second << endl;
                        ++nbPrinted;
                        continue;
                    }
                }
            }

            if(listOk)
            {
                if(std::find(macroContainer.getIncorrectMacros().begin(), macroContainer.getIncorrectMacros().end(), p.first)==macroContainer.getIncorrectMacros().end()
                && std::find(macroContainer.getRedefinedMacros().begin(), macroContainer.getRedefinedMacros().end(), p.first)==macroContainer.getRedefinedMacros().end())
                {
                    cout << " - " << p.first << " => " << p.second << endl;
                    ++nbPrinted;
                    continue;
                }
            }

            if(nbPrinted >= 5000)
            {
                std::cout << "Only printed the first 5000 results." << endl;
                break;
            }
        }

    }

    else if(str.substr(0,10) == "interpret ")
    {
        string macroName = str.substr(10);
        clearSpaces(macroName);

        if(macroName.empty())
        {
            cout << "You need to specify a existing macro name to this command." << endl;
        }
        else
        {
            std::vector<std::string> possibleValues;

            for(const auto& p: macroContainer.getDefines())
            {
                if(p.first == macroName)
                {
                    possibleValues.emplace_back(p.second);
                }
            }

            if(possibleValues.empty())
            {
                std::cout << "This macro does not seem to exist." << endl;
            }
            else if(possibleValues.size()==1)
            {
                std::cout << "No need to interpret this macro." << endl;
                std::cout << "This macro has already one definition." << endl;
            }
            else
            {
                std::cout << "Type the number definition you would like to apply to this macro:" << endl;

                for(unsigned i=0; i<possibleValues.size(); ++i)
                {
                    std::cout << i+1 << ". \"" << possibleValues[i] << '\"' << endl;
                }

                std::cout << "0. Cancel, don't interpret this macro." << endl;

                getline(cin, str);

                if(isAllDigits(str))
                {
                    int result = std::atoi(str.c_str());
                    //cout << "result= " << result << endl;

                    if(result > static_cast<int>(possibleValues.size()))
                    {
                        cout << "Number out of range." << endl;
                    }
                    else if(result > 0)
                    {
                        macroContainer.emplaceAndReplace(macroName, possibleValues[result-1]);
                    }
                    else if(result != 0)
                    {
                        cout << "The macro was not interpreted." << endl;
                    }
                }
            }
        }
    }

    else if(str.substr(0, 11) == "importfile "){
        if(!importFile(str.substr(11), macroContainer, configuration))
            cout << "/!\\ Error: can't open the given file. /!\\" << endl;
        else {
            runCommand("stat", macroContainer, configuration);
        }
    }

    else if(str.substr(0, 13) == "importfolder "){
        if(!importDirectory(str.substr(13), macroContainer, configuration))
            cout << "/!\\ Error: can't open that directory. /!\\" << endl;
        runCommand("stat", macroContainer, configuration);
        return true;
    }

    else if(str.substr(0, 5) == "look ")
    {
        str=str.substr(5);
        clearSpaces(str);

        if(macroContainer.countMacroName(str)>1)
        {
            std::cout << "This macro has been redefined and needs to be interpreted." << endl;
            std::cout << "Type 'interpret " << str << "'." << endl;
        }
        else if(macroContainer.getDefines().empty())
            cout << "No macros were imported yet." << endl;
        else
        {
            string userInput = str;

            bool found = false;

            for(auto& p : macroContainer.getDefines())
            {
                if(p.first == userInput)
                {
                    cout << "first definition: " << p.second << endl;
                    string output = p.second;
                    auto status = calculateExpression(output, macroContainer, configuration);
                    if(status == CalculationStatus::EVAL_ERROR)
                        cout << "/!\\ The expression can't be calculated. /!\\" << endl;
                    if(status == CalculationStatus::EVAL_WARNING){
                        cout << "possible output: " << output << "??? ";
                    }
                    else
                        cout << "output: " << output;
                    if(status==CalculationStatus::EVAL_OKAY || status==CalculationStatus::EVAL_WARNING){
                        std::string hexaRepresentation;
                        try { hexaRepresentation = convertDeciToHexa(std::stoi(output)); }
                        catch(const std::exception& ex){}
                        if(!hexaRepresentation.empty())
                            cout << " (hexa: 0x" << hexaRepresentation << ')';
                    }
                    if(status == CalculationStatus::EVAL_WARNING){
                        cout << "\n\nIt seems that you are using macros that seem incorrect or have been redefined." << endl;
                        cout << "The output can't be trusted." << endl;
                        cout << "To fix a specific macro: please type 'interpret [macro]'." << endl;
                    }
                    if(status == CalculationStatus::EVAL_ERROR ||status == CalculationStatus::EVAL_OKAY)
                        cout << endl;

                    found=true;
                    break;
                }
            }

            if(!found)
                cout << "No macro was found with this name '" << str << "'." << endl;
        }
    }
    else if(str.substr(0,7) == "define " && str.size()>=8){
        std::istringstream ss(str.substr(6));
        string str1;
        ss >> str1;
        if(str1.empty())
            std::cout << "Error: no parameter was given to the define command." << endl;
        else
        {
            string str2;
            if(ss.tellg() != (-1))
                str2=str.substr(6+ss.tellg());
            if(!doesExprLookOk(str2))
                cout << "/!\\ Warning: the expression of the macro doesn't look correct. /!\\" << endl;
            macroContainer.emplaceAndReplace(str1, str2);
        }
    }
    else if(str.substr(0,7) == "search " && str.size()>8){
        std::vector<std::string> wordsToFind;
        extractList(wordsToFind, str.substr(7));
        macroContainer.searchKeywords(wordsToFind, std::cout);
    }
    else if(str == "options"){
        // Print the configuration
        configuration.toStream(cout);
        cout << "The options are saved in the file '" << OPTIONS_FILENAME << "' next to the executable." << endl;
    }
    else if(str.substr(0,12) == "changeoption"){
        istringstream ss(str.substr(12));
        string s1,s2;
        ss >> s1 >> s2;
        if(s1.empty()||s2.empty()){
            cout << "Error: Empty option parameter." << endl;
        }
        else {
            configuration.changeOption(s1,s2);
        }
    }
    else if(str.substr(0,6)=="where ")
    {
        stringstream ss(str.substr(6));
        string str1;

        if(!(ss>>str1) || ss.tellg()==(-1))
        {
            cout << "Error: incorrect parameters." << endl;
        }
        else
        {
            if(!searchDirectory(str.substr(ss.tellg()+6+1), str1, configuration))
            {
                std::cout << "Can't open the directory given as the second parameter." << endl;
                cout << str.substr(ss.tellg()+6+1) << endl;
            }
        }
    }
    else if(str.substr(0,9) == "evaluate ")
    {
        string expr = str.substr(9);

        auto status = calculateExpression(expr, macroContainer, configuration);

        if(status == CalculationStatus::EVAL_WARNING){
            std::cout << "possible ";
        }
        cout << "output: " << expr;

        if(status==CalculationStatus::EVAL_OKAY || status==CalculationStatus::EVAL_WARNING){
            std::string hexaRepresentation;
            try { hexaRepresentation = convertDeciToHexa(std::stoi(expr)); }
            catch(const std::exception& ex){}
            if(!hexaRepresentation.empty())
                cout << " (hexa: 0x" << hexaRepresentation << ')';
            cout << endl;
        }
        if(status == CalculationStatus::EVAL_WARNING)
        {
            cout << " ???" << endl;
            cout << "It seems that you are using macros that seem incorrect or have been redefined." << endl;
            cout << "The output can't be trusted." << endl;
            cout << "To fix a specific macro: please type 'interpret [macro]'." << endl;
        }
        else if(status == CalculationStatus::EVAL_ERROR)
        {
            cout << "\n/!\\ The expression can't be calculated. /!\\" << endl;
        }



    }
    #ifdef _WIN32 || _WIN64
    else if(str == "cls"){
        system("cls");
    }
    #endif
    else if(str == "exit")
        return false;
    else {
        cout << "This command is unknown." << endl;
    }

    return true;
}



