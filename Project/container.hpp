#ifndef CONTAINER_HPP
#define CONTAINER_HPP

#include <vector>
#include <string>
#include "stringeval.hpp"

class MacroContainer
{
public:
    // Default constructor
    MacroContainer();

    // Modification methods
    void emplace(const std::string& macroName, const std::string& macroValue);
    void emplaceAndReplace(const std::string& macroName, const std::string& macroValue);
    void clearDatabase(bool clearOkay, bool clearRedefined, bool clearIncorrect);

    // Lookup commands
    void searchKeywords(const std::vector<std::string>& keywords, std::ostream& outputStreamResults) const;
    unsigned countMacroName(const std::string& macroName) const;

    // Getters
    const std::vector< std::pair< std::string, std::string> >& getDefines() const;
    const std::vector< std::string >& getRedefinedMacros() const;
    const std::vector< std::string >& getIncorrectMacros() const;

private:
    static void removeFromVector(std::vector< std::pair<std::string,std::string> >& v, const std::string& macroName);
    static void removeFromVector(std::vector<std::string>& v, const std::string& str);

    std::vector< std::pair< std::string, std::string> > defines;
    std::vector< std::string > redefinedMacros;
    std::vector< std::string > incorrectMacros;

//friend bool calculateExpression(string& expr, const MacroContainer& macroContainer, bool& shouldDisplayPbInfo, const Options& config);
//friend bool runCommand(string str, MacroContainer& macroContainer, Options& configuration);
};


#endif // CONTAINER_HPP
