#ifndef CONTAINER_HPP
#define CONTAINER_HPP

#include <vector>
#include <string>
#include "stringeval.hpp"
#include "options.hpp"


class MacroDatabase
{
public:
    MacroDatabase();
    void emplace(const std::string& macroName, const std::string& macroValue);
    void emplaceAndReplace(const std::string& macroName, const std::string& macroValue);
    bool importFromFile(const std::string& filepath, const Options& config);
    bool importFromFolder(const std::string& folderpath, const Options& config);
    // Getters
    const std::vector< std::pair< std::string, std::string> >& getDefines() const;
    const std::vector< std::string >& getRedefinedMacros() const;
    const std::vector< std::string >& getIncorrectMacros() const;

private:
    static bool emplaceOnce(std::vector< std::string >& v, const std::string& macroName);
    static bool emplaceOnce(std::vector< std::pair<std::string,std::string> >& v, const std::string& macroName, const std::string& macroValue);
    static void removeFromVector(std::vector< std::pair<std::string,std::string> >& v, const std::string& macroName);
    static void removeFromVector(std::vector<std::string>& v, const std::string& str);

protected:
    std::vector< std::pair<std::string,std::string> > defines;
    std::vector< std::string > redefinedMacros;
    std::vector< std::string > incorrectMacros;
};

class MacroContainerr : public MacroDatabase
{
public:
    // Default constructor
    MacroContainerr();

    // Console commands

    void clearDatabase(bool clearOkay, bool clearRedefined, bool clearIncorrect);
    void searchKeywords(const std::vector<std::string>& keywords, std::ostream& outputStreamResults) const;
    unsigned countMacroName(const std::string& macroName) const;
    bool isRedefined(std::string macroName) const;

private:
    std::vector< std::string > origins;
};


#endif // CONTAINER_HPP
