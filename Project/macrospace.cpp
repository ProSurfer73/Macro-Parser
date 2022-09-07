#include "macrospace.hpp"

/*** MacroSpace class ***/

MacroSpace::MacroSpace()
{}

bool MacroSpace::importFromFile(const std::string& filepath, const Options& config)
{
    if(importDirectory(filepath, *this, config)){
        sourcesLoaded.emplace_back(filepath);
        return true;
    }

    std::cout << "The file could not be opened." << endl;
    return false;
}

bool MacroSpace::importFromFolder(const std::string& folderpath, const Options& config)
{
    if(importDirectory(folderpath, *this, config)){
        sourcesLoaded.emplace_back(folderpath);
        return true;
    }

    std::cout << "The folder could not be opened." << endl;
    return false;
}


/*** MacroSpaceContainer class ***/

MacroSpaceContainer::MacroSpaceContainer()
{}

MacroSpace& MacroSpaceContainer::giveMacroSpace(string name)
{
    for(auto& p : storage){
        if(p.first == name)
            return p.second;
    }

    storage.emplace_back(name, MacroSpace());
    return (storage[storage.size()-1].second);
}
