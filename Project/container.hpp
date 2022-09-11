/**
  ******************************************************************************
  * @file    container.hpp
  * @author  MCD Application Team
  * @brief   Macro-Parser
  *
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

#ifndef CONTAINER_HPP
#define CONTAINER_HPP

#include <vector>
#include <string>

#include "options.hpp"
#include "filesystem.hpp"

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

class MacroContainer : public MacroDatabase
{
public:
    // Default constructor
    MacroContainer();
    MacroContainer(const MacroDatabase&);

    // Import commands (herited from MacroDatabase)
    bool importFromFile(const std::string& filepath, const Options& config);
    bool importFromFolder(const std::string& folderpath, const Options& config);

    // Console commands
    static bool isNameValid(const std::string& macroContainerName);
    void clearDatabase(bool clearOkay, bool clearRedefined, bool clearIncorrect);
    void searchKeywords(const std::vector<std::string>& keywords, std::ostream& outputStreamResults) const;
    unsigned countMacroName(const std::string& macroName) const;
    bool isRedefined(std::string macroName) const;
    void printOrigins() const;
    void printDiff(const MacroContainer&, const Options& configuration) const;

private:
    std::vector< std::string > origins;
};

#include "stringeval.hpp"


#endif // CONTAINER_HPP
