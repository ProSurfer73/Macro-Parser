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
    for(auto it=defines.begin(); it!=defines.end();){
        if(it->first == macroName)
            it=defines.erase(it);
        else
            ++it;
    }
    removeFromVector(incorrectMacros, macroName);
    removeFromVector(redefinedMacros, macroValue);
    defines.emplace_back(macroName, macroValue);
}


void MacroContainer::removeFromVector(std::vector<std::string>& v, const std::string& str)
{

    for(auto it=v.begin(); it!=v.end();)
    {
        if(*it == str)
            it = v.erase(it);
        else
            ++it;
    }
}

