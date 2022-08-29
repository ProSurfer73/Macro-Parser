#ifndef FILESYSTEM_HPP
#define FILESYSTEM_HPP

#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <windows.h>
#include <iostream>
#include <thread>
#include <mutex>

#include <iomanip>
#include <vector>
#include <fstream>
#include <sstream>
#include <windows.h>
#include <iostream>
#include <thread>
#include <mutex>
#include "stringeval.hpp"
#include "config.hpp"
#include "command.hpp"

//#define ENABLE_MUTEX_LOADINGBAR

using namespace std;

typedef std::vector<std::string> stringvec;

struct MacroContainer;

// Fucntions available

BOOL DirectoryExists(LPCTSTR szPath);

bool readFile(const string& pathToFile, MacroContainer& macroContainer);

bool readDirectory(string dir, MacroContainer& macroContainer, const bool loadOnlySourceFile);

#endif // FILESYSTEM_HPP
