/**
  ******************************************************************************
  * @file    command.cpp
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

#include "command.hpp"

#include "container.hpp"
#include "stringeval.hpp"

CommandManager::CommandManager()
{}

CommandManager::CommandManager(const Options& options)
: configuration(options), macrospaces()
{}


static void printBasicHelp()
{
    cout << "\nBASIC COMMANDS:" << endl;
    cout << "- help [all?] : print basic/all commands" << endl;
    cout << "- importfile [file] : import macros from a file to the program" << endl;
    cout << "- importfolder [folder] : import all macros from all header files from a folder" << endl;
    cout << "- look [macro] : calculate the value of a macro given in input" << endl;
    cout << "- exit : quit the program" << endl;

    cout << "\nOnly basic commands were printed, to display the list of all commands, please type 'help all'." << endl;
}

static void printAdvancedHelp()
{
    cout << "\nBASIC COMMANDS:" << endl;
    cout << "- help [all?] : print basic/all commands" << endl;
    cout << "- importfile [file] : import macros from a file to the program" << endl;
    cout << "- importfolder [folder] : import all macros from all header files from a folder" << endl;
    cout << "- look [macro] : calculate the value of a macro given in input" << endl;
    cout << "- exit : quit the program" << endl;

    cout << "\nINFO COMMANDS (print informations):" << endl;
    cout << "- stat : print the number of macros imported" << endl;
    cout << "- where [macro] [folderpath] : look for files containing a macro definition inside a folder" << endl;
    cout << "- search [name] [...] : print all macros containing the string(s) given in their name" << endl;
    cout << "- list [all/ok/re/in] : print the list of all/okay/redefined/incorrect macros" << endl;

    cout << "\nADVANCED COMMANDS" << endl;
    cout << "- define [macro] [value] : add/replace a specific macro by a specific value" << endl;
    cout << "- interpret [macro] : look and choose among possible definitions for this macro" << endl;
    cout << "- evaluate [expr] : evaluate an expression that may contain macros, boolean values.." << endl;
    cout << "- options : display the options used for file import and string evaluation" << endl;
    cout << "- changeoption [name] [value] : change the parameter given to an option" << endl;
    cout << "- clear [all/ok/re/in] : empty the list of all/okay/redefined/incorrect macros" << endl;
    cout << "- cls : clear console" << endl;

    cout << "\nMACRO SPACES (to store macro in separate memory spaces) (to be implemented)" << endl;
    cout << "[command] [macrospace?]: to run the command in a macrospace, add macrospace at the end" << endl;
    cout << "- printsources [macrospace] : list the folders from which the list origins" << endl;
    cout << "- list spaces : list the macrospaces currently defined" << endl;
    cout << "msall is a macrospace that designate all the macrospaces unified." << endl;

    /*cout << "\nSPECIAL PARAMETERS (to be implemnted)" << endl;
    cout << "--alphaorder : show results in an alphabetical order" << endl;
    cout << "--increasing : show results in an increasing order" << endl;
    cout << "--decreasing : show results in a decreasing order" << endl;
    cout << "?: describes an optional paramater" << endl;*/
}

void CommandManager::dealWithUser()
{
    bool running=true;

    while(running)
    {
        cout << " > ";

        string userInput;
        getline(cin, userInput);

        running = runCommand(userInput);

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

static void extractList2(std::vector<std::string>& outputList, const std::string& initialString)
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

static void printStatMacrospace(MacroContainer const& mc)
{
    cout << mc.getDefines().size() << " macros were loaded." << endl;
    cout << "|-> " << mc.getRedefinedMacros().size() << " macros have been redefined." << endl;
    cout << "|-> " << mc.getIncorrectMacros().size() << " macros seem incorrect." << endl;
}



bool CommandManager::runCommand(string str)
{
    MacroContainer& macroContainer=getMacroSpace("default");

    // lower characters related to the name of the command
    for(unsigned i=0; i<str.size() && str[i]!=' '; ++i)
        str[i] = tolower(str[i]);

    if(str.empty())
        {}
    else if(str.substr(0,5) == "clear")
    {
        std::vector<std::string> parameters;
        std::vector<std::string> containersName;
        extractList(parameters, str.substr(5));

        bool fine=true;

        bool eraseRe=false;
        bool eraseIn=false;
        bool eraseOk=false;

        for(const std::string& param: parameters)
        {
            if(doesMacrospaceExists(param)){
                containersName.push_back(param);
            }
            else if(param=="ok")
                eraseOk=true;
            else if(param=="in")
                eraseIn=true;
            else if(param=="re")
                eraseRe=true;
            else if(param=="all")
                eraseOk=eraseIn=eraseRe=true;
            else {
                cout << "Incorrect option parameter '" << param << "'." << endl;
                fine = false;
            }
        }
        if(!eraseRe && !eraseOk && !eraseIn)
        {
            cout << "No option parameter was given. No macro was erased." << endl;
            cout << "Try 'clear all' or 'clear ok' or 'clear in'." << endl;
        }
        if(containersName.empty())
            containersName.emplace_back("default");

        // User message to explain what the program did
        std::cout << "Cleared ";
        if(eraseRe&&eraseOk&&eraseIn) cout << "all "; else cout << "some ";
        cout << "macros in container";
        if(containersName.size()>1) cout << 's';
        cout << ": ";
        for(const std::string& str: containersName) cout << str << " ";
        cout << endl;

        // Finally let's clear the macro database
        for(const std::string& str: containersName)
            getMacroSpace(str).clearDatabase(eraseOk, eraseRe, eraseIn);
    }
    else if(str == "help")
        printBasicHelp();
    else if(str == "helpall" || str == "help all")
        printAdvancedHelp();
    else if(str.substr(0,4) == "stat"){
        std::vector<std::string> parameters;
        extractList(parameters, str.substr(4));

        if(!parameters.empty()){
            if(doesMacrospaceExists(parameters.front()))
            {
                printStatMacrospace(getMacroSpace(parameters.front()));
            }
            else
            {
                cout << "The macrospace '"<< parameters.front() << "' does not exists." << endl;
            }
        }
        else {
            printStatMacrospace(macroContainer);
        }
    }

    else if(str.substr(0,4) == "list")
    {
        std::vector<std::string> parameters;
        extractList(parameters, str.substr(4));

        bool listRe=false;
        bool listIn=false;
        bool listOk=false;

        if(parameters.size()==1 && parameters.front()=="spaces")
        {
            for(const auto& p: macrospaces)
                std::cout << "- " << p.first << " => " << p.second.getDefines().size() << " macros." << endl;
        }
        else
        {

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

    }

    else if(str.substr(0,10) == "interpret ")
    {
        std::vector<std::string> parameters;
        extractList(parameters, str.substr(9));

        if(!parameters.empty())
        {
        string macroName = parameters[0];
        MacroContainer *mc=&macroContainer;
        if(parameters.size()>=2)
            mc = &(mc[1]);


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
                if(macrospaces.size()==1)
                    std::cout << "This macro does not seem to exist." << endl;
                else
                    std::cout << "This macro does not seem to exist in this macrospace." << endl;
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
                        mc->emplaceAndReplace(macroName, possibleValues[result-1]);
                    }
                    else if(result != 0)
                    {
                        cout << "The macro was not interpreted." << endl;
                    }
                }
            }
        }

        }
    }

    else if(str.substr(0, 11) == "importfile "){
        std::vector<std::string> parameters;
        extractList(parameters, str.substr(11));
        std::vector<std::string> directories;
        std::vector<std::string> macrospacesName;

        for(const std::string& str: parameters){
            if(str.find(":\\")!=std::string::npos){
                directories.emplace_back(str);
            }
            else if(str.find('/')!=std::string::npos && str.find('.')){
                directories[directories.size()-1] += std::string(" ")+str;
            }
            else {
                // Else we have to deal with a macrospace name !
                macrospacesName.emplace_back(str);
            }
        }

        if(macrospacesName.empty())
            macrospacesName.emplace_back("default");

        if(!directories.empty()){
            auto& curMacroSpace = getMacroSpace(macrospacesName.front());
            curMacroSpace.importFromFile(parameters.front(), configuration);
            printStatMacrospace(curMacroSpace);
        }
        else {
            cout << "/!\\ Error: can't open the file provided. /!\\" << endl;
        }
    }

    else if(str.substr(0, 13) == "importfolder "){
        std::vector<std::string>
        parameters;
        extractList(parameters, str.substr(13));
        std::vector<std::string> directories;
        std::vector<std::string> macrospacesName;

        for(const std::string& str: parameters){
            if(str.find(":\\")!=std::string::npos){
                directories.emplace_back(str);
            }
            else if(str.find('/')!=std::string::npos && str.find('.')){
                directories[directories.size()-1] += std::string(" ")+str;
            }
            else {
                // Else we have to deal with a macrospace name !
                macrospacesName.emplace_back(str);
            }
        }

        if(macrospacesName.empty())
            macrospacesName.emplace_back("default");

        //for(auto& str1:macrospacesName)cout << "- " << str1 << endl;

        if(!directories.empty()){
            auto& curMacroSpace = getMacroSpace(macrospacesName.front());
            curMacroSpace.importFromFolder(parameters.front(), configuration);
            printStatMacrospace(curMacroSpace);
        }
        else {
            cout << "/!\\ Error: can't open the directory:" << directories.front() << " /!\\" << endl;
        }
        return true;
    }

    else if(str.substr(0, 5) == "look ")
    {
        std::vector<std::string> parameters;
        extractList(parameters, str.substr(5));

        str=parameters.front();
        clearSpaces(str);

        if(macroContainer.countMacroName(str)>1)
        {
            std::cout << "This macro has been redefined and needs to be interpreted." << endl;
            std::cout << "Type 'interpret " << str << "'." << endl;
        }
        else if(parameters.size()>1 && !doesMacrospaceExists(parameters[1]))
            cout << "The macrospace '" << parameters[1] << "' does not exist." << endl;
        else
        {
            MacroContainer *mc=nullptr;
            if(parameters.size()>1){
                mc = &(getMacroSpace(parameters[parameters.size()-1]));
            }
            else {
                mc = &macroContainer;
            }
            if(mc)
            {

            string userInput = str;

            bool found = false;

            for(auto& p : mc->getDefines())
            {
                if(p.first == userInput)
                {
                    cout << "first definition: " << p.second << endl;
                    string output = p.second;
                    auto status = calculateExpression(output, *mc, configuration);
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
            else
                std::cout << "The macrospace '" << parameters.back() << "' does not exist." << endl;
        }
    }
    else if(str.substr(0,7) == "define " && str.size()>=8){
        std::vector<std::string> parameters;
        extractList(parameters, str.substr(7));

        if(parameters.empty())
            std::cout << "Error: no parameter was given to the define command." << endl;
        else
        {
            string str2;
            if(parameters.size()>=2)
                str2 = parameters[1];
            if(!doesExprLookOk(str2))
                cout << "/!\\ Warning: the expression of the macro doesn't look correct. /!\\" << endl;

            string macroStringName="default";
            if(parameters.size()>=3)
                macroStringName = parameters[2];
            getMacroSpace(macroStringName).emplaceAndReplace(parameters.front(), str2);
        }
    }
    else if(str.substr(0,7) == "search " && str.size()>8){
        std::vector<std::string> wordsToFind;
        std::vector<MacroContainer*> macrospacesCur;
        extractList(wordsToFind, str.substr(7));

        // extract macrospaces names
        if(!wordsToFind.empty())
        {
            std::cout << "search launched in macrocontainer(s): ";

            for(auto it = wordsToFind.begin(); it!=wordsToFind.end();){
                if(doesMacrospaceExists(*it)){
                    std::cout << *it << " ,";
                    macrospacesCur.emplace_back(&(getMacroSpace(*it)));
                    it = wordsToFind.erase(it);
                }
                else
                    ++it;
            }

            if(macrospacesCur.empty()){
                cout << "default";
                macrospacesCur.emplace_back(&macroContainer);
            }
            cout << endl;


            if(!macrospacesCur.empty())
            {
                for(MacroContainer* mc: macrospacesCur){
                    mc->searchKeywords(wordsToFind, std::cout);
                }
            }
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

        // Extract a macrospace from the command
        std::vector<std::string> parameters;
        extractList(parameters, str.substr(9));
        std::vector<std::string> macroSpaceNames;
        if(!parameters.empty())
        {
            for(const std::string& str1: parameters) {
                if(doesMacrospaceExists(str1)){
                    macroSpaceNames.push_back(str1);
                    simpleReplace(expr, str1, "");
                }
            }
        }
        MacroContainer *mc=&(macroContainer);
        if(!macroSpaceNames.empty())
            mc=&(getMacroSpace(macroSpaceNames.front()));

        auto status = calculateExpression(expr, *mc, configuration);

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
    else if(str.substr(0,13) == "printsources "){
        string macrospaceName = str.substr(13);
        if(doesMacrospaceExists(macrospaceName)){
            getMacroSpace(macrospaceName).printOrigins();
        }
        else {
            cout << "The macrospace '" << macrospaceName << "' does not seem to exist." << endl;
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

bool CommandManager::runCommand2(string str)
{
    // Extract command and parameters from str
    std::vector<std::string> parameters;
    extractList(parameters, str);
    if(str.empty())
        return true;
    std::string& commandStr = parameters[0];
    lowerString(commandStr);

    // Shortcut to the default macrospace
    MacroContainer& mcc = getMacroSpace("default");


    if(commandStr=="where")
    {
        if(parameters.size()<3)
        {
            cout << "Error: incorrect parameters." << endl;
        }
        else
        {
            if(!searchDirectory(parameters[2], parameters[1], configuration))
            {
                std::cout << "Can't open the directory given as the second parameter." << endl;
            }
        }
    }
    else if(commandStr == "evaluate")
    {
        string expr = str.substr(9);


        // Extract a macrospace from the command
        std::vector<std::string> parameters;
        extractList(parameters, str.substr(9));
        std::vector<std::string> macroSpaceNames;
        if(!parameters.empty())
        {
            for(const std::string& str1: parameters) {
                if(doesMacrospaceExists(str1)){
                    macroSpaceNames.push_back(str1);
                    simpleReplace(expr, str1, "");
                }
            }
        }
        MacroContainer *mc=&(mcc);
        if(!macroSpaceNames.empty())
            mc=&(getMacroSpace(macroSpaceNames.front()));

        auto status = calculateExpression(expr, *mc, configuration);

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
    else if(commandStr == "printsources"){
        string macrospaceName = str.substr(13);
        if(doesMacrospaceExists(macrospaceName)){
            getMacroSpace(macrospaceName).printOrigins();
        }
        else {
            cout << "The macrospace '" << macrospaceName << "' does not seem to exist." << endl;
        }
    }
    else if(commandStr == "cls"){
        system("cls");
    }
    else if(commandStr == "exit")
        return false;
    else {
        cout << "This command is unknown." << endl;
    }

    return true;
}

void CommandManager::addMacroSpace(const std::string& macrospaceName, const MacroContainer& macrospace)
{
    macrospaces.emplace_back(macrospaceName, macrospace);
}

MacroContainer& CommandManager::getMacroSpace(const std::string& macrospaceName)
{
    for(auto& p: macrospaces){
        if(p.first == macrospaceName)
            return p.second;
    }

    macrospaces.emplace_back(macrospaceName, MacroContainer());
    MacroContainer* kkk=nullptr;
    for(auto& p: macrospaces)
        kkk = &(p.second);
    return *kkk;
}

bool CommandManager::doesMacrospaceExists(const std::string& macrospaceName) const
{
    for(const auto& p: macrospaces){
        if(p.first == macrospaceName)
            return true;
    }

    return false;
}

