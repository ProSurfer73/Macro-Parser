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
class Options;

/**< A database of macros defined by name and listing from where the macros come from. */
class MacroContainer
{
public:
    /** \brief Default constructor. Initialize an empty database.
     */
    MacroContainer();

    /** \brief add a new macro to the database.
     *
     * \param macroName the name of the macro.
     * \param macroValue its value.
     */
    void emplace(const std::string& macroName, const std::string& macroValue);

    /** \brief import the macros from another database into this database.
     *
     * \param macrodatabase the other database from which we want to merge the macros.
     */
    void import(const MacroContainer& macrodatabase);

    /** \brief compress the database to reduce memory footprint.
     */
    void compress();

    // Getters
    inline const std::unordered_multimap< std::string, std::string >& getDefines() const { return defines; }
    bool exists(const std::string& macroName) const;
    bool isRedefined(const std::string& macroName) const;
    bool alreadyExists(const std::string& macroName, const std::string& macroValue) const;

public:
    /// Console related commands

    /** \brief check if the name of potential macrospace is considered correct (no space characters are allowed).
     *
     * \param macroContainerName the string to be checked.
     * \return true if it respects the conditions, false otherwise.
     */
    static bool isNameValid(const std::string& macroContainerName);

    /** \brief deletes macros from the categories listed.
     *
     * \param clearOkay if true, it erases macros that are considered to be valid.
     * \param clearRedefined if true, it clears macros that have multiple definitions.
     * \param clearIncorrect if true, it erases incorrect macros from the list.
     */
    void clearDatabase(bool clearOkay, bool clearRedefined, bool clearIncorrect);

    /** \brief if a macro name contains one of the keyord, it will be printed on the output stream result.
     *
     * \param keywords a list of keyword to look for.
     * \param outputStreamResults generally std::cout.
     */
    void searchKeywords(const std::vector<std::string>& keywords, std::ostream& outputStreamResults) const;

    /** \brief Counts the number of values attached to a particular macro name.
     *
     * \param macroName the name of the macro.
     * \return the number of values attached to it.
     */
    unsigned countMacroName(const std::string& macroName) const;

    /** \brief print the list of origins (from where the macros originates) to std::cout.
     */
    void printOrigins() const;


    /** \brief get the number of macros that have multiple definitions.
     */
    unsigned countRedefined() const;

    /** \brief get the number of macros from the database that are incorrect or empty.
     */
    unsigned countIncorrectOrEmpty() const;

    /** \brief Compare a list of macro collection using special options.
     *
     * \param mcs the other macro collection to be compared.
     * \param configuration  general configuration of the program.
     * \param param list of ordering and filtering rules. Please look at the implementation of the function for more information.
     */
    static void printDiffFromList(std::vector<MacroContainer*>& mcs, const Options& configuration, const std::vector<std::string>& param);

    /** \brief Compare two macro collection using special options.
     *
     * \param mcs the other macro collection to be compared.
     * \param configuration  general configuration of the program.
     * \param param list of ordering and filtering rules. Please look at the implementation of the function for more information.
     */
    void printDiff(std::vector<MacroContainer*>& mcs, const Options& configuration, const std::vector<std::string>& param) const;

    /** \brief get the list of the origins of the collection (files, folders, user...)
     *
     * \param v an array of string (each of them corresponds to a source from which the collection was loaded).
     */
    void getListOrigins(std::vector<std::string>& v) const;

    /** \brief get the list of the origins of the macros. It describes from where the macros originates (file, user, ect..).
     *
     * \return an array containing the list of origins.
     */
    const std::vector<std::string>& getListOrigins() const;

    /** \brief add a macro and its definition to the collection (and replace old macro(s) definition(s) ).
     *
     * \param macroName the name of the macro.
     * \param macroValue the definition of the macro.
     */
    void emplaceAndReplace(const std::string& macroName, const std::string& macroValue);

protected:
    /** \brief Add a new source (to track from where the imported macros come from).
     *
     * \param newOrigin a source (it could be file, the user..).
     */
    void addOrigin(const std::string& newOrigin);

private:
    /**< the database definitions */
    std::unordered_multimap< std::string, std::string > defines;
    /**< the sources of the database (it describes from where the macros come from) */
    std::vector< std::string > origins;
    /**< counts the number of macros tha thave the same name, but different definitions. */
    unsigned nbRedefined; // redefined macros are counted while loading a file
};

#endif // CONTAINER_HPP
