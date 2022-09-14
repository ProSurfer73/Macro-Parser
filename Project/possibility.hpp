#ifndef POSSIBILITY_HPP
#define POSSIBILITY_HPP

#include <string>

// Abstract class
class PossibilityPicker
{
public:
    virtual void choose(const std::string& curexpr, const std::string& macroName, std::vector<std::string>& possibilities)=0;
};

// Default chooser: choose everytime the first option available
class DefaultPicker
{
public:
    virtual void choose(const std::string& curexpr, const std::string& macroName, std::vector<std::string>& possibilities);
    void getFinalResult(std::string& str);
};

// Random picker: choose randomly among possible options everytime
class RandomPicker
{
public:
    virtual void choose(const std::string& curexpr, const std::string& macroName, std::vector<std::string>& possibilities);

};




#endif // POSSIBILITY_HPP
