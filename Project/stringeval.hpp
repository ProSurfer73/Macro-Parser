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
#include <sstream>
#include <cassert>

#include "config.hpp"
#include "hexa.hpp"
#include "command.hpp"
#include "options.hpp"

using namespace std;

struct MacroContainer;
enum class CalculationStatus { EVAL_ERROR, EVAL_WARNING, EVAL_OKAY };

bool isMacroCharacter(char c);

void lowerString(std::string& str);

void searchAndReplaceMacros(std::string& expr);

void clearSpaces(string& str);

void splitString(std::vector<std::string>& outputVector, std::string initialStr);

bool doesExprLookOk(const string& expr);

double evaluateSimpleArithmeticExpr(const std::string& expr);

bool simpleReplace(std::string& str, const std::string& from, const std::string& to);



enum CalculationStatus calculateExpression(string& expr, const MacroContainer& macroContainer, const Options& config, bool printWarnings=true);


#endif // STRINGEVAL_HPP
