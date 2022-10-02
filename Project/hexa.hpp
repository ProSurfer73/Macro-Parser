/**
  ******************************************************************************
  * @file    hexa.hpp
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

#ifndef HEXA_HPP
#define HEXA_HPP

#include <math.h>
#include <string.h>
#include <iostream>
#include <string>
#include <ctype.h>

#include "options.hpp"

void locateAndReplaceHexa(std::string& str, const class Options& options);

long long convertHexaToDeci(const std::string& hex);

std::string convertDeciToHexa(long int num_decimal);

bool isHexaLetter(char c);

bool tryConvertToHexa(std::string& deciStr);

void locateAndReplaceEnding(std::string& str, const Options& options);

#endif // HEXA_HPP
