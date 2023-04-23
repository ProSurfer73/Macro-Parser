/**
  ******************************************************************************
  * @file    command.hpp
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

#ifndef COMMAND_HPP
#define COMMAND_HPP

#include <string>
#include "macrospace.hpp"
#include "options.hpp"

/**< Our main program is here. This class interacts with the user using the console and is able to run string commands. */
class CommandManager
{
public:
    /** \brief Default constructor (default configuration is set).
     */
    CommandManager();

    /** \brief Set a specific configuration before launching our program.
     *
     *  \param options the configuration to be set.
     */
    CommandManager(const Options& options);

    /** \brief console manager. Interacts with the user using the console.
     */
    void dealWithUser();

    /** \brief run a specific command.
     *
     *  \param str the command to run.
     *
     *  \return true if the program should continue to be run, false if the user asked to quit.
     */
    bool runCommand(const std::string& str);

    /** \brief launch a script (a text file containing a serie of commands to be run).
     *
     * \param filepath the name of the file
     * \param printStatus true if the commands thems
     *
     * \return true if the file exists, false otherwise.
     */
    bool loadScript(const std::string& filepath, bool printStatus=false);

private:
    /**< contains a list of options easily modifiable by the end user. */
    Options configuration;
    /**< contains the macro database, with all the different macrospaces. */
    Macrospaces macrospaces;
};

#endif // COMMAND_HPP
