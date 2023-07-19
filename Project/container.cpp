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

#include <iostream>
#include <cassert>
#include <algorithm>

#include "container.hpp"
#include "options.hpp"
#include "vector.hpp"
#include "stringeval.hpp"

/*** MacroDatabase ***/

// Default constructor

MacroContainer::MacroContainer()
{
    // Set large default presize fro the hashing table
    defines.reserve(50000);
    nbRedefined = 0;
}

bool MacroContainer::exists(const std::string& macroName) const
{
    for(const auto& p: defines){
        if(p.first == macroName )
            return true;
    }
    return false;
}

bool MacroContainer::alreadyExists(const std::string& macroName, const std::string& macroValue) const
{
    auto range = defines.equal_range(macroName);
    for(auto it=range.first; it!=range.second; ++it){
        if(it->second == macroValue && it->first==macroName){
            return true;
        }
    }
    return false;
}

void MacroContainer::compress()
{
    /*for(auto& p: defines)
    {
        p.first.shrink_to_fit();
        p.second.shrink_to_fit();
    }*/
}

void MacroContainer::emplace(const std::string& macroName, const std::string& macroValue)
{
    auto range = defines.equal_range(macroName);
    int occurences = 0;

    for(auto it=range.first; it!=range.second; ++it)
    {
        if(macroValue==it->second)
            return;

        ++occurences;
    }

    // If something
    if(occurences == 1)
    {
        ++nbRedefined;
    }

    defines.emplace(macroName, macroValue);
}

// Getters

void MacroContainer::import(const MacroContainer& mdatabase)
{
    for(const auto& p : mdatabase.defines)
    {
        emplace(p.first, p.second);
    }
}


bool MacroContainer::isRedefined(const std::string& macroName) const
{
    return defines.count(macroName) > 1;
}

/*** MacroContainer class implementation ***/

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
            outputStreamResults << " - " << p.first << " => " << p.second << '\'' << std::endl;
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

void MacroContainer::printOrigins() const
{
    for(const std::string& str: origins){
        std::cout << " - " << str << std::endl;
    }
}

static void printDiffFromList(std::vector<MacroContainer*>& mcs, const Options& configuration, const std::vector<std::string>& param)
{
    // Delete all pointers equald to 0 from the vector
    for(auto it=mcs.begin(); it!=mcs.end();){
        if(*it == nullptr)
            it = mcs.erase(it);
        else
            ++it;
    }

    // Ensure size >= 2
    assert(mcs.size()>=2);

    // Let's run the function
    auto* l = mcs.front();
    l->printDiff(mcs, configuration, param);
}


void MacroContainer::printDiff(std::vector<MacroContainer*>& mcs, const Options& configuration, const std::vector<std::string>& param) const
{
    // Step 0: Let's detect the option parameters involved

    // The differents spacediff command options
    int cmp=0;
    bool dontdisplayUnknown=false;
    bool dontdisplayUndefined=false;
    bool dontdisplayMultiple=false;
    bool showonlyDifferent=false;

    for(const std::string& str: param)
    {
        // Ordering rules

        if(str == "--increasing")
        {
            if(cmp != 0) cmp = -1;
            else cmp = 1;
        }
        else if(str == "--decreasing")
        {
            if(cmp != 0) cmp = -1;
            else cmp = 2;
        }
        else if(str == "--alpha")
        {
            if(cmp != 0) cmp = -1;
            else cmp = 3;
        }

        // Other rules

        else if(str == "--notunknown")
        {
            dontdisplayUnknown = true;
        }

        else if(str == "--notundefined")
        {
            dontdisplayUndefined = true;
        }

        else if (str == "--notmultiple")
        {
            dontdisplayMultiple = true;
        }

        else if (str == "--different")
        {
            showonlyDifferent = true;
        }

    }

    if(cmp == (-1)){
        std::cout << "/!\\ Argument error: you gave two conflicting sorting rules /!\\" << std::endl;
        return;
    }




    // 1st step: look for common macros

    std::vector<const std::string*> commonMacroList;

    for(auto& p : defines)
    {
        bool isCommon=true;
        for(const MacroContainer* mc : mcs)
        {
            if(!mc)
                continue;

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

        if(isCommon
        && (p.first.size()<3 || !(p.first[p.first.size()-1]==')' && p.first[p.first.size()-2]=='x' && p.first[p.first.size()-3]=='(')))
        {
            // Let's recheck that the macro does not already exist before pushing it
            bool good = true;
            for(const std::string* str : commonMacroList){
                if(*str == p.first){
                    good = false;
                    break;
                }
            }
            if(good){
                commonMacroList.push_back(&(p.first));
            }

        }
    }

    std::cout << "Number of common macros: " << commonMacroList.size() << std::endl;

    // Second step: list the result corresponding to these common macros

    if(cmp==1)
    {
        // Increasing order

        std::sort(commonMacroList.begin(), commonMacroList.end(), [mcs, configuration](const std::string *a, const std::string *b){
                std::string aa(*a), bb(*b);
                calculateExprWithStrOutput(aa, *(mcs.front()), configuration);
                calculateExprWithStrOutput(bb, *(mcs.front()), configuration);
                return aa < bb;
        });

    }
    else if(cmp==2)
    {
        // Decreasing order

        std::sort(commonMacroList.begin(), commonMacroList.end(), [mcs, configuration](const std::string *a, const std::string *b){
                std::string aa(*a), bb(*b);
                calculateExprWithStrOutput(aa, *(mcs.front()), configuration);
                calculateExprWithStrOutput(bb, *(mcs.front()), configuration);
                return aa > bb;
        });

    }
    else if(cmp==3)
    {
        // Alpha order
        std::sort(commonMacroList.begin(),commonMacroList.end(), [](const std::string *a, const std::string *b){ return (*a)<(*b); });
    }


    if(dontdisplayUnknown || dontdisplayUndefined || dontdisplayMultiple || showonlyDifferent)
    {
    unsigned totalDisplayed=0;

    // If the macro is common
    for(unsigned i=0; i<commonMacroList.size(); ++i)
    {
        const std::string* str = commonMacroList[i];

        std::vector<std::string> rrr;

        //for(const MacroContainer* mc: mcs)
        for(unsigned i=0; i<mcs.size(); ++i)
        {
            if(!mcs[i])
                continue;

            MacroContainer *mc = mcs[i];

            // Show the result of m
            //auto itFound = std::find(mc->defines.begin(),mc->defines.end(),p.first);
            auto itf = mc->defines.begin();
            for(;itf!=mc->defines.end();++itf)
            {
                if(itf->first == *str){
                    break;
                }
            }


            if(itf != mc->defines.end())
            {
                std::string str2 = itf->second;

                calculateExprWithStrOutput(str2, *mc, configuration, false);

                if(dontdisplayUnknown && str2.find("unknown:") != std::string::npos){
                    rrr.clear();
                    break;
                }

                if(dontdisplayUndefined && str2.find("undefined:") != std::string::npos){
                    rrr.clear();
                    break;
                }

                if(dontdisplayMultiple && str2.find(", ") != std::string::npos){
                    rrr.clear();
                    break;
                }

                if(showonlyDifferent && !rrr.empty() && rrr.back()==str2){
                    rrr.clear();
                    break;
                }

                rrr.push_back( str2);

                /*if(std::find(mc->redefinedMacros.begin(), mc->redefinedMacros.end(), itf->first) != mc->redefinedMacros.end())
                    std::cout << '?';*/


                if(i < mcs.size()-1)
                    rrr.push_back( " | " );
            }
            else
                std::cout << "not found"<< std::endl;
        }

        if(!rrr.empty())
        {
            std::cout << *str << ": ";

            for(std::string& ss: rrr)
                std::cout << ss;

            std::cout << std::endl;

            ++totalDisplayed;
        }


    }

    std::cout << totalDisplayed << " macros were displayed." << std::endl;

    }
    else
    {


    // If the macro is common
    for(unsigned i=0; i<commonMacroList.size(); ++i)
    {
        const std::string* str = commonMacroList[i];

        std::cout << *str << ": ";

        //for(const MacroContainer* mc: mcs)
        for(unsigned i=0; i<mcs.size(); ++i)
        {
            if(!mcs[i])
                continue;

            MacroContainer *mc = mcs[i];

            // Show the result of m
            //auto itFound = std::find(mc->defines.begin(),mc->defines.end(),p.first);
            auto itf = mc->defines.begin();
            for(;itf!=mc->defines.end();++itf)
            {
                if(itf->first == *str){
                    break;
                }
            }


            if(itf != mc->defines.end())
            {
                std::string str2 = itf->second;

                calculateExprWithStrOutput(str2, *mc, configuration, false);

                std::cout << str2;

                /*if(std::find(mc->redefinedMacros.begin(), mc->redefinedMacros.end(), itf->first) != mc->redefinedMacros.end())
                    std::cout << '?';*/


                if(i < mcs.size()-1)
                    std::cout << " | ";
            }
            else
                std::cout << "not found"<< std::endl;
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

void MacroContainer::emplaceAndReplace(const std::string& macroName, const std::string& macroValue)
{
    // 1. Let's add or replace it in the database
    if(defines.count(macroName)>1)
        --nbRedefined;
    defines.erase(macroName);
    defines.emplace(macroName, macroValue);

    // 2. Let's note where it comes from
    std::string added = "define ";
    added += macroName;
    added += ' ';
    added += macroValue;
    origins.emplace_back( std::move(added) );
}

const std::vector<std::string>& MacroContainer::getListOrigins() const
{
    return origins;
}

unsigned MacroContainer::countRedefined() const
{
    return nbRedefined;
}

unsigned MacroContainer::countIncorrectOrEmpty() const
{
    unsigned total=0;

    for(const auto& p: defines)
    {
        if(!doesExprLookOk(p.second))
            ++total;
    }

    return total;
}

void MacroContainer::addOrigin(const std::string& newOrigin)
{
    origins.emplace_back(newOrigin);
}
