/**
  ******************************************************************************
  * @file    macrosearch.cpp
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

#include <fstream>
#include <thread>
#include <chrono>
#include <iomanip>
#include <atomic>
#include <iostream>
#include <algorithm>
#include <cctype>
#include "options.hpp"
#include "stringeval.hpp"
#include "config.hpp"
#include "macrosearch.hpp"


bool destructShortComment(std::string& str)
{
    auto searched = str.find("//");
    if(searched != std::string::npos)
    {
        str = str.substr(0, searched);
        return true;
    }

    return false;
}

bool searchFile(const string& pathToFile, const std::string& macroName)
{
    std::ifstream file(pathToFile);

    if(!file.is_open())
        return false;

    string str;

    while(file >> str)
    {
        if(str == "#define")
        {
            file >> str;
            destructShortComment(str);

            // let's shorten a macroName
            size_t pos = str.find('(');
            if(pos != std::string::npos
            && macroName==str.substr(0,pos))
                return true;

            // Let's compare it to the macroname we have.
            if(str == macroName)
                return true;
        }
    }

    return false;
}

bool searchDirectory(string dir, const std::string& macroName, const Options& config, std::vector<std::string>& previousResults)
{
    std::vector<std::string> fileCollection;
    explore_directory(dir, fileCollection);

    if(fileCollection.empty())
        return false;

    for(auto& str: fileCollection)
    {
        if(std::find(previousResults.begin(), previousResults.end(), str) == previousResults.end()
        && searchFile(str, macroName, config))
        {
            std::cout << str << std::endl;
            previousResults.emplace_back(std::move(str));
        }
    }

    return true;
}



#if (defined(_WIN32) || defined(_WIN64))

#include <windows.h>

void explore_directory(std::string dirname, std::vector<std::string>& files)
{
    WIN32_FIND_DATAA data;
    HANDLE hFind;

    if(dirname.back()!='\\')
        dirname += '\\';

    //std::cout << "dirname: " << dirname << std::endl;

    if ((hFind = FindFirstFileA((dirname+'*').c_str(), &data)) != INVALID_HANDLE_VALUE)
    {
        do
        {
            if(data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY){
                if(strcmp(data.cFileName, ".")!=0 && strcmp(data.cFileName, "..")!= 0){
                    explore_directory(dirname+data.cFileName,files);
                }

            }
            else {
                files.emplace_back(dirname+data.cFileName);
            }
            //Sleep(500);
        }
        while (FindNextFileA(hFind, &data) != 0);

        FindClose(hFind);
    }
}

bool directoryExists(const char* szPath)
{
  DWORD dwAttrib = GetFileAttributesA(szPath);

  return (dwAttrib != INVALID_FILE_ATTRIBUTES && (dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

#else

#include <dirent.h>
#include <sys/types.h>

void explore_directory(std::string basepath, std::vector<std::string>& vec)
{
    struct dirent *dp;
    DIR *dir = opendir(basepath.c_str());
    if(!dir)
        return;

    basepath += '/';


    while ((dp = readdir(dir)) != NULL)
    {


        if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0)
        {

            if(dp->d_type != DT_DIR){
                vec.emplace_back(basepath+dp->d_name);
            }
            else {
                // Construct new path from our base path
                    explore_directory(basepath+dp->d_name, vec);
            }
        }
    }

    closedir(dir);
}

bool directoryExists(const char* basepath)
{
    DIR *dir = opendir(basepath);
    bool isOpen = (dir != nullptr);

    if(isOpen)
        closedir(dir);

    return isOpen;
}

#endif
