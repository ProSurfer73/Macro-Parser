#ifndef MACROSEARCH_HPP
#define MACROSEARCH_HPP

#include <string>
#include <vector>
class Options;
using std::string;


/// Functions needed inside macroLoader

/** \brief remove the short comment part from a source code line
 *         for example: "while(1) // infinite loop" => "while(1) ".
 *
 * \param str the string to be shortened if it contains a short comment.
 * \return true if something was removed, false if not.
 */
bool destructShortComment(std::string& str);

/// Look for a specific macro among a folder

/** \brief search for a macro name defintion inside a file.
 *
 * \param pathToFile the path to the file in which we want to find the macro.
 * \param macroName the name of the macro we want to find.
 * \param config user configuration.
 * \return true if the macro definition exists in the file, false if not or if the file could not be opened.
 */
bool searchFile(const string& pathToFile, const std::string& macroName, const Options& config);

/** \brief look for a macro name definition maong files contained inside a folder.
 *
 * \param dir the directory name.
 * \param macroName the name of the macro we are looking for.
 * \param config the configuration (list of options set by the user).
 * \param previousResults the list of the files found are going ot be added to this array.
 * \return false if no file could be listed inside the given directory or if the directory could not be opened, true otherwise.
 */
bool searchDirectory(string dir, const std::string& macroName, const Options& config, std::vector<std::string>& previousResults);

/// System related functions (to list files inside a directory)

/** \brief gives the complete list of the files contained inside a directory and its subdirectories.
 *
 * \param dirname the name of the directory.
 * \param files the complete list of the files contained inside the directory is going to be added to this array.
 *
 */
void explore_directory(std::string dirname, std::vector<std::string>& files);

/** \brief check if the path provided by the end user corresponds to directory or not.
 *
 * \param basepath the directory path
 * \return true if the operating system tells there is directory at this location, false if not.
 */
bool directoryExists(const char* basepath);


#endif // MACROSEARCH_HPP
