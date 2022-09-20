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

// Fucntions available

BOOL DirectoryExists(LPCTSTR szPath);

bool readFile(const string& pathToFile, MacroContainer& macroContainer, const Options& config);

<<<<<<< Updated upstream
bool readDirectory(string dir, MacroContainer& macroContainer, const Options& config);
=======
// To detect words, when reading the file character by character
class WordDetector
{
public:
    WordDetector(const char* initialstr);
    bool receive(char character);

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

friend void threadImportFolder(const std::vector<std::string>* v, const Options* config, unsigned start, unsigned thesize, MacroDatabase* macroContainer);
friend class MacroDatabase;
};

bool searchFile(const string& pathToFile, const std::string& macroName, const Options& config);
bool searchDirectory(string dir, const std::string& macroName, const Options& config);

#if defined(_WIN32) || defined(_WIN64)

#else

#endif
>>>>>>> Stashed changes

#endif // FILESYSTEM_HPP
