/**
  ******************************************************************************
  * @file    config.hpp
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

#ifndef CONFIG_HPP
#define CONFIG_HPP

/// General config option

#define ENABLE_FILE_LOADING_BAR /**< if defined, prints a loading bar to the end user when loading a big folder. */
#define DISPLAY_FOLDER_IMPORT_TIME /**< if defined, it displays the time it took to import a folder. */
#define IGNORE_MACRO_INSIDE_LONG_COMMENT /**< if defined, ignores #define expressions contained inside long comments. */
#define OPTIONS_FILENAME "config.txt" /**< location at which the user configuration is saved. */
#define ENABLE_CLOSESTR /**< if defined, it allows approximation from 1 or 2 character when the user is very close from a command name. */

// Config parameters for the string evaluation


#define READ_HEXADECIMAL /**< If defined, the program will be able to read hexadecimal values. */
#define READ_OCTAL /**< If defined, the program will be able to read octal values. */
#define READ_BINARY /**< If defined, the program will be able to read binary values. */

/// Debugging options

#define DEBUG_ENABLE_ASSERTIONS
//#define DEBUG_LOG_FILE_IMPORT
//#define DEBUG_LOG_STRINGEVAL

#endif // CONFIG_HPP
