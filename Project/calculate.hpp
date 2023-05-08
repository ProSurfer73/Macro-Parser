#ifndef CALCULATE_HPP
#define CALCULATE_HPP

/**< File related to the calculate command. */

#include <vector>
#include <string>
#include "macrospace.hpp"

/** \brief the purpose of this function is to look for #include statements and explore them.
 *
 * \param initialFile file in which the user wants the macro to be calculated.
 * \param folderPath the path of the folder containing the source and header files.
 *
 * \return true if the file was opened.
 */
bool scanMainFile(const std::string& initialFile, const std::string& folderPath, const class MacroContainer& macrocontainer);

/** \brief explore the folder and load files corresponding to a filename.
 *
 * \param folderPath the path
 * \param macrospace the database of macros containing.
 * \param filename the filename of the header file to be loaded.
 */
void exploreAndLoad(const std::vector<std::string>& folderPath, class MacroLoader& macrospace, const std::string& filename);

/** \brief this checks if a file contains a macro specified or not.
 *
 * \param filepath the file path of the file.
 * \param macroName the name of the macro.
 *
 * \return true if the file can be opened, false otherwise.
 */
bool fileContainsMacro(const std::string& filepath, const std::string& macroName);


#endif // CALCULATE_HPP
