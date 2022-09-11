
#ifndef MACROSPACE_HPP
#define MACROSPACE_HPP

#include <vector>
#include "container.hpp"

class Macrospaces
{
public:
    Macrospaces();

    void addMacroSpace(const std::string& macrospaceName, const MacroContainer& macrospace);
    MacroContainer& getMacroSpace(const std::string& macrospaceName);
    bool doesMacrospaceExists(const std::string& macrospaceName) const;
    MacroContainer* tryGetMacroSpace(const std::string& macrospaceName);
    void deleteMacroSpace(const std::string& macrospaceName);
    void printContentToUser();
    std::size_t size() const;

private:
    void updateMsAll();

private:
    std::vector< std::pair<std::string, MacroContainer> > macrospaces;
};




#endif // MACROSPACE_HPP
