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
    cout << "- search [name] [...] : print all macros containing the string(s) given in their name" << endl;
    cout << "- list [all/ok/re/in] : print the list of all/okay/redefined/incorrect macros" << endl;
    cout << "- options : display the options used for file import and string evaluation" << endl;
    cout << "- changeoption [name] [value] : change an option name" << endl;
    cout << "- clear [all/ok/re/in] : empty the list of all/okay/redefined/incorrect macros" << endl;
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

static void removeFromVector(std::vector<std::string>& v, const std::string& str)
{
    for(auto it=v.begin(); it!=v.end();)
    {
        if(*it == str)
            it = v.erase(it);
        else
            ++it;
    }
}



bool runCommand(string str, MacroContainer& macroContainer, Options& configuration)
{
    // lower characters related to the name of the command
    for(unsigned i=0; i<str.size() && str[i]!=' '; ++i)
        str[i] = tolower(str[i]);

    if(str.empty())
        {}
    else if(str == "clearall" || str == "clear all"){
        macroContainer.defines.clear();
        macroContainer.redefinedMacros.clear();
        macroContainer.incorrectMacros.clear();
    }
    else if(str == "clearre" || str == "clear re"){
        macroContainer.redefinedMacros.clear();
    }
    else if(str == "clearin" || str == "clear in"){
        macroContainer.incorrectMacros.clear();
    }
    else if(str == "clearok" || str =="clear ok"){
        // Delete all macros that are at the smae time not contained in the incorrect and redefined lists
        for(auto it=macroContainer.defines.begin(); it!=macroContainer.defines.end();){
            if(std::find(macroContainer.incorrectMacros.begin(), macroContainer.incorrectMacros.end(), it->first) == macroContainer.incorrectMacros.end()
            && std::find(macroContainer.redefinedMacros.begin(), macroContainer.redefinedMacros.end(), it->first) == macroContainer.redefinedMacros.end())
                it=macroContainer.defines.erase(it);
            else
                ++it;
        }
    }
    else if(str == "help")
        printHelp();
    else if(str == "stat"){
        cout << macroContainer.defines.size() << " macros were loaded." << endl;
        cout << "|-> " << macroContainer.redefinedMacros.size() << " macros have been redefined." << endl;
        cout << "|-> " << macroContainer.incorrectMacros.size() << " macros seem incorrect." << endl;
    }
    else if(str == "list"){
        cout << "The command 'list' does not exist." << endl;
        cout << "Did you mean listok ? listre ? listin ?" << endl;
    }
    else if(str == "listall" || str == "list all"){
        unsigned nbPrinted=0;
        for(const auto& p: macroContainer.defines){
            cout << p.first << " => " << p.second << endl;
            if(++nbPrinted >= 1000){
                cout << "/!\\ Only printed the 1000 first results /!\\" << endl;
                break;
            }
        }
    }

    // List only the macros that are at the same time not incorrect and not redefined
    else if(str == "listok" || str == "list ok"){
        for(const auto& p: macroContainer.defines){
            if(std::find(macroContainer.incorrectMacros.begin(), macroContainer.incorrectMacros.end(), p.first) == macroContainer.incorrectMacros.end()
            && std::find(macroContainer.redefinedMacros.begin(), macroContainer.redefinedMacros.end(), p.first) == macroContainer.redefinedMacros.end())
                std::cout << p.first << " => " << p.second << endl;
        }
    }
    else if(str == "listre" || str == "list re"){
        for(const string& str: macroContainer.redefinedMacros){
            cout << " - " << str;
            for(const pair<string,string>& p: macroContainer.defines){
                if(p.first == str){
                    cout << " => " << p.second;
                    break;
                }
            }
            cout << endl;
        }
    }
    else if(str == "listin" || str == "list in"){
        for(const string& str: macroContainer.incorrectMacros){
            cout << " - " << str;
            for(const pair<string,string>& p: macroContainer.defines){
                if(p.first == str){
                    cout << " => " << p.second;
                    break;
                }
            }
            cout << endl;
        }
    }
    else if(str.substr(0, 11) == "importfile "){
        if(!readFile(str.substr(11), macroContainer))
            cout << "/!\\ Error: can't open the given file. /!\\" << endl;
        else {
            runCommand("stat", macroContainer, configuration);
            return true;
        }
    }

    else if(str.substr(0, 13) == "importfolder "){
        readDirectory(str.substr(13), macroContainer, configuration.doesImportOnlySourceFileExtension());
        runCommand("stat", macroContainer, configuration);
        return true;
    }

    else if(str.substr(0, 5) == "look ")
    {
        if(macroContainer.defines.empty())
            cout << "No macros were imported yet." << endl;
        else
        {
            string userInput = str.substr(5);

            bool found = false;

            for(auto& p : macroContainer.defines)
            {
                if(p.first == userInput)
                {
                    cout << "first definition: " << p.second << endl;
                    string output = p.second;
                    bool displayPbInfo = false;
                    bool okay = calculateExpression(output, macroContainer, displayPbInfo, configuration);
                    if(!okay)
                        cout << "/!\\ The expression can't be calculated. /!\\\n";
                    if(displayPbInfo){
                        cout << "possible output: " << output << "??? ";
                                            }
                    else
                        cout << "output: " << output;
                    if(okay){
                        string hexaRepresentation;
                        try { hexaRepresentation = convertDeciToHexa(std::stoi(output)); }
                        catch(const std::exception& ex){}
                        if(!hexaRepresentation.empty())
                            cout << " (hexa: 0x" << hexaRepresentation << ')';
                    }
                    cout << endl;
                    if(displayPbInfo){
                        cout << "\nIt seems that you are using macros that seem incorrect or have been redefined. The output can't be trusted." << endl;
                        cout << "You can look for the values available for that specific macro by typing 'search [macro]'." << endl;
                        cout << "And finally you address the issue by correcting its value by typing 'define [macro] [value]'." << endl;
                    }

                    found=true;
                    break;
                }
            }

            if(!found)
                cout << "No macro was found with this name." << endl;
        }
    }
    else if(str.substr(0,7) == "define " && str.size()>=10){
        std::istringstream ss(str.substr(6));
        string str1;
        ss >> str1;
        string str2;
        if(ss.tellg() != (-1))
            str2=str.substr(6+ss.tellg());
        if(!doesExprLookOk(str2))
            cout << "/!\\ Warning: the expression of the macro doesn't look correct. /!\\" << endl;
        macroContainer.emplaceAndReplace(str1, str2);
    }
    else if(str.substr(0,7) == "search " && str.size()>8){
        std::vector<std::string> wordsToFind;
        extractList(wordsToFind, str.substr(7));

        for(const auto& p: macroContainer.defines)
        {
            bool okay=true;
            for(const auto& s: wordsToFind){
                if(p.first.find(s) == std::string::npos)
                    okay=false;
            }
            if(okay)
                cout << " - " << p.first << " => " << p.second << '\'' << endl;
        }
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
    else if(str == "cls"){
        system("cls");
        return true;
    }
    else if(str == "exit")
        return false;
    else {
        cout << "This command is unknown." << endl;
    }

    cout << endl;

    return true;
}



