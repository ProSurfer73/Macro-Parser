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
#include <unordered_map>

#include "options.hpp"
#include "filesystem.hpp"


class MacroDatabase
{
public:
    MacroDatabase();
    void emplace(const std::string& macroName, const std::string& macroValue);
    virtual void emplaceAndReplace(const std::string& macroName, const std::string& macroValue);
    bool importFromFile(const std::string& filepath, const Options& config);
    bool importFromFolder(const std::string& folderpath, const Options& config);
    void import(const MacroDatabase& macrodatabase);
    void compress();

    // Getters
    inline const std::unordered_multimap< std::string, std::string >& getDefines() const { return defines; }
    bool exists(const std::string& macroName) const;
    bool isRedefined(const std::string& macroName) const;
    bool alreadyExists(const std::string& macroName, const std::string& macroValue) const;

private:
    static bool emplaceOnce(std::vector< std::string >& v, const std::string& macroName);
    static bool emplaceOnce(std::vector< std::pair<std::string,std::string> >& v, const std::string& macroName, const std::string& macroValue);
    static void removeFromVector(std::vector< std::pair<std::string,std::string> >& v, const std::string& macroName);
    static void removeFromVector(std::vector<std::string>& v, const std::string& str);

protected:
    std::unordered_multimap< std::string, std::string > defines;
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


    static void printDiffFromList(std::vector<MacroContainer*>& mcs, const Options& configuration, const std::vector<std::string>& param);

    void printDiff(std::vector<MacroContainer*>& mcs, const Options& configuration, const std::vector<std::string>& param) const;

    void getListOrigins(std::vector<std::string>& v) const;
    const std::vector<std::string>& getListOrigins() const;
    virtual void emplaceAndReplace(const std::string& macroName, const std::string& macroValue);

private:
    std::vector< std::string > origins;
};

#include "stringeval.hpp"


#endif // CONTAINER_HPP
