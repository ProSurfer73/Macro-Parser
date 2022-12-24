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
#include "container.hpp"

class Macrospaces
{
public:
    Macrospaces();

    // Macrospaces related methods
    void addMacroSpace(const std::string& macrospaceName, const MacroContainer& macrospace);
    MacroContainer& getMacroSpace(const std::string& macrospaceName);
    bool doesMacrospaceExists(const std::string& macrospaceName);
    MacroContainer* tryGetMacroSpace(const std::string& macrospaceName);
    void deleteMacroSpace(const std::string& macrospaceName);
    void printContentToUser();
    std::size_t size() const;

private:
    void updateMsAll();

public:
    std::vector< std::pair<std::string, MacroContainer> > macrospaces;
};




#endif // MACROSPACE_HPP
