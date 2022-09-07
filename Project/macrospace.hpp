#ifndef MACROSPACE_HPP
#define MACROSPACE_HPP

#include <vector>
#include "container.hpp"

class MacroContainer;

 class MacroSpace : public MacroContainer
{
public:
    MacroSpace();

    bool importFromFile(const std::string& fileName, const Options& config);
    bool importFromFolder(const std::string& configPath, const Options& config);

private:
    std::vector<std::string> sourcesLoaded;
};


class MacroSpaceContainer
{
public:
    MacroSpaceContainer();

    MacroSpace& giveMacroSpace(string name);


private:
    std::vector< std::pair<std::string, MacroSpace> > storage;
};



#endif // MACROSPACE_HPP
