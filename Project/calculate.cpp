#include <fstream>
#include <cctype>

#include "calculate.hpp"

bool scanMainFile(const std::string& initialFile, const std::string& folderPath, const MacroContainer& macrocontainer)
{
    // let's open the file in read mode.
    std::ifstream file(initialFile);

    // if the file could not be opened.
    if(!file)
        return false;

    // line of text read from the file.
    std::string line;

    // list of file path contained inside the provided directory.
    std::vector<std::string> directoryContent;

    // let's list the content of the folder passed in parameters.
    explore_directory(initialFile, directoryContent);

    // the list of macros contained inside include statements.
    MacroLoader macroDatabase;

    // Let's read the file line by line
    while(std::getline(file, line))
    {
        // look for a string contained inside a string.
        std::size_t counter = line.find("#include");

        // if the include statement
        if(counter != std::string::npos)
        {
            // let's skip space characters.
            while(isspace(line[counter++]));

            // let's detect filename boundaries.
            if(line[counter] == '"')
            {
                // let's skip space characters.
                while(isspace(line[counter++]));

                // let's detect the filename.
                std::string filename;
                while(isalpha(line[counter]) || line[counter]=='.')
                {
                    filename += line[counter];
                    counter++;
                }

                // finally, let's open this file again and again.
                exploreAndLoad(directoryContent, macroDatabase, filename);

            }

        }


    }




    return true;
}

void exploreAndLoad(const std::vector<std::string>& folderPath, class MacroLoader& macrospace, const std::string& filename)
{

}

bool fileContainsMacro(const std::string& filepath, const std::string& macroName)
{
    // let's open the file in read mode.
    std::ifstream file(filepath);

    // let's quit if the could not be opened.
    if(!file)
        return false;

    // line areda from the file.
    std::string line;

    // let's read the file line by line.
    while(std::getline(file, line))
    {
        // let's look for the string '#define'.
        std::size_t pos = line.find("#define");

        if(pos != std::string::npos)
        {
            // let's skip space characters.
            while(isspace(line[pos]));


        }
    }

    return true;
}

