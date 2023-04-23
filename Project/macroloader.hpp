#ifndef MACROLOADER_HPP
#define MACROLOADER_HPP

#include "container.hpp"
#include "macrosearch.hpp"

// This class enables the capability of loading macros from files and folders from a Macrospace.
// Specifically, it contains the implementation related to it.

class MacroLoader : public MacroContainer
{
public:
    /** \brief import all macros contained in a file.
     *
     * \param filepath the path to the file we want to import.
     * \param config the list of options (preprocessor instructions interpretation enabled ?)
     * \return true if the file was correctly imported, false if not.
     */
    bool importFromFile(const std::string& filepath, const Options& config);

    /** \brief import all macros from all the files contained in a folder.
     *
     * \param folderpath the folder path.
     * \param config the list of options (preprocessor instructions interpretation enabled ?)
     * \return
     */
    bool importFromFolder(const std::string& folderpath, const Options& config);
};



#endif // MACROLOADER_HPP
