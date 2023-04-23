/**
  ******************************************************************************
  * @file    macrospace.hpp
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

#ifndef MACROSPACE_HPP
#define MACROSPACE_HPP

#include <vector>

#include "macroloader.hpp" // contains MacroLoader class
#include "container.hpp" // contains MacroContainer class


class Macrospaces
{
public:
    /** \brief Default constructor.
     */
    Macrospaces();

    /** \brief add a macrospace to this big database.
     *
     * \param macrospaceName the macrospace name attached to it.
     * \param macrospace the macrospace we want to add.
     */
    void addMacroSpace(const std::string& macrospaceName, const MacroLoader& macrospace);

    /** \brief obtain a macrospace from the database by its name. If it does not exists, it automatically creates an empty one with the name provided.
     *
     * \return the existing macrospace, or the new empty macrospace that has just been created.
     */
    MacroLoader& getMacroSpace(const std::string& macrospaceName);

    /** \brief check if a macrospace exists by its name.
     *
     * \return true if the macrospace with the name provided exists, false if it does not.
     */
    bool doesMacrospaceExists(const std::string& macrospaceName);


    MacroLoader* tryGetMacroSpace(const std::string& macrospaceName);
    void deleteMacroSpace(const std::string& macrospaceName);

    /** \brief show the list of all macrospaces and their content to the end user, using std::cout.
     */
    void printContentToUser();

    /** \brief get the number of macrospaces contained in this big database.
     */
    std::size_t size() const;

private:
    /** \brief update the msall macrospace. It is the macrospace that regroup all the macrospaces reunited.
     */
    void updateMsAll();

public:
    /**< the database containing all the macrospaces */
    std::vector< std::pair<std::string, MacroLoader> > macrospaces;
};




#endif // MACROSPACE_HPP
