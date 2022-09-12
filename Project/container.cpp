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
    if(doesExprLookOk(macroValue))
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

        emplaceOnce(defines, macroName, macroValue);
    }
    else
    {
        emplaceOnce(incorrectMacros, macroName, macroValue);
    }
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

const std::vector< std::pair<std::string,std::string> >& MacroDatabase::getIncorrectMacros() const
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

void MacroDatabase::import(const MacroDatabase& mdatabase)
{
    for(const auto& p : mdatabase.defines)
    {
        emplace(p.first, p.second);
    }
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

void MacroContainer::printDiffFromList(std::vector<MacroContainer*>& mcs, const Options& configuration)
{
    // Ensure size >= 2
    assert(mcs.size()>=2);

    MacroContainer *first = mcs.front();
    //mcs.erase(mcs.begin());
    first->printDiff(mcs, configuration);
}

void MacroContainer::printDiff(std::vector<MacroContainer*>& mcs, const Options& configuration) const
{
    unsigned limit=100;

    // 1st step: look for common macros

    std::vector<std::string> commonMacroList;

    for(auto& p : defines)
    {
        bool isCommon=true;
        for(const MacroContainer* mc : mcs)
        {
            bool exist=false;
            for(auto& p2 : mc->defines)
            {
                if(p.first == p2.first){
                    exist=true;
                    break;
                }
            }
            if(!exist){
                isCommon=false;
                break;
            }
        }

        if(isCommon && std::find(commonMacroList.begin(), commonMacroList.end(), p.first)==commonMacroList.end())
            commonMacroList.emplace_back(p.first);
    }

    std::cout << "Number of common macros: " << commonMacroList.size() << endl;

    // Second step: list the result corresponding to these common macros


    for(const auto& p: (mcs.front()->defines) )
    {
        // If the macro is common
        if(std::find(commonMacroList.begin(), commonMacroList.end(), p.first) != commonMacroList.end())
        {
            std::cout << p.first << ": ";

            //for(const MacroContainer* mc: mcs)
            for(unsigned i=0; i<mcs.size(); ++i)
            {
                MacroContainer *mc = mcs[i];

                // Show the result of m
                //auto itFound = std::find(mc->defines.begin(),mc->defines.end(),p.first);
                auto itf = mc->defines.begin();
                for(;itf!=mc->defines.end();++itf)
                {
                    if(itf->first == p.first){
                        break;
                    }
                }


                if(itf != mc->defines.end())
                {
                    std::string str = itf->second;

                    calculateExprWithStrOutput(str, *mc, configuration);

                    std::cout << str;

                    if(i < mcs.size()-1)
                        std::cout << " | ";
                }
                else
                    std::cout << "not found"<< endl;
            }

            std::cout << std::endl;
        }
    }

}


void MacroContainer::getListOrigins(std::vector<std::string>& v) const
{
    for(const std::string& str: origins)
    {
        v.emplace_back(str);
    }
}



