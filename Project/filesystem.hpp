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
#include "container.hpp"


//#define ENABLE_MUTEX_LOADINGBAR

using namespace std;

typedef std::vector<std::string> stringvec;

class Options;
class MacroDatabase;

// Fucntions available




class FileSystem
{
public:
    static bool searchFile(const string& pathToFile, const std::string& macroName, const Options& config);
    static bool searchDirectory(string dir, const std::string& macroName, const Options& config);
    static BOOL DirectoryExists(LPCTSTR szPath);

private:
     static bool importFile(const string& pathToFile, MacroDatabase& macroContainer, const Options& config);
     static bool importDirectory(string dir, MacroDatabase& macroContainer, const Options& config);

friend class MacroDatabase;
};

bool searchDirectory(string dir, const std::string& macroName, const Options& config);

#endif // FILESYSTEM_HPP
