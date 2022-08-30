#ifndef CONTAINER_HPP
#define CONTAINER_HPP

#include <vector>
#include <string>
#include "stringeval.hpp"

class MacroContainer
{
public:
    MacroContainer();
    void emplace(const std::string& macroName, const std::string& macroValue);
    void emplaceAndReplace(const std::string& macroName, const std::string& macroValue);

    // Attributes
    std::vector< std::pair< std::string, std::string> > defines;
    std::vector< std::string > redefinedMacros;
    std::vector< std::string > incorrectMacros;

private:
    static void removeFromVector(std::vector<std::string>& v, const std::string& str);
};


#endif // CONTAINER_HPP
