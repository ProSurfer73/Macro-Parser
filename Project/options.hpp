/**
  ******************************************************************************
  * @file    options.hpp
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

#ifndef OPTIONS_HPP
#define OPTIONS_HPP

// Options that can be modified and configured by the user at runtime.

#include <ostream>
#include <string>

class Options
{
public:
    /** \brief Default constructor. Sets default configuration.
     */
    Options();

    /** \brief outputs the current configuration to a string stream from the standard library.
     */
    void toStream(std::ostream& stream) const;

    /** \brief change an option value.
     *
     * \param s1 the name of the option.
     * \param s2 the new value to be affected to the option.
     * \return true if the value of the option was correctly changed.
     */
    bool changeOption(std::string s1, std::string& s2);

    // Getters, get the value of the options.
    bool doesImportOnlySourceFileExtension() const;
    bool doesImportMacroCommented() const;
    bool doesPrintReplacements() const;
    bool doesPrintExprAtEveryStep() const;
    bool doKeepListRedefinedMacros() const;
    bool doDisableInterpretations() const;

private:
    /** \brief saves the configuration to a given file name.
     *
     * \param filename the name of the file to which the conifguraiton will be saved.
     */
    bool saveToFile(const char* filename) const;

    /** \brief load the configuration from a given file name.
     *
     * \param filename the name of the file from which the ocnfiguration will be load.
     */
    bool loadFromFile(const char* filename);

    /** \brief convert a string to a boolean value when possible.
     *
     * \param input the string to be converted to a boolean value.
     * \param boolean a variable variable, that will contains the value of the convertion if it is possible.
     * \return true if the convertion occured successfully, false otherwise.
     */
    static bool loadBooleanValue(std::string input, bool& boolean);

    /** \brief reset the configuration to the default configuration.
     */
    void resetToDefault();

    /**< variables containing the configuration. */
    bool importOnlySourceFileExtension;
    bool importMacroCommented; // should macro that are inside comment such as // and /* be imported ?
    bool printReplacements; //
    bool printExprAtEveryStep;
    bool keepListRedefinedMacros;
    bool disableInterpretations;
};


#endif // OPTIONS_HPP
