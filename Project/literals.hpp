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

/**< This file contains useful functions related to strings and hexadecimal values. */

#include <string>
#include "options.hpp"

/** \brief looks for hexadecimal values inside strings and try to replace them with decimal values.
 */
void locateAndReplaceHexa(std::string& str, const class Options& options);

/** \brief convert an hexadecimal value to a deicmal value.
 *
 * \param hex the hexadecimal value (in a string format).
 * \return the decimal value.
 *
 */
long long convertHexaToDeci(const std::string& hex);

/** \brief convert a decimal value to an hexadecimal value.
 *
 * \param num_decimal the decimal value to be converted to hexadecimal value.
 * \return the hexadecimal value (in a string format).
 */
std::string convertDeciToHexa(long int num_decimal);

/** \brief check if a provided letter is part of the hexadecimal representation.
 *
 * \param c the character we want to check.
 * \return true if the letter might be part of an hexadecimal representation, false if not.
 */
bool isHexaLetter(char c);

/** \brief lets try to convert a decimal value into a hexadecimal value
 *
 * \param deciStr the string of text in decimal to be modified into hexadecimal value.
 *
 * \return true if the convertion occured, false otherwise.
 */
bool tryConvertToHexa(std::string& deciStr);

/** \brief delete suffixes from integer expresssions.
 *
 * \param str the string where integer suffixes should be deleted.
 * \param options the program options.
 */
void locateAndReplaceEnding(std::string& str, const Options& options);

/** \brief automaitcally localize and convert all octal values contained inside a string of text.
 *
 * \param the string where octal values will be converted into decimal values.
 * \param options the program options.
 */
void locateAndReplaceOctal(std::string& str, const Options& options);

/** \brief remove apostrophes between figures inside of a string.
 *
 * \param str the string where apostrophes should be removed.
 */

void removeApostrophes(std::string& str);

#endif // HEXA_HPP
