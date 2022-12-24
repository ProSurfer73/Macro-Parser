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

/// Static options ///
// Options dealt with at compilation time


/// Dynamic options ///
// Options that can be modified and configured by the user at runtime

#include <ostream>
#include <string>

#include "config.hpp"

class Options
{
public:
    Options();
    void toStream(std::ostream& stream) const;
    bool changeOption(std::string s1, std::string& s2);

    // Getters
    bool doesImportOnlySourceFileExtension() const;
    bool doesImportMacroCommented() const;
    bool doesPrintReplacements() const;
    bool doesPrintExprAtEveryStep() const;
    bool doKeepListRedefinedMacros() const;
    bool doDisableInterpretations() const;

private:
    bool saveToFile(const char* filename) const;
    bool loadFromFile(const char* filename);
    static bool loadBooleanValue(std::string input, bool& boolean);
    void resetToDefault();

    bool importOnlySourceFileExtension;
    bool importMacroCommented; // should macro that are inside comment such as // and /* be imported ?
    bool printReplacements; //
    bool printExprAtEveryStep;
    bool keepListRedefinedMacros;
    bool disableInterpretations;
};


#endif // OPTIONS_HPP
