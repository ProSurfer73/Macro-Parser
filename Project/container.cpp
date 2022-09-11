/**
  ******************************************************************************
  * @file    container.cpp
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

#include "container.hpp"

/*** MacroDatabase ***/



// Default constructor

MacroDatabase::MacroDatabase()
{
    // Set large default presize
    // avoid reallocating small amount of memory each time
    defines.reserve(10000);
    redefinedMacros.reserve(1000);
    incorrectMacros.reserve(1000);
}

void MacroDatabase::emplace(const std::string& macroName, const std::string& macroValue)
{
    bool alreadyExists=false;

    for(const auto& p: defines)
    {
        if(p.first == macroName && p.second!=macroValue){
            alreadyExists=true;
            break;
        }
    }

    if(alreadyExists)
    {
        emplaceOnce(redefinedMacros, macroName);
    }
    if(!doesExprLookOk(macroValue))
    {
        emplaceOnce(incorrectMacros, macroName);
    }
    emplaceOnce(defines, macroName, macroValue);
}

void MacroDatabase::emplaceAndReplace(const std::string& macroName, const std::string& macroValue)
{
    removeFromVector(defines, macroName);
    removeFromVector(incorrectMacros, macroName);
    removeFromVector(redefinedMacros, macroName);
    defines.emplace_back(macroName, macroValue);
}

bool MacroDatabase::importFromFile(const std::string& filepath, const Options& config)
{
    return FileSystem::importFile(filepath, *this, config);
}

bool MacroDatabase::importFromFolder(const std::string& folderpath, const Options& config)
{
    return FileSystem::importDirectory(folderpath, *this, config);
}

// Getters

const std::vector< std::pair< std::string, std::string> >& MacroDatabase::getDefines() const
{
    return defines;
}

const std::vector< std::string >& MacroDatabase::getRedefinedMacros() const
{
    return redefinedMacros;
}

const std::vector< std::string >& MacroDatabase::getIncorrectMacros() const
{
    return incorrectMacros;
}

bool MacroDatabase::emplaceOnce(std::vector< std::string >& v, const std::string& macroName)
{
    if(v.empty()){
        v.emplace_back(macroName);
        return true;
    }

    if(std::find(v.begin(), v.end(), macroName)==v.end())
    {
        v.emplace_back(macroName);
        return true;
    }

    return false;
}

bool MacroDatabase::emplaceOnce(std::vector< std::pair<std::string,std::string> >& v, const std::string& macroName, const std::string& macroValue)
{
    if(v.empty()){
        v.emplace_back(macroName, macroValue);
        return true;
    }


    bool exists=false;
    for(auto it=v.begin(); it!=v.end(); ++it)
    {
        if(it->first == macroName && it->second == macroValue)
        {
            exists=true;
        }
    }

    if(!exists)
    {
        v.emplace_back(macroName, macroValue);
    }

    return !exists;
}

void MacroDatabase::removeFromVector(std::vector< std::pair<std::string,std::string> >& v, const std::string& macroName)
{
    for(auto it=v.begin(); it!=v.end();){
        if(it->first == macroName)
            it=v.erase(it);
        else
            ++it;
    }
}

void MacroDatabase::removeFromVector(std::vector<std::string>& v, const std::string& macroName)
{

    for(auto it=v.begin(); it!=v.end();)
    {
        if(*it == macroName)
            it = v.erase(it);
        else
            ++it;
    }
}

/*** MacroContainer class implementation ***/

MacroContainer::MacroContainer()
: MacroDatabase(), origins()
{}

MacroContainer::MacroContainer(const MacroDatabase& database)
: MacroDatabase(database)
{}

bool MacroContainer::isNameValid(const std::string& macroContainerName)
{
    bool stillOkay=true;
    for(char c: macroContainerName){
        if(!(isalnum(c)||c=='_'))
            stillOkay=false;
    }
    return stillOkay;
}

void MacroContainer::clearDatabase(bool clearDefines, bool clearRedefined, bool clearIncorrect)
{
    if(clearDefines)
        defines.clear();
    if(clearRedefined)
        redefinedMacros.clear();
    if(clearIncorrect)
        incorrectMacros.clear();
}



// Lookup commands

void MacroContainer::searchKeywords(const std::vector<std::string>& keywords, std::ostream& outputStreamResults) const
{
    for(const auto& p: defines)
    {
        bool okay=false;
        for(const std::string& keyword: keywords)
        {
            if(p.first.find(keyword) != std::string::npos){
                okay=true;
                break;
            }
        }
        if(okay)
            outputStreamResults << " - " << p.first << " => " << p.second << '\'' << endl;
    }
}


unsigned MacroContainer::countMacroName(const std::string& macroName) const
{
    unsigned nb=0;
    for(const std::pair<std::string,std::string>& p : defines)
    {
        if(p.first == macroName)
            nb++;
    }
    return nb;
}

bool MacroContainer::isRedefined(std::string macroName) const
{
    unsigned nb=0;
    for(const std::pair<std::string,std::string>& p : defines)
    {
        if(p.first == macroName){
            if(++nb >= 2)
                return true;
        }
    }
    return false;
}

bool MacroContainer::importFromFile(const std::string& filepath, const Options& config)
{
    if(MacroDatabase::importFromFile(filepath, config)){
        origins.emplace_back(filepath);
        return true;
    }
    return false;
}

bool MacroContainer::importFromFolder(const std::string& folderpath, const Options& config)
{
    if(MacroDatabase::importFromFolder(folderpath, config)){
        origins.emplace_back(folderpath);
        return true;
    }
    return false;
}

void MacroContainer::printOrigins() const
{
    for(const std::string& str: origins){
        std::cout << " - " << str << endl;
    }
}

void MacroContainer::printDiff(const MacroContainer& mc, const Options& configuration) const
{
    unsigned limit=100;

    for(const auto& p1 : defines)
    {
        for(const auto& p2 : mc.defines)
        {
            // If this is two macros that have the same name
            if(p1.first == p2.first)
            {
                // Show the macro that is going to be compared

                // Let's calculate the expressions
                std::string expr1 = p1.first;
                calculateExprWithStrOutput(expr1, *this, configuration);
                std::string expr2 = p1.first;
                calculateExprWithStrOutput(expr2, mc, configuration);

                if(expr1 != "unknown" && expr2 != "unknown")
                    std::cout << p1.first << ": " << expr1 << " | " << expr2 << std::endl;

                /*
                // Calculate the first expression according to *this instance
                std::string expr1 = p1.first;
                auto status1 = calculateExpression(expr1, *this, configuration, false);

                // Calculate the second expression accoding to mc instance
                std::string expr2 = p1.first;
                auto status2 = calculateExpression(expr2, mc, configuration, false);

                // Convert the results to hexa if possible
                if(status1 == CalculationStatus::EVAL_OKAY || status1 == CalculationStatus::EVAL_WARNING)
                    tryConvertToHexa(expr1);
                if(status2 == CalculationStatus::EVAL_OKAY || status2 == CalculationStatus::EVAL_WARNING)
                    tryConvertToHexa(expr2);

                // Show the result expr1
                if(status1 == CalculationStatus::EVAL_OKAY)
                    std::cout << expr1;
                else if(status1 == CalculationStatus::EVAL_WARNING)
                    std::cout << expr1 << '?';
                else if(status1 == CalculationStatus::EVAL_ERROR)
                    std::cout << "unknown";

                std::cout << " | ";

                // Show the result expr2
                if(status2 == CalculationStatus::EVAL_OKAY)
                    std::cout << expr2;
                else if(status2 == CalculationStatus::EVAL_WARNING)
                    std::cout << expr2 << '?';
                else if(status2 == CalculationStatus::EVAL_ERROR)
                    std::cout << "unknown";

                cout << endl;

                if(--limit<0)
                    return;*/
            }
        }
    }
}




