#include <iostream>
#include <iomanip>
#include <fstream>
#include <atomic>
#include <thread>
#include "stringeval.hpp"
#include "macrosearch.hpp"
#include "options.hpp"
#include "macroloader.hpp"
#include "config.hpp"
#include "strings.hpp"

/**< detect looks for keywords among source code, when reading a file character by character. */
class WordDetector
{
public:
    /** \brief Only possible constructor.
     *
     * \param initialstr the string we want to detect, when reading character one by one.
     */
    WordDetector(const char* initialstr)
    : str(initialstr), pos(0)
    {}

    /** \brief let's take into account a character read.
     *
     * \param character read from the file.
     * \return true if it matches with the complete keyword initially provided, false if it does not.
     */
    bool receive(char character)
    {
        if(str[pos]=='\r')
        {
            pos=0;
            if(isspace(character)||character=='(')
                return true;
        }

        else if(str[pos]==character
             ||(str[pos]==' ' && isspace(character)))
        {
            ++pos;
            if(str[pos]=='\0'){
                pos=0;
                return true;
            }
        }

        else
            pos=0;

        return false;
    }

private:
    /**< the string we want our detector to detect. */
    const char* str;
    /**< the position at which we are when matching with the keyword. */
    int pos;
};

static std::string extractDirPathFromFilePath(const std::string& filepath)
{
    if(filepath.find("\\") != std::string::npos)
            return filepath.substr(0, filepath.find_last_of('\\'));
    else
        return filepath.substr(0, filepath.find_last_of('/'));
}

/*static void skipShortComment(std::ifstream& file)
{
    char characterRead;
    while(file.get(characterRead) && characterRead != '\n');
}*/

static void skipLongComment(std::ifstream& stream)
{
    //std::cout << "=>long" << std::endl;

    // Let's skip the comment in the file, until we reach the end of it
    // Skip everything from here
    char k='a';
    char characterRead;
    while(stream.get(characterRead)){
        if(k=='*' && characterRead=='/')
            break;
        k=characterRead;
        //std::cout << characterRead;
    }

    //std::cout << "<=long" << std::endl;
}

static bool destructLongComment(std::string& str)
{
    auto searched = str.find("/*");

    // If there is a long comment
    if(searched != std::string::npos)
    {
        // Let's delete it from the current string
        str = str.substr(0, searched);
        return true;
    }

    return false;
}


static bool importFile(const char* pathToFile, MacroContainer& macroContainer, const Options& config, MacroContainer* origin)
{
    std::ifstream file(pathToFile);

    if(!file.is_open())
        return false;

    MacroContainer localContainer;

    #ifdef DEBUG_LOG_FILE_IMPORT
        std::cout << "Opened " << pathToFile << std::endl;
    #endif

    WordDetector defineDetector("#define ");
    WordDetector ifdefDetector("#ifdef ");
    WordDetector elifDetector("#elif\r");
    WordDetector elseDetector("#else");
    WordDetector endifDetector("#endif");
    WordDetector ifndefDetector("#ifndef ");
    WordDetector includeDetector("#include");
    WordDetector ifDetector("#if\r");

    char characterRead;

    int posLineComment=0;

    std::vector<char> keepTrack={1};

    bool firstInstruction=true;

    while(file.get(characterRead))
    {
        /// avoid to load defines that are commented

        if(!config.doesImportMacroCommented())
        {
            if(characterRead == '/')
            {
                posLineComment++;

                if(posLineComment == 2)
                {
                    // we skip the all line
                    while(file.get(characterRead) && characterRead != '\n');

                    // we reset the slash counter
                    posLineComment=0;
                }
            }

            else if(characterRead == '*' && posLineComment==1)
            {
                // We skip everything until we reach the end of comment "*/"
                skipLongComment(file);

                posLineComment=0;
            }

            else
                posLineComment = 0;
        }




        if(defineDetector.receive(characterRead))
        {
                firstInstruction = false;

                string str1;

                /// We get the identifier

                // We skip spaces first
                while(file.get(characterRead)){
                    if(characterRead == '\n')
                        break;
                    else if(!isspace(characterRead)){
                        str1 += characterRead;
                        break;
                    }
                }

                if(characterRead == '\n' && str1.empty())
                    continue;

                string str2;

                // Then we load the identifier (the complete word)
                while(file.get(characterRead) && !isspace(characterRead)){
                        if(characterRead == '\n'){
                            goto avoidValueGetting;
                        }
                        str1 += characterRead;
                }

                // We get the value
                while(file.get(characterRead) && characterRead != '\n')
                {
                    str2 += characterRead;
                }

                // Deal with comment that could appear on str2
                destructShortComment(str2);

                if(config.doesImportMacroCommented()){
                    // Let's skip the comment for the current line
                    destructLongComment(str2);
                }
                else {
                    // let's skip the long comment for multiple lines

                    // Is there a comment
                    relookForComment:
                    auto r1 = str2.find("/*");
                    auto r2 = str2.find("*/");
                    if(r1 < r2 && r1 != std::string::npos && r2 != std::string::npos){
                        // Let's delete this comment
                        str2 = str2.substr(0,r1)+=str2.substr(r2+2);
                        //std::cout << "str2: " << str2 << std::endl;

                        goto relookForComment;
                    }
                    else if(r1 != std::string::npos && r2 == std::string::npos) {
                        // Let's skip the comment inside the file
                        skipLongComment(file);
                    }

                    // let's try this first on my own project
                }

                clearSpaces(str1);
                clearSpaces(str2);

                avoidValueGetting:

                #ifdef DEBUG_LOG_FILE_IMPORT
                std::cout << str1 << " => " << str2 << std::endl;
                #endif


                // If it is a multiple line macro
                while(str2.back() == '\\')
                {
                    str2.pop_back();
                    std::cout << "2poped back!" << std::endl;

                    string inpLine;

                    // we get the next line
                    getline(file, inpLine);
                    clearSpaces(inpLine);
                    str2 += inpLine;

                    destructShortComment(str2);


                    /// Let's destruct long comments
                    if(config.doesImportMacroCommented()) {
                        // Let's skip the comment for the current line
                        destructLongComment(str2);
                    }
                    else {
                        // let's skip the long comment for multiple lines

                        // Is there a comment
                        relookForComment2:
                        auto r1 = str2.find("/*");
                        auto r2 = str2.find("*/");
                        if(r1 < r2 && r1 != std::string::npos && r2 != std::string::npos){
                            // Let's delete this comment
                            str2 = str2.substr(0,r1)+=str2.substr(r2+2);
                            //std::cout << "str2: " << str2 << std::endl;

                            goto relookForComment2;
                        }
                        else if(r1 != std::string::npos && r2 == std::string::npos) {
                            // Let's skip the comment inside the file
                            skipLongComment(file);
                        }

                        // let's try this first on my own project
                    }
                }


                // If the importer has priority order
                if((!origin && keepTrack.back()>=0)
                || (origin && keepTrack.back()>=1)){
                    //std::cout << "import: " << str1 << " --- " << str2 << "---" << (int)keepTrack.back() << std::endl;
                    macroContainer.emplace(str1, str2);
                    localContainer.emplace(str1, str2);
                }

                if(!config.doDisableInterpretations() && keepTrack.back()>=1){
                    //std::cout << "import: " << str1 << " --- " << str2 << "---" << (int)keepTrack.back() << std::endl;
                    localContainer.emplace(str1, str2);
                }

                else {
                    //std::cout << "notimported: " << str1 << " --- " << str2 << " -- " << (int)keepTrack.back() << std::endl;
                }

                // Debugging trace
                /*std::cout << "state {";
                for(char e: keepTrack) std::cout << (int)e << ", ";
                std::cout << "};" << std::endl;*/
        }

        if(!config.doDisableInterpretations())
        {

        // If we detected #if
        if(ifDetector.receive(characterRead))
        {
            //std::cout << "#if ";
            firstInstruction=false;

            string conditionStr;
            conditionStr += characterRead;

            while(file.get(characterRead) && characterRead != '\n')
            {
                conditionStr += characterRead;
            }

            clearSpaces(conditionStr);

            //std::cout << conditionStr << std::endl;

            // Let's create a new container with our local defines


            for(const auto& p : localContainer.getDefines())
            {
                if(doesExprLookOk(p.first)){
                    simpleReplace(conditionStr, (std::string("defined(")+=p.first)+=')', "true");
                    simpleReplace(conditionStr, std::string("defined ")+=p.first, "true");
                }
            }

            //std::cout << "conditionStr: " << conditionStr << endl;
            auto status = calculateExpression(conditionStr, localContainer, config, nullptr);

            //for(const string& str : localMacroNames)
            //{
            //    simpleReplace(conditionStr, std::string("defined(")+str+")", "true");
            //    simpleReplace(conditionStr, std::string("defined ")+str, "true");
            //}

            //std::cout << "conditionStr: " << conditionStr << endl;

            if(status == CalculationStatus::EVAL_OKAY )
            {
                if(conditionStr == "true")
                {
                    //std::cout << 1 << endl;
                    keepTrack.push_back(1);
                }
                else //if(conditionStr=="false")
                {
                    //std::cout << -1 << endl;
                    keepTrack.push_back(-2);
                }
            }
            else
            {
                //std::cout << "Not interpreted: " << conditionStr << endl;
                keepTrack.push_back(0);
            }
        }

        // If we detected ifdef
        if(ifdefDetector.receive(characterRead))
        {
            //std::cout << "#ifdef";

            firstInstruction=false;

            string macroNameRead;
            while(file.get(characterRead))
            {
                if(isspace(characterRead) && !macroNameRead.empty())
                    break;
                else if(isMacroCharacter(characterRead))
                    macroNameRead += characterRead;
                else
                    break;
            }

            clearSpaces(macroNameRead);

            //::cout << macroNameRead << std::endl;

            //std::cout << "macroNameRead: '" << macroNameRead << "'" << std::endl;

            //std::cout << "does '" << macroNameRead << "' exists?" << std::endl;
            if(localContainer.exists(macroNameRead))
            {
                //std::cout << "yes!" << std::endl;
                keepTrack.push_back(1);
            }
            else
            {
                keepTrack.push_back(0);
            }
            firstInstruction=false;
        }

        // If we detected ifndef
        if(ifndefDetector.receive(characterRead))
        {
            //std::cout << "entered ifndef" << std::endl;

            if(!firstInstruction)
            {
            string macroNameRead;
            while(file.get(characterRead))
            {
                if(isspace(characterRead) && !macroNameRead.empty())
                    break;
                else if(isMacroCharacter(characterRead))
                    macroNameRead += characterRead;
                else
                    break;
            }

            clearSpaces(macroNameRead);

            if(localContainer.exists(macroNameRead))
            {
                keepTrack.push_back(-2);
            }
            else
            {
                keepTrack.push_back(0);
            }

            }
            else
                keepTrack.push_back(1);

            firstInstruction=false;
        }

            if(elseDetector.receive(characterRead))
            {
                if(keepTrack.back()==1)
                {
                    keepTrack[keepTrack.size()-1] = -1;
                }
                else if(keepTrack.back()==-2)
                {
                    keepTrack[keepTrack.size()-1] = 0;
                }
            }

            // If it is #elif treat it like that
            if(elifDetector.receive(characterRead))
            {

                // If the previous condition was evaluated to false
                if(keepTrack.back() == -2)
                {
                    // Let's evaluate this one
                    string conditionStr;
                    conditionStr += characterRead;

                    while(file.get(characterRead) && characterRead != '\n')
                    {
                        conditionStr += characterRead;
                    }

                    clearSpaces(conditionStr);

                    // Let's create a new container with our local defines

                    for(const auto& p : localContainer.getDefines())
                    {
                        if(doesExprLookOk(p.first)){
                            simpleReplace(conditionStr, (std::string("defined(")+=p.first)+=')', "true");
                            simpleReplace(conditionStr, std::string("defined ")+=p.first, "true");
                        }
                    }

                    //MacroContainer localContainer;

                    //for(const std::pair<std::string,std::string>& p : macroContainer.getDefines())
                    //{
                    //    if(std::find(localMacroNames.begin(), localMacroNames.end(), p.first) != localMacroNames.end())
                    //        localContainer.emplace(p.first, p.second);
                    //}

                    //std::cout << "conditionStr: " << conditionStr << endl;
                    auto status = calculateExpression(conditionStr, localContainer, config, nullptr);

                    //for(const string& str : localMacroNames)
                    //{
                    //    simpleReplace(conditionStr, std::string("defined(")+str+")", "true");
                    //    simpleReplace(conditionStr, std::string("defined ")+str, "true");
                    //}

                    //std::cout << "conditionStr: " << conditionStr << endl;

                    if(status == CalculationStatus::EVAL_OKAY )
                    {
                        if(conditionStr == "true")
                        {
                            //std::cout << 1 << endl;
                            keepTrack[keepTrack.size()-1] = 1;
                        }
                        else if(conditionStr=="false" || conditionStr=="0")
                        {
                            //std::cout << -1 << endl;
                            keepTrack[keepTrack.size()-1] = 2;
                        }

                    }
                    else
                    {
                        //std::cout << "Not interpreted: " << conditionStr << endl;
                        keepTrack[keepTrack.size()-1] = 0;
                    }

                }
                // If the previous condition was evaluated to true
                else if(keepTrack.back() == 1)
                {
                    // Then this part should be ignored
                    keepTrack[keepTrack.size()-1] = -1;
                }
            }

        // If we detected endif
        if(endifDetector.receive(characterRead))
        {
            if(keepTrack.size()>1){
                //std::cout << "#endif => " << last << std::endl;
                keepTrack.pop_back();
                std::cout << "1poped back!" << std::endl;
            }

        }

        // If we detected #include
        if(includeDetector.receive(characterRead))
        {
            // Let's extract the filename
            string wholeWord;
            bool isGood=false;
            while(file.get(characterRead))
            {
                if(characterRead=='\n')
                    break;
                else if(characterRead=='"')
                {
                    if(!isGood)
                        isGood=true;
                    else
                        break;
                }
                else if(isGood)
                    wholeWord += characterRead;
            }

            if(!wholeWord.empty())
            {
                //std::cout << "include: '" << wholeWord << "'" << std::endl;

                string pathDir = extractDirPathFromFilePath(pathToFile);
                //std::cout << "asked import of: " << pathDir+'/'+wholeWord << std::endl;
                if(!origin && !config.doDisableInterpretations())
                    importFile((pathDir+'/'+wholeWord).c_str(), localContainer, config, &localContainer);
            }
        }

        }
    }

    // Let's move our local container to our global one
    //macroContainer.import(localContainer);

    return true;
}



bool MacroLoader::importFromFile(const std::string& filepath, const Options& config)
{
    if(importFile(filepath.c_str(), *this, config, nullptr)){
        this->addOrigin(filepath);
        return true;
    }
    return false;
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

static bool importDirectory(string dir, MacroContainer& macroContainer, const Options& config)
{
    std::vector<std::string> fileCollection;
    explore_directory(dir, fileCollection);

    if(fileCollection.empty())
        return false;

    // Let's print the number of files loaded for debugging purposes
    std::cout << "Number of files listed: " << fileCollection.size() << std::endl;

    unsigned localNbFile = 0;
    #ifdef ENABLE_FILE_LOADING_BAR
    std::cout << std::setprecision(3);
    std::atomic<bool> ended(false);
    std::atomic<unsigned> nbFiles(0);
    std::thread tr = std::thread(printNbFilesLoaded, std::ref(ended), std::ref(nbFiles), fileCollection.size());
    #endif
    #ifdef DISPLAY_FOLDER_IMPORT_TIME
    auto now = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    #endif // DISPLAY_FOLDER_IMPORT_TIME

    // If interpretation is enabled
    std::unordered_multimap<std::string,MacroContainer> database;

    for(const std::string& str: fileCollection)
    {
        if(!config.doesImportOnlySourceFileExtension() || hasEnding(str, ".h") || hasEnding(str, ".c") || hasEnding(str, ".cpp") || hasEnding(str, ".hpp"))
        {
            try
            {
                MacroContainer mc;

                if(!importFile(str.c_str(), mc, config, nullptr)){
                    std::cerr << "Couldn't read/open file : " << str << std::endl;
                }
                else {
                    database.emplace(str, std::move(mc));
                }
            }
            catch(const std::exception& ex)
            {
                std::cerr << "An error has occured while trying to interpret this source file:" << std::endl;
                std::cerr << str << std::endl;
                std::cerr << "Exception message: " << ex.what() << std::endl;
            }



        }

        localNbFile++;

        #ifdef ENABLE_FILE_LOADING_BAR
        // let's write to our atomic variable
        nbFiles = localNbFile;
        #endif
    }

    #ifdef ENABLE_FILE_LOADING_BAR
    // let's write to our atomic variable&²
    ended = true;
    tr.join();
    #endif
    #ifdef DISPLAY_FOLDER_IMPORT_TIME
    auto importTime = (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count()-now);
    std::cout << "Import time: " << importTime << " ms.\n";
    #endif // DISPLAY_FOLDER_IMPORT_TIME

    for(auto& p: database) {
        macroContainer.import(p.second);
    }


    return true;
}

bool MacroLoader::importFromFolder(const std::string& folderpath, const Options& config)
{
    if(importDirectory(folderpath.c_str(), *this, config)){
        this->addOrigin(folderpath);
        return true;
    }
    return false;
}

bool importProjectFile(const std::string& filepath, MacroContainer& macroContainer)
{
    enum{IAR, NONE} fileType;

    // 1. Let's detect the type of file we are dealing with.
    if(hasEnding(filepath, ".ewp"))
    {
        fileType = IAR;
    }
    else
        // let's stop here, it does not match any project file known to the program.
        return false;



    // Let's open the file
    std::ifstream file(filepath);

    // if the file could not be opened.
    if(!file)
        return false;

    // Let's then deal with the file according to its type.
    if(fileType == IAR)
    {

    }


    return true;
}
