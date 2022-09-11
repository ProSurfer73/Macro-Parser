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

    cout << "\nMACRO SPACES (to store macro in separate memory spaces) (currently implemented)" << endl;
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

        running = runCommand2(userInput);

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
        if((str.find('/') != std::string::npos
        || str.find('\\') != std::string::npos
        || str.find('.') != std::string::npos)
        && outputList.size()>=2
        && (outputList.back().find('/') != std::string::npos
        || outputList.back().find('\\') != std::string::npos
        || outputList.back().find('.') != std::string::npos)){
            outputList[outputList.size()-1] += str;
        }
        else {
            outputList.emplace_back(str);
        }
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



bool CommandManager::runCommand2(string input)
{
    // Extract command and parameters from str
    std::vector<std::string> parameters;
    extractList2(parameters, input);
    if(input.empty())
        return true;
    std::string& commandStr = parameters[0];
    lowerString(commandStr);

    // Shortcut to the default macrospace
    MacroContainer& mcc = getMacroSpace("default");

    if(commandStr.substr(0,5) == "clear")
    {
        if(commandStr.size()>5)
            parameters.emplace_back(commandStr.substr(5));
        parameters.erase(parameters.begin());

        std::vector<std::string> containersName;

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
    else if(commandStr == "interpret")
    {
        parameters.erase(parameters.begin());

        if(!parameters.empty())
        {
        string macroName = parameters[0];
        MacroContainer *mc=nullptr;
        if(parameters.size()>=2)
            mc = &(getMacroSpace(parameters[1]));
        else
            mc = &(getMacroSpace("default"));


        if(macroName.empty())
        {
            cout << "You need to specify a existing macro name to this command." << endl;
        }
        else
        {
            std::vector<std::string> possibleValues;

            for(const auto& p: mc->getDefines())
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
                else{
                    // Here a macrospace has been given by parameter
                    if(doesMacrospaceExists(parameters[1]))
                        std::cout << "This macro does not seem to exist in this macrospace." << endl;
                    else
                        std::cout << "The macrospace '" << parameters[1] << "' does not exist." << endl;
                }

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

                std::string ss;
                getline(cin, ss);

                if(isAllDigits(ss))
                {
                    int result = std::atoi(ss.c_str());
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
    else if(parameters.front() == "look")
    {
        string str=parameters[1];

        /*if(macroContainer.countMacroName(str)>1)
        {
            std::cout << "This macro has been redefined and needs to be interpreted." << endl;
            std::cout << "Type 'interpret " << str << "'." << endl;
        }
        else*/ if(parameters.size()>2 && !doesMacrospaceExists(parameters[2]))
            cout << "The macrospace '" << parameters[1] << "' does not exist." << endl;
        else
        {
            MacroContainer *mc=nullptr;
            if(parameters.size()>=3){
                mc = &(getMacroSpace(parameters[parameters.size()-1]));
            }
            else {
                mc = &(getMacroSpace("default"));
            }
            if(mc != NULL)
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
    else if(commandStr == "define")
    {
        if(parameters.empty())
            std::cout << "Error: no parameter was given to the define command." << endl;
        else
        {
            string str2;
            if(parameters.size()>=3)
                str2 = parameters[2];
            if(!doesExprLookOk(parameters[1]))
                cout << "/!\\ Warning: the expression of the macro doesn't look correct. /!\\" << endl;

            string macroStringName="default";
            if(parameters.size()>=4)
                macroStringName = parameters[3];
            parameters.emplace_back();
            getMacroSpace(macroStringName).emplaceAndReplace(parameters[1], parameters[2]);
        }
    }
    else if(commandStr.substr(0,4) == "stat")
    {
        if(parameters.front().size()>4){
            parameters.emplace_back(commandStr.substr(4));

        }
        parameters.erase(parameters.begin());

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
            printStatMacrospace(getMacroSpace("default"));
        }
    }
    else if(commandStr == "search")
    {
        std::vector<MacroContainer*> macrospacesCur;
        parameters.erase(parameters.begin());

        // extract macrospaces names
        if(parameters.size()>=1)
        {
            std::cout << "search launched in macrocontainer(s): ";

            for(auto it = parameters.begin(); it!=parameters.end();){
                if(doesMacrospaceExists(*it)){
                    std::cout << *it << ", ";
                    macrospacesCur.emplace_back(&(getMacroSpace(*it)));
                    it = parameters.erase(it);
                }
                else
                    ++it;
            }

            if(macrospacesCur.empty()){
                cout << "default";
                macrospacesCur.push_back(&(getMacroSpace("default")));
            }
            cout << endl;

            for(const std::string& str3: parameters)
                std::cout << str3 << std::endl;


            if(!macrospacesCur.empty())
            {
                for(MacroContainer* mc: macrospacesCur){
                    mc->searchKeywords(parameters, std::cout);
                }
            }
        }
    }
    else if(commandStr == "options"){
        // Print the configuration
        configuration.toStream(cout);
        cout << "The options are saved in the file '" << OPTIONS_FILENAME << "' next to the executable." << endl;
    }
    else if(commandStr == "changeoption"){
        if(parameters.size()<3 || parameters.size()>3){
            cout << "Error: wrong number of parameters." << endl;
        }
        else {
            configuration.changeOption(parameters[1],parameters[2]);
        }
    }
    else if(commandStr == "importfile")
    {
        parameters.erase(parameters.begin());

        std::vector<std::string> macrospacesName;

        for(const std::string& str: parameters){
            if(MacroContainer::isNameValid(str)) {
                // Else we have to deal with a macrospace name !
                macrospacesName.emplace_back(str);
            }
        }

        if(macrospacesName.empty())
            macrospacesName.emplace_back("default");

        auto& curMacroSpace = getMacroSpace(macrospacesName.front());

        if(curMacroSpace.importFromFile(parameters.front(), configuration)){
            printStatMacrospace(curMacroSpace);
        }
        else {
            cout << "/!\\ Error: can't open the file provided. /!\\" << endl;
        }
    }

    else if(commandStr == "importfolder")
    {
        std::vector<std::string> macrospacesName;
        parameters.erase(parameters.begin());

        for(const std::string& str: parameters)
        {
            if(str.find(":\\")!=std::string::npos){}
            else if(str.find('/')!=std::string::npos && str.find('.')){}
            else {
                // Else we have to deal with a macrospace name !
                macrospacesName.emplace_back(str);
            }
        }

        if(macrospacesName.empty())
            macrospacesName.emplace_back("default");

        //for(auto& str1:macrospacesName)cout << "- " << str1 << endl;

        if(parameters.size()>=1){
            auto& curMacroSpace = getMacroSpace(macrospacesName.front());
            if(!curMacroSpace.importFromFolder(parameters[0], configuration)){
                std::cout << "/!\\ Error: Can't open this directory /!\\" << endl;
            }
            else
            printStatMacrospace(curMacroSpace);
        }
        else {
            cout << "/!\\ Error: no directory was provided. /!\\" << endl;
        }
        return true;
    }
    else if(commandStr=="where")
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
        string expr = input.substr(9);

        // Extract a macrospace from the command
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
    else if(commandStr.substr(0,4) == "list")
    {
        if(commandStr.substr(4).size()>=1)
            parameters.push_back(commandStr.substr(4));
        parameters.erase(parameters.begin());
        std::vector<std::string> macrospacesNames;

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
            if(doesMacrospaceExists(param)){
                macrospacesNames.emplace_back(param);
            }
            else if(param=="ok")
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

        MacroContainer *mc=nullptr;
        if(macrospacesNames.empty()){
            mc = &(getMacroSpace("default"));
        } else {
            mc = &(getMacroSpace(macrospacesNames.front()));
        }

        for(const auto& p : mc->getDefines())
        {
            if(listRe)
            {
                for(const string& str: mc->getRedefinedMacros())
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
                for(const string& str: mc->getIncorrectMacros())
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
                if(std::find(mc->getIncorrectMacros().begin(), mc->getIncorrectMacros().end(), p.first)==mc->getIncorrectMacros().end()
                && std::find(mc->getRedefinedMacros().begin(), mc->getRedefinedMacros().end(), p.first)==mc->getRedefinedMacros().end())
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
    else if(commandStr == "printsources"){
        string macrospaceName = parameters[1];
        if(doesMacrospaceExists(macrospaceName)){
            getMacroSpace(macrospaceName).printOrigins();
        }
        else {
            cout << "The macrospace '" << macrospaceName << "' does not seem to exist." << endl;
        }
    }
    else if(commandStr == "spacediff")
    {
        if(parameters.size()<3)
        {
            cout << "Not enough parameters were given to the command." << endl;
        }
        else
        {
            MacroContainer *mc1 = tryGetMacroSpace(parameters[1]);
            MacroContainer *mc2 = tryGetMacroSpace(parameters[2]);

            if(!mc1 || !mc2)
            {
                std::cout << "macrospaces not correct." << std::endl;
            }
            else
            {
                mc1->printDiff(*mc2, configuration);
            }

        }
    }
    else if(commandStr == "help")
        printBasicHelp();
    else if(commandStr == "helpall" || (parameters.size()==2 && parameters.front()+parameters[1]=="helpall"))
        printAdvancedHelp();
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

MacroContainer* CommandManager::tryGetMacroSpace(const std::string& macroSpaceName)
{
    for(auto& p: macrospaces){
        if(p.first == macroSpaceName)
            return &(p.second);
    }
    return nullptr;
}

