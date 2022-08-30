#include "options.hpp"

Options::Options()
{
    resetToDefault();
    loadFromFile(OPTIONS_FILENAME);
}

void Options::resetToDefault()
{
    // Set default parameters for the option
    importOnlySourceFileExtension = true;
    importMacroCommented = false;
    printReplacements = false;
    printExprAtEveryStep = false;
    keepListRedefinedMacros = true;
}


bool Options::loadFromFile(const char* filename)
{
    std::ifstream file(filename);

    if(file.is_open())
    {
        std::string line;

        while(getline(file, line))
        {
            if(line.substr(0,30)=="importOnlySourceFileExtension=")
            {
                loadBooleanValue(line.substr(30), importOnlySourceFileExtension);
            }
            else if(line.substr(0,21)=="importMacroCommented=")
            {
                loadBooleanValue(line.substr(21), importMacroCommented);
            }
            else if(line.substr(0,18)=="printReplacements=")
            {
                loadBooleanValue(line.substr(18), printReplacements);
            }
            else if(line.substr(0,21)=="printExprAtEveryStep=")
            {
                loadBooleanValue(line.substr(21), printExprAtEveryStep);
            }
            else if(line.substr(0,24) == "keepListRedefinedMacros=")
            {
                loadBooleanValue(line.substr(24), keepListRedefinedMacros);
            }
        }




        return true;
    }


    return false;
}

bool Options::loadBooleanValue(const std::string& input, bool& booleanValue)
{
    if(input=="1" || input=="true"){
        booleanValue = true;
        return true;
    }
    else if(input == "0" || input=="false"){
        booleanValue = false;
        return true;
    }

    return false;
}

void Options::toStream(std::ostream& stream) const
{
    stream << "importOnlySourceFileExtension=" << importOnlySourceFileExtension << std::endl;
    stream << "importMacroCommented=" << importMacroCommented << std::endl;
    stream << "printReplacements=" << printReplacements << std::endl;
    stream << "printExprAtEveryStep=" << printExprAtEveryStep << std::endl;
    stream << "keepListRedefinedMacros=" << keepListRedefinedMacros << std::endl;
}



bool Options::saveToFile(const char* filename) const
{
    std::ofstream file(filename);

    if(!file)
        return false;

    toStream(file);
    return true;
}

bool Options::changeOption(const std::string& s1, const std::string& s2)
{
    bool valueToBeSet=false;

    // Interpret s2
    if(s2=="1"||s2=="true"){
        valueToBeSet=true;
    }
    else if(s2=="0"||s2=="false"){
        valueToBeSet=false;
    }
    else {
        std::cout << "Error setting the option : The value must be 1 or 0." << std::endl;
        return false;
    }


    if(s1=="importOnlySourceFileExtension"){
        importOnlySourceFileExtension=valueToBeSet;
    }
    else if(s1=="importMacroCommented"){
        importMacroCommented=valueToBeSet;
    }
    else if(s1=="printReplacements"){
        printReplacements=valueToBeSet;
    }
    else if(s1=="printExprAtEveryStep"){
        printExprAtEveryStep=valueToBeSet;
    }
    else if(s1=="keepListRedefinedMacros"){
        keepListRedefinedMacros=valueToBeSet;
    }
    else {
        std::cout << "Error setting the option: the option does not exists." << std::endl;
        return false;
    }

    saveToFile(OPTIONS_FILENAME);

    return true;
}

// Getters implementation

bool Options::doesImportOnlySourceFileExtension() const
{
    return importOnlySourceFileExtension;
}

bool Options::doesImportMacroCommented() const
{
    return importMacroCommented;
}

bool Options::doesPrintReplacements() const
{
    return printReplacements;
}

bool Options::doesPrintExprAtEveryStep() const
{
    return printExprAtEveryStep;
}

bool Options::doKeepListRedefinedMacros() const
{
    return keepListRedefinedMacros;
}
