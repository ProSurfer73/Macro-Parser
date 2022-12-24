/**
  ******************************************************************************
  * @file    filesystem.hpp
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

#ifndef FILESYSTEM_HPP
#define FILESYSTEM_HPP

#include <string>
#include <vector>

using namespace std;

typedef std::vector<std::string> stringvec;

class Options;
class MacroDatabase;
class MacroContainer;

// Fucntions available

std::string extractDirPathFromFilePath(const std::string& filepath);


// To detect words, when reading the file character by character
class WordDetector
{
public:
    inline WordDetector(const char* initialstr);
    inline bool receive(char character);

private:
    const char* str;
    int pos;
};


class FileSystem
{
public:
    static bool directoryExists(const char* szPath);

private:
     static bool importFile(const char* pathToFile, MacroDatabase& macroContainer, const Options& config, MacroContainer* origin=nullptr);
     static bool importDirectory(string dir, MacroDatabase& macroContainer, const Options& config);

friend class MacroDatabase;
};

bool searchFile(const string& pathToFile, const std::string& macroName, const Options& config);
bool searchDirectory(string dir, const std::string& macroName, const Options& config, std::vector<std::string>& previousResults);

void listFilesRecursively(const char* basepath, std::vector<char*>& vec);


#endif // FILESYSTEM_HPP
