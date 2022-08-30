#include "container.hpp"


MacroContainer::MacroContainer()
{
    // Set large default presize
    // avoid reallocating small amount of memory each time
    defines.reserve(10000);
    redefinedMacros.reserve(1000);
    incorrectMacros.reserve(1000);
}

void MacroContainer::emplace(const std::string& macroName, const std::string& macroValue)
{
    bool isTheSame = false;

    for(const std::pair<std::string,std::string>& p: defines)
    {
        if(p.first == macroName)
        {
            if(p.second != macroValue)
                redefinedMacros.emplace_back(macroName);
            else
                isTheSame=true;
            break;
        }
    }

    if(!isTheSame)
    {
        // Add the couple to the define list
        defines.emplace_back( macroName, macroValue );

        if(!doesExprLookOk(macroValue)){
            incorrectMacros.emplace_back(macroName);
        }
    }
}

void MacroContainer::emplaceAndReplace(const std::string& macroName, const std::string& macroValue)
{
    removeFromVector(defines, macroName);
    removeFromVector(incorrectMacros, macroName);
    removeFromVector(redefinedMacros, macroName);
    defines.emplace_back(macroName, macroValue);
}

void MacroContainer::removeFromVector(std::vector< std::pair<std::string,std::string> >& v, const std::string& macroName)
{
    for(auto it=v.begin(); it!=v.end();){
        if(it->first == macroName)
            it=v.erase(it);
        else
            ++it;
    }
}

void MacroContainer::removeFromVector(std::vector<std::string>& v, const std::string& macroName)
{

    for(auto it=v.begin(); it!=v.end();)
    {
        if(*it == macroName)
            it = v.erase(it);
        else
            ++it;
    }
}

void MacroContainer::searchKeywords(const std::vector<std::string>& keywords, std::ostream& outputStreamResults)
{
    for(const auto& p: defines)
    {
        bool okay=true;
        for(const std::string& keyword: keywords)
        {
            if(p.first.find(keyword) == std::string::npos){
                okay=false;
                break;
            }
        }
        if(okay)
            outputStreamResults << " - " << p.first << " => " << p.second << '\'' << endl;
    }
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

