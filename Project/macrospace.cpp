#include "macrospace.hpp"

Macrospaces::Macrospaces()
{
    updateMsAll();
}

void Macrospaces::addMacroSpace(const std::string& macrospaceName, const MacroContainer& macrospace)
{
    macrospaces.emplace_back(macrospaceName, macrospace);

    if(macrospaceName=="msall")
        updateMsAll();
}

MacroContainer& Macrospaces::getMacroSpace(const std::string& macrospaceName)
{
    if(macrospaceName=="msall")
        updateMsAll();

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

bool Macrospaces::doesMacrospaceExists(const std::string& macrospaceName)
{
    for(const auto& p: macrospaces){
        if(p.first == macrospaceName)
            return true;
    }

    return false;
}

MacroContainer* Macrospaces::tryGetMacroSpace(const std::string& macroSpaceName)
{
    if(macroSpaceName=="msall")
        updateMsAll();

    for(auto& p: macrospaces){
        if(p.first == macroSpaceName)
            return &(p.second);
    }
    return nullptr;
}

void Macrospaces::deleteMacroSpace(const std::string& macroSpaceName)
{
    for(auto it=macrospaces.begin(); it!=macrospaces.end();++it){
        if(it->first == macroSpaceName){
            macrospaces.erase(it);
            return;
        }
    }
}

void Macrospaces::printContentToUser()
{
    updateMsAll();
    for(const auto& p: macrospaces){
            std::cout << "- " << p.first << " => " << p.second.getDefines().size() << " macros." << endl;
    }
}

std::size_t Macrospaces::size() const
{
    return macrospaces.size();
}

void Macrospaces::updateMsAll()
{
    MacroContainer *mc=nullptr;
    for(auto& p: macrospaces){
        if(p.first == "msall")
            mc = &(p.second);
    }

    if(!mc){
        macrospaces.emplace_back("msall", MacroContainer());
        mc = &(macrospaces[macrospaces.size()-1].second);
    }

    for(auto& p : macrospaces)
    {
        if(p.first != "msall")
        {
            mc->import(p.second);
        }
    }
}
