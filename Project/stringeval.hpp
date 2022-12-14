/**
  ******************************************************************************
  * @file    stringeval.hpp
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

#ifndef STRINGEVAL_HPP
#define STRINGEVAL_HPP

#include <vector>
#include <string>

#include "container.hpp"
#include "stringeval.hpp"
#include "hexa.hpp"

void clearBlacklist();

enum class CalculationStatus { EVAL_ERROR, EVAL_WARNING, EVAL_OKAY };

bool isMacroCharacter(char c);

void lowerString(std::string& str);

void searchAndReplaceMacros(std::string& expr);

void clearSpaces(std::string& str);

void splitString(std::vector<std::string>& outputVector, std::string initialStr);

bool doesExprLookOk(const std::string& expr);

double evaluateSimpleArithmeticExpr(const std::string& expr);

bool simpleReplace(std::string& str, const std::string& from, const std::string& to);

enum CalculationStatus calculateExpression(std::string& expr, const MacroContainer& macroContainer, const Options& config,
std::vector<std::string>* printWarnings=nullptr, bool enableBoolean=true, std::vector<std::string>* outputs=nullptr,
std::vector< std::pair<std::string, std::string> >* redef=nullptr);

void calculateExprWithStrOutput(std::string& expr, const MacroContainer& macroContainer,
            const Options& options, bool expand=true, std::vector<std::pair<std::string,std::string> >* redef=nullptr);

void listUndefinedFromExpr(std::vector<std::string>& missingMacros, const std::string& expr);


#endif // STRINGEVAL_HPP
