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
#include "macrospace.hpp"

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
    cout << "\nBASIC COMMANDS (for first use of macro parser):" << endl;
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

    cout << "\nADVANCED COMMANDS (other useful commands):" << endl;
    cout << "- define [macro] [value] : add/replace a specific macro by a specific value" << endl;
    cout << "- interpret [macro] : look and choose among possible definitions for this macro" << endl;
    cout << "- evaluate [expr] : evaluate an expression that may contain macros, boolean values.." << endl;
    cout << "- options : display the options used for file import and string evaluation" << endl;
    cout << "- changeoption [name] [value] : change the parameter given to an option" << endl;
    cout << "- clear [all/ok/re/in] : empty the list of all/okay/redefined/incorrect macros" << endl;
    cout << "- cls : clear console" << endl;

    cout << "\nMACRO SPACES (to load macros in separate memory spaces):" << endl;
    cout << "[command] [macrospace?]: to run the command in a macrospace, add macrospace at the end" << endl;
    cout << "- printsources [macrospace] : list the folders from which the list origins" << endl;
    cout << "- list spaces : list the macrospaces currently defined" << endl;
    cout << "msall is a macrospace that designate all the macrospaces unified." << endl;

    /*cout << "\nSPECIAL PARAMETERS (to be implemnted)" << endl;
    cout << "--alphaorder : show results in an alphabetical order" << endl;
    cout << "--increasing : show results in an increasing order" << endl;
    cout << "--decreasing : show results in a decreasing order" << endl;
    cout << "?: describes an optional paramater" << endl;*/

    std::cout << "\nABOUT SCRIPTS:" << std::endl;
    std::cout << "The script 'boot.txt', at the directory of the exe, if it exists, will be executed at each start of the program" << std::endl;
    std::cout << "- loadscript [filepath]: execute a script that contains the commands described above" << std::endl;
    std::cout << "Special script commands: SILENT (don't show script execution on console from now on), TALKY (show it)." << std::endl;
    std::cout << "Scripts accept single line comments // just like in C." << std::endl;
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
    cout << mc.getDefines().size()+mc.getIncorrectMacros().size() << " macros were loaded." << endl;
    cout << "|-> " << mc.getRedefinedMacros().size() << " macros have been redefined." << endl;
    cout << "|-> " << mc.getIncorrectMacros().size() << " macros seem incorrect." << endl;
}



bool CommandManager::runCommand(string input)
{
    // Extract command and parameters from str
    std::vector<std::string> parameters;
    extractList2(parameters, input);
    if(parameters.empty())
        return true;
    std::string& commandStr = parameters[0];
    lowerString(commandStr);

    // Shortcut to the default macrospace
    MacroContainer& mcc = macrospaces.getMacroSpace("default");

    // Locate macrospaces given through user input
    /*std::vector<std::string> commandMacrospaces;
    for(unsigned i=1; i<parameters.size(); ++i){
        if(macrospaces.doesMacrospaceExists(parameters[i]))
            commandMacrospaces.emplace_back(parameters[i]);
    }*/

    if(commandStr.substr(0,5) == "clear")
    {
        if(commandStr.size()>5)
            parameters.emplace_back(commandStr.substr(5));
        parameters.erase(parameters.begin());

        bool fine=true;

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
                fine = false;
            }
        }
        if(!eraseRe && !eraseOk && !eraseIn)
        {
            cout << "No option parameter was given. No macro was erased." << endl;
            cout << "Try 'clear all' or 'clear ok' or 'clear in'." << endl;
        }

        std::vector<std::string> commandMacrospaces;
        for(unsigned i=1; i<parameters.size(); ++i){
            if(macrospaces.doesMacrospaceExists(parameters[i]))
                commandMacrospaces.emplace_back(parameters[i]);
        }
        if(commandMacrospaces.empty())
            commandMacrospaces.emplace_back("default");

        // User message to explain what the program did
        std::cout << "Cleared ";
        if(eraseRe&&eraseOk&&eraseIn) cout << "all "; else cout << "some ";
        cout << "macros in container";
        if(commandMacrospaces.size()>1) cout << 's';
        cout << ": ";
        for(const std::string& str: commandMacrospaces) cout << str << " ";
        cout << endl;

        // Finally let's clear the macro database
        for(const std::string& str: commandMacrospaces)
            macrospaces.getMacroSpace(str).clearDatabase(eraseOk, eraseRe, eraseIn);
    }
    else if(commandStr == "interpret")
    {
        parameters.erase(parameters.begin());

        if(!parameters.empty())
        {
        string macroName = parameters[0];
        MacroContainer *mc=nullptr;
        if(parameters.size()>=2)
            mc = &(macrospaces.getMacroSpace(parameters[1]));
        else
            mc = &(macrospaces.getMacroSpace("default"));


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
                    if(macrospaces.doesMacrospaceExists(parameters[1]))
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

        clearBlacklist();

        /*if(parameters.size()>2 && !macrospaces.doesMacrospaceExists(parameters[2]))
            cout << "The macrospace '" << parameters[1] << "' does not exist." << endl;
        else*/
        {
            std::vector<std::string> commandMacrospaces;
            std::vector<std::string> trueInputs;
            for(unsigned i=1; i<parameters.size(); ++i){
                if(macrospaces.doesMacrospaceExists(parameters[i])){
                    commandMacrospaces.emplace_back(parameters[i]);
                }
                else
                    trueInputs.emplace_back(parameters[i]);
            }
            if(commandMacrospaces.empty())
                commandMacrospaces.emplace_back("default");

            bool found = false;

            if(commandMacrospaces.size()==1 && trueInputs.size()==1)
            {

            for(const auto& p : macrospaces.getMacroSpace(commandMacrospaces.front()).getRedefinedMacros())
            {
                if(p == trueInputs.front()){
                    found = true;
                    break;
                }
            }

            if(found)
            {
                std::cout << "This macro has been redefined and needs to be interpreted." << endl;
                std::cout << "Type 'interpret " << trueInputs.front() << "'." << endl;
            }
            else
            {

            for(auto& p : macrospaces.getMacroSpace(commandMacrospaces.front()).getDefines())
            {
                if(p.first == trueInputs.front())
                {
                    std::vector<std::string> results;
                    string putput=p.second;
                    clearBlacklist();
                    auto status = calculateExpression(putput, macrospaces.getMacroSpace(commandMacrospaces.front()), configuration, true, true, &results);//&results);
                    found=true;

                    if(results.size()>1)
                    {
                        // Sort and remove duplicates
                        auto& v = results;
                        std::sort(v.begin(), v.end());
                        v.erase(std::unique(v.begin(), v.end()), v.end());

                        std::cout << results.size() << " possible results: ";
                        for(unsigned i=0; i<results.size(); ++i){
                            if(results[i]!="multiple"){
                                std::cout << results[i];
                                if(tryConvertToHexa(results[i]))
                                    std::cout << " (hexa: " << results[i] << ')';
                                if(i<results.size()-1 && results[i+1]!="multiple")
                                    std::cout << ", ";
                            }

                        }
                        std::cout << std::endl;

                        cout << "\nIt seems that you are using macros that have been redefined." << endl;
                        cout << "The output can't be trusted." << endl;
                        cout << "To fix a specific macro: please type 'interpret [macro]'." << endl;
                    }
                    else
                    {
                        cout << "first definition: " << p.second << endl;
                    string output = p.second;

                    clearBlacklist();
                    auto status = calculateExpression(output, macrospaces.getMacroSpace(commandMacrospaces.front()), configuration, true, true);
                    if(status == CalculationStatus::EVAL_ERROR)
                        cout << "/!\\ The expression can't be calculated. /!\\" << endl;
                    if(status == CalculationStatus::EVAL_WARNING){
                        cout << "possible output: " << output << "??? ";
                    }
                    else
                        cout << "output: " << output;
                    if(status==CalculationStatus::EVAL_OKAY || status==CalculationStatus::EVAL_WARNING){
                        if(tryConvertToHexa(output))
                            cout << " (hexa: " << output << ')';
                    }
                    if(status == CalculationStatus::EVAL_WARNING){
                        cout << "\n\nIt seems that you are using macros that have been redefined." << endl;
                        cout << "The output can't be trusted." << endl;
                        cout << "To fix a specific macro: please type 'interpret [macro]'." << endl;
                    }
                    if(status == CalculationStatus::EVAL_ERROR ||status == CalculationStatus::EVAL_OKAY)
                        cout << endl;


                    break;
                    }


                }
            }

            if(!found)
                cout << "No macro was found with this name '" << trueInputs.front() << "'." << endl;

            }



            }
            else if(commandMacrospaces.size()>1 && trueInputs.size()==1)
            {
                for(unsigned i=0; i<commandMacrospaces.size(); ++i)
                {
                    cout << commandMacrospaces[i] << " => ";
                    string expr = trueInputs.front();
                    calculateExprWithStrOutput(expr, macrospaces.getMacroSpace(commandMacrospaces[i]), configuration, true);
                    cout << expr << endl;
                }
            }
            else
            {
                cout << "Incorrect parameters were given to the command." << endl;
            }
        }


    }
    else if(commandStr == "define")
    {
        if(parameters.empty())
            std::cout << "Error: no parameter was given to the define command." << endl;
        else
        {
            string str2;
            if(parameters.size()>=3){
                str2 = parameters[2];
                if(!doesExprLookOk(parameters[2]))
                    cout << "/!\\ Warning: the expression of the macro doesn't look correct. /!\\" << endl;
            }
            string macroStringName="default";
            if(parameters.size()>=4)
                macroStringName = parameters[3];
            parameters.emplace_back();
            macrospaces.getMacroSpace(macroStringName).emplaceAndReplace(parameters[1], parameters[2]);
        }
    }
    else if(commandStr.substr(0,4) == "stat")
    {
        if(parameters.front().size()>4){
            parameters.emplace_back(commandStr.substr(4));

        }
        parameters.erase(parameters.begin());

        if(!parameters.empty()){
            if(macrospaces.doesMacrospaceExists(parameters.front()))
            {
                printStatMacrospace(macrospaces.getMacroSpace(parameters.front()));
            }
            else
            {
                cout << "The macrospace '"<< parameters.front() << "' does not exists." << endl;
            }
        }
        else {
            printStatMacrospace(macrospaces.getMacroSpace("default"));
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
                if(macrospaces.doesMacrospaceExists(*it)){
                    std::cout << *it << ", ";
                    macrospacesCur.emplace_back(&(macrospaces.getMacroSpace(*it)));
                    it = parameters.erase(it);
                }
                else
                    ++it;
            }

            if(macrospacesCur.empty()){
                cout << "default";
                macrospacesCur.push_back(&(macrospaces.getMacroSpace("default")));
            }
            cout << endl;


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

        auto& curMacroSpace = macrospaces.getMacroSpace(macrospacesName.front());

        if(curMacroSpace.importFromFile(parameters.front(), configuration)){
            printStatMacrospace(curMacroSpace);
        }
        else {
            cout << "/!\\ Error: can't open the file provided. /!\\" << endl;
            macrospaces.deleteMacroSpace(macrospacesName.front());
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

        if(parameters.size()>=1){
            auto& curMacroSpace = macrospaces.getMacroSpace(macrospacesName.front());
            if(!curMacroSpace.importFromFolder(parameters[0], configuration)){
                std::cout << "/!\\ Error: Can't open this directory /!\\" << endl;
                macrospaces.deleteMacroSpace(macrospacesName.front());
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
            // 1. Locate macrospaces
            std::vector<MacroContainer*> mcs;

            for(auto it=parameters.begin(); it!=parameters.end();)
            {
                if(macrospaces.doesMacrospaceExists(*it)){
                    mcs.push_back(macrospaces.tryGetMacroSpace(*it));
                    it = parameters.erase(it);
                }
                else {
                    ++it;
                }
            }

            // 2. List sources
            std::vector<std::string> lookupFolders;
            for(MacroContainer* mc : mcs){
                mc->getListOrigins(lookupFolders);
            }
            if(parameters.size()>=3){
                lookupFolders.push_back(parameters[2]);
            }

            if(lookupFolders.empty())
            {
                std::cout << "No source folder or file was specified." << endl;
            }
            else
            {
                for(const std::string& str2 : lookupFolders)
                {
                    if(FileSystem::directoryExists(str2.c_str()) && !searchDirectory(str2, parameters[1], configuration))
                    {
                        std::cout << "Can't open the directory '" << str2 << "'" << endl;
                    }
                    else if(searchFile(str2, parameters[1], configuration))
                    {
                        std::cout << " - " << str2 << std::endl;
                    }
                }
            }




        }
    }
    else if(commandStr == "evaluate")
    {
        clearBlacklist();

        string expr = input.substr(9);

        // Extract a macrospace from the command
        std::vector<std::string> macroSpaceNames;
        if(!parameters.empty())
        {
            for(const std::string& str1: parameters) {
                if(macrospaces.doesMacrospaceExists(str1)){
                    macroSpaceNames.push_back(str1);
                    simpleReplace(expr, str1, std::string());
                }
            }
        }
        MacroContainer *mc=&(mcc);
        if(!macroSpaceNames.empty())
            mc=&(macrospaces.getMacroSpace(macroSpaceNames.front()));

        std::vector<std::string> results;
        auto status = calculateExpression(expr, *mc, configuration, true, true, &results);

        if(!results.empty())
        {
            // Sort and remove duplicates
            auto& v = results;
            std::sort(v.begin(), v.end());
            v.erase(std::unique(v.begin(), v.end()), v.end());

            std::cout << results.size() << " possible results: ";
            for(unsigned i=0; i<results.size(); ++i){
                if(results[i] != "multiple"){
                    std::cout << results[i];
                    if(tryConvertToHexa(results[i]))
                        std::cout << " (hexa: " << results[i] << ')';
                    if(i<results.size()-1 && results[i+1]!="multiple")
                        std::cout << ", ";
                }
            }
            std::cout << std::endl;

            cout << "\nIt seems that you are using macros that have been redefined." << endl;
            cout << "The output can't be trusted." << endl;
            cout << "To fix a specific macro: please type 'interpret [macro]'." << endl;
        }
        else
        {

        /*if(status == CalculationStatus::EVAL_WARNING){
            std::cout << "possible ";
        }


        if(status==CalculationStatus::EVAL_OKAY || status==CalculationStatus::EVAL_WARNING){
            if(tryConvertToHexa(expr))
                cout << expr ;
            cout << endl;
        }*/

        cout << "output: " << expr;

        if(tryConvertToHexa(expr))
            cout << " (hexa: " << expr << ')';

        if(status == CalculationStatus::EVAL_WARNING)
        {
            cout << " ???" << endl;
            cout << "It seems that you are using macros that have been redefined." << endl;
            cout << "The output can't be trusted." << endl;
            cout << "To fix a specific macro: please type 'interpret [macro]'." << endl;
        }
        else
            cout << endl;
        if(status == CalculationStatus::EVAL_ERROR)
        {
            cout << "\n/!\\ The expression can't be calculated. /!\\" << endl;
        }

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
            macrospaces.printContentToUser();
        }
        else
        {

        for(const std::string& param: parameters)
        {
            if(macrospaces.doesMacrospaceExists(param)){
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
            mc = &(macrospaces.getMacroSpace("default"));
        } else {
            mc = &(macrospaces.getMacroSpace(macrospacesNames.front()));
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

            if(listOk)
            {
                if(std::find(mc->getRedefinedMacros().begin(), mc->getRedefinedMacros().end(), p.first)==mc->getRedefinedMacros().end())
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

        if(listIn)
        {
            for(const auto& p: mc->getIncorrectMacros())
            {
                cout << " - " << p.first << " => " << p.second << endl;
                ++nbPrinted;

                if(nbPrinted >= 5000)
                {
                    std::cout << "Only printed the first 5000 results." << endl;
                    break;
                }
            }
        }

        }

    }
    else if(commandStr == "printsources"){
        string macrospaceName = parameters[1];
        if(macrospaces.doesMacrospaceExists(macrospaceName)){
            macrospaces.getMacroSpace(macrospaceName).printOrigins();
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
            std::vector<MacroContainer*> mcContainer;

            if(parameters.size()<3){
                std::cout << "Spacediff need at least 2 different macrospaces." << endl;
            }

            bool inputOkay=true;
            for(unsigned i=1; i<parameters.size(); ++i)
            {
                MacroContainer *tmp_mc = macrospaces.tryGetMacroSpace(parameters[i]);
                if(tmp_mc){
                    mcContainer.push_back(tmp_mc);
                }
                else {
                    mcContainer.push_back(nullptr);
                    inputOkay=false;
                }
            }

            if(!inputOkay)
            {
                std::cout << "macrospaces";

                // let's write the name of the macrospaces not correct
                for(unsigned i=0; i<mcContainer.size(); ++i){
                    if(!mcContainer[i]){
                        std::cout << " \"" << parameters[i+1] << '"';
                    }
                }

                std::cout << "not recognized." << std::endl;
            }
            else
            {
                MacroContainer::printDiffFromList(mcContainer, configuration);
            }

        }
    }
    else if(commandStr == "helpall" || (parameters.size()==2 && parameters.front()+parameters[1]=="helpall"))
        printAdvancedHelp();
    else if(commandStr == "help")
        printBasicHelp();
    else if(commandStr == "cls"){
        system("cls");

    }
    else if(commandStr == "loadscript")
    {
        if(parameters.size()>1)
        {
            if(!loadScript(parameters[1]))
                std::cout << "Could not load the script file '" << parameters[1] << "'" << std::endl;
        }

        else
            std::cout << "Error: no parameter given to the command." << std::endl;
    }
    else if(commandStr == "exit")
        return false;
    else {
        cout << "This command is unknown." << endl;
    }

    return true;
}

static bool preprocessLine(std::string& line)
{
    auto searchComment = line.find("//");

    if(searchComment == std::string::npos)
        return true;

    bool isComment=true;

    unsigned i=0;
    for(; i<line.size() && i<searchComment; ++i)
    {
        if(line[i]!=' '){
            isComment=false;
            break;
        }

    }

    line = line.substr(i);

    if(!isComment)
    {
        line = line.substr(0, searchComment-i);
    }

    return !isComment;
}

static bool isEmptyLine(const std::string& str)
{
    for(char c: str){
        if(c != ' ')
            return false;
    }
    return true;
}

static auto* gg = std::cout.rdbuf();

bool CommandManager::loadScript(const std::string& filepath, bool printStatus)
{
    std::ifstream file(filepath);

    if(file)
    {
        if(printStatus)
            std::cout << "Currently executing " << filepath << "." << std::endl;

        std::string line;

        while(std::getline(file, line))
        {
            if(preprocessLine(line))
            {
                if(isEmptyLine(line))
                {}
                else if(line=="SILENT")
                {
                    std::cout.rdbuf(nullptr);
                }
                else if(line == "TALKY")
                {
                    std::cout.rdbuf(gg);
                }
                else if(!line.empty())
                {
                    std::cout << "\nRan '" << line << "'." << std::endl;
                    runCommand(line);
                }

            }

        }

        std::cout.rdbuf(gg);

        if(printStatus)
            std::cout << "Ended the execution of " << filepath << "." << std::endl;


        return true;
    }

    return false;
}

