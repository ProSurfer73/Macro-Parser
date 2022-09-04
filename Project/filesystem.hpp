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
#include "options.hpp"

//#define ENABLE_MUTEX_LOADINGBAR

using namespace std;

typedef std::vector<std::string> stringvec;

struct MacroContainer;
class Options;

// Fucntions available

BOOL DirectoryExists(LPCTSTR szPath);

bool importFile(const string& pathToFile, MacroContainer& macroContainer, const Options& config);

bool importDirectory(string dir, MacroContainer& macroContainer, const Options& config);

bool searchFile(const string& pathToFile, const std::string& macroName, const Options& config);

bool searchDirectory(string dir, const std::string& macroName, const Options& config);


#endif // FILESYSTEM_HPP
