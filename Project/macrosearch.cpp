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


/** \brief check if a string has a specific ending.
 *
 * \param the full string.
 * \param the endind.
 * \return true if the full string has the string ending at its very end, false if it does not.
 */
bool hasEnding (std::string const &fullString, std::string const &ending)
{
    if (fullString.length() >= ending.length()) {
        return (0 == fullString.compare (fullString.length() - ending.length(), ending.length(), ending));
    } else {
        return false;
    }
}






#ifdef ENABLE_FILE_LOADING_BAR

static void printNbFilesLoaded(std::atomic<bool>& ended, std::atomic<unsigned>& nbFiles, const unsigned maxNbFiles)
{
    // First initial delay before starting to diplay loading status
    const auto start = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

    int delayFirstSleep = 4000;

    while(delayFirstSleep > 0)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(120));
        delayFirstSleep -= 120;

        // we read our atomic variable
        bool myend = ended;

        if(myend)
            break;
    }

    // We display the loading status, yeah

    int notEverytime=0;

    while(true)
    {
        ++notEverytime;

        if(notEverytime == 10)
        {
            // we read our atomic variable
            unsigned currentNbFiles = nbFiles;

            std::cout << '[' << currentNbFiles*100/static_cast<float>(maxNbFiles) << "%] " << currentNbFiles << " files over " << maxNbFiles << " are loaded. ~"
            << maxNbFiles*(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count()-start)/(currentNbFiles+1)/60000 << "min left\n" ;

            notEverytime = 0;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(400));

        // we read our atomic variable
        bool myend = ended;

        if(myend)
            break;
    }
}

#endif


bool searchFile(const string& pathToFile, const std::string& macroName, const Options& config)
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
    WIN32_FIND_DATA data;
    HANDLE hFind;

    if(dirname.back()!='\\')
        dirname += '\\';

    //std::cout << "dirname: " << dirname << std::endl;

    if ((hFind = FindFirstFile((dirname+'*').c_str(), &data)) != INVALID_HANDLE_VALUE)
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
        while (FindNextFile(hFind, &data) != 0);

        FindClose(hFind);
    }
}


bool directoryExists(const char* szPath)
{
  DWORD dwAttrib = GetFileAttributes(szPath);

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