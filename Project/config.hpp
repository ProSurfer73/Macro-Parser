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

#define ENABLE_FILE_LOADING_BAR
#define DISPLAY_FOLDER_IMPORT_TIME
#define DISPLAY_USER_INFO
#define IGNORE_MACRO_INSIDE_LONG_COMMENT
#define OPTIONS_FILENAME "config.txt"

// Config parameters for the string evaluation

#define READ_HEXADECIMAL

/// Debugging options

#define DEBUG_ENABLE_ASSERTIONS
//#define DEBUG_LOG_FILE_IMPORT
//#define DEBUG_LOG_STRINGEVAL

#endif // CONFIG_HPP
