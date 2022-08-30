#include "filesystem.hpp"

bool hasEnding (std::string const &fullString, std::string const &ending)
{
    if (fullString.length() >= ending.length()) {
        return (0 == fullString.compare (fullString.length() - ending.length(), ending.length(), ending));
    } else {
        return false;
    }
}

void read_directory(const std::string& name, stringvec& v)
{
    std::string pattern(name);
    pattern.append("\\*");
    WIN32_FIND_DATA data;
    HANDLE hFind;
    if ((hFind = FindFirstFile(pattern.c_str(), &data)) != INVALID_HANDLE_VALUE) {
        do {
            v.emplace_back(data.cFileName);
        } while (FindNextFile(hFind, &data) != 0);
        FindClose(hFind);
    }
}


BOOL DirectoryExists(LPCTSTR szPath)
{
  DWORD dwAttrib = GetFileAttributes(szPath);

  return (dwAttrib != INVALID_FILE_ATTRIBUTES &&
         (dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}



bool readFile(const string& pathToFile, MacroContainer& macroContainer)
{
    ifstream file(pathToFile);

    if(!file.is_open())
        return false;

    #ifdef DEBUG_LOG_FILE_IMPORT
        cout << "Opened " << pathToFile << endl;
    #endif

    char defineStr[] = "#define ";
    unsigned posDefineStr = 0;

    char ifdefStr[] = "#ifdef ";
    unsigned posIfdefStr=0;

    char elifStr[] = "#elif ";
    unsigned posElifStr=0;

    char elseStr[] = "#else";
    unsigned posElseStr=0;

    char endifStr[] = "#endif";
    unsigned posEndifStr=0;

    char characterRead;

    int posLineComment=0;

    std::vector<char> keepTrack={1};

    std::vector<std::string> localMacroNames;



    bool insideConditions=false;

    while(file.get(characterRead))
    {
        /// avoid to load defines that are commented
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

        #ifdef IGNORE_MACRO_INSIDE_LONG_COMMENT

        else if(characterRead == '*' && posLineComment==1)
        {
            // We skip everything until we reach the end of comment "*/"
            char previousRead='r';

            while(file.get(characterRead))
            {
                if(previousRead=='*' && characterRead=='/') {
                    break;
                }
                previousRead=characterRead;
            }

            posLineComment=0;
        }

        #endif


        if(characterRead == defineStr[posDefineStr])
        {
            posDefineStr++;

            //
            if(posDefineStr >= 8)
            {
                posDefineStr = 0;

                string str1;

                /// We get the identifier

                // We skip spaces first
                while(file.get(characterRead)){
                    if(characterRead == '\n')
                        continue;
                    else if(characterRead != ' '){
                        str1 += characterRead;
                        break;
                    }
                }
                // Then we load the identifier (the complete word)
                while(file.get(characterRead) && characterRead != ' '){
                        if(characterRead == '\n')
                            continue;
                        str1 += characterRead;
                }

                string str2;

                // We get the value
                while(file.get(characterRead) && characterRead != '\n')
                {
                    str2 += characterRead;
                }

                // Deal with comment that could appear on str2
                auto look = str2.find("//");
                if(look == string::npos) look = str2.find("/*");
                if(look != string::npos){
                    str2 = str2.substr(0,look);
                }

                #ifdef DEBUG_LOG_FILE_IMPORT
                cout << str1 << " => " << str2 << endl;
                #endif

                clearSpaces(str1);
                clearSpaces(str2);

                // Ignore single letter macro and empty ones
                if(str1.size()==1)
                    continue;


                // If it is a multiple line macro
                while(str2.back() == '\\')
                {
                    str2.pop_back();

                    string inpLine;

                    // we get the next line
                    getline(file, inpLine);
                    clearSpaces(inpLine);
                    str2 += inpLine;

                    auto look = str2.find("//");
                    if(look == string::npos){
                        look = str2.find("/*");
                        if(look != string::npos){
                            char k='a';
                            while(file.get(characterRead)){
                                if(k=='*' && characterRead=='/')
                                    break;
                                k=characterRead;
                            }
                        }
                    }
                    else {
                        str2 = str2.substr(0,look);
                    }
                }

                // If the importer has priority order
                if(keepTrack.back()>=0)
                    macroContainer.emplace(str1, str2);

                if(!insideConditions || keepTrack.back()>=1)
                    localMacroNames.emplace_back(str1);
            }

        }
        else
        {
            posDefineStr = 0;
        }

        // #ifdef, #elif, #else, #endif detection mechanism
        if(characterRead == ifdefStr[posIfdefStr]){ posIfdefStr++; } else { posIfdefStr=0; }
        if(characterRead == elifStr[posElifStr]){ posElifStr++; } else { posElifStr=0; }
        if(characterRead == endifStr[posEndifStr]){ posEndifStr++; } else { posEndifStr=0; }
        if(characterRead == elseStr[posElseStr]){ posElseStr++; } else { posElseStr=0; }

        // If we detected ifdef
        if( posIfdefStr==7 )
        {
            insideConditions=true;
            posIfdefStr=0;

            string macroNameRead;
            while(file.get(characterRead))
            {
                if(characterRead == ' ' && !macroNameRead.empty())
                    break;
                else if(isMacroCharacter(characterRead))
                    macroNameRead += characterRead;
                else
                    break;
            }

            if(std::find(localMacroNames.begin(), localMacroNames.end(), macroNameRead) != localMacroNames.end())
            {
                keepTrack.push_back(1);
            }
            else
            {
                keepTrack.push_back(0);
            }
        }

        // If we detected elif
        if(posElifStr==6 || posElseStr==5)
        {
            //
            if(keepTrack.back()==1)
            {
                keepTrack[keepTrack.size()-1] = -1;
                posElifStr=0;
                posElseStr=0;
            }
        }

        // If we detected endif
        if(posEndifStr == 6)
        {
            string str1;

            insideConditions=keepTrack.size()>1;
            keepTrack.pop_back();
            posEndifStr=0;
        }
    }



    return true;
}



void explore_directory(std::string directory_name, stringvec& fileCollection)
{
   stringvec sv;

    // We look for all the files and folders that are in that directory
    read_directory(directory_name, sv);


    // We explore all teh different inputs
    for(size_t i=0; i < sv.size(); ++i)
    {
        // IF there is a point then, it's a file
        // Also it's important to note that filenames "." and ".." aren't files
        // oldimplemntation: if(sv[i].find('.') != std::string::npos && sv[i]!="." && sv[i]!="..")
        if(!DirectoryExists((directory_name+'\\'+sv[i]).c_str()) && sv[i]!="." && sv[i]!="..")
            fileCollection.emplace_back(directory_name+'\\'+sv[i]);

        // Else it's a folder, and you have to reexecute the function recursilvely
        else if(sv[i]!="." && sv[i]!=".."){
            explore_directory
            (directory_name+'\\'+sv[i], fileCollection);
        }
    }
}

#ifdef ENABLE_FILE_LOADING_BAR


static void printNbFilesLoaded(std::mutex& mymutex, bool& ended, unsigned& nbFiles, const unsigned maxNbFiles)
{
    // First initial delay before starting to diplay loading status
    const auto start = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

    int delayFirstSleep = 4000;

    while(delayFirstSleep > 0)
    {
        Sleep(120);
        delayFirstSleep -= 120;

        mymutex.lock();
        bool myend = ended;
        mymutex.unlock();

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
            #if (defined DEBUG_LOG_FILE_IMPORT && !(defined DEBUG_LOG_FILE_IMPORT)) && defined ENABLE_MUTEX_LOADINGBAR
                mymutex.lock();
                unsigned currentNbFiles = nbFiles;
                mymutex.unlock();
            #else
                unsigned currentNbFiles = nbFiles;
            #endif

            #if defined DEBUG_LOG_FILE_IMPORT && defined ENABLE_MUTEX_LOADINGBAR
            mymutex.lock();
            #endif // DEBUG_LOG_FILE_IMPORT

            cout << '[' << currentNbFiles*100/static_cast<float>(maxNbFiles) << "%] " << currentNbFiles << " files over " << maxNbFiles << " are loaded. ~"
            << maxNbFiles*(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count()-start)/(currentNbFiles+1)/60000 << "min left\n" ;

            #if defined DEBUG_LOG_FILE_IMPORT && defined ENABLE_MUTEX_LOADINGBAR
            mymutex.unlock();
            #endif // DEBUG_LOG_FILE_IMPORT

            notEverytime = 0;
        }


        Sleep(400);

        mymutex.lock();
        bool myend = ended;
        mymutex.unlock();

        if(myend)
            break;
    }
}

#endif

bool readDirectory(string dir, MacroContainer& macroContainer, const bool loadOnlySourceFile)
{
    stringvec fileCollection;

    explore_directory(dir, fileCollection);

    if(fileCollection.empty())
        return false;

    #ifdef ENABLE_FILE_LOADING_BAR
    std::cout << std::setprecision(3);
    bool ended = false;
    unsigned localNbFile = 0;
    unsigned nbFiles = 0;
    std::mutex mymutex;
    std::thread tr = std::thread(printNbFilesLoaded, std::ref(mymutex), std::ref(ended), std::ref(nbFiles), fileCollection.size());
    #endif
    #ifdef DISPLAY_FOLDER_IMPORT_TIME
    auto now = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    #endif // DISPLAY_FOLDER_IMPORT_TIME

    for(const string& str: fileCollection)
    {
        if(!loadOnlySourceFile || hasEnding(str, ".h") || hasEnding(str, ".c") || hasEnding(str, ".cpp") || hasEnding(str, ".hpp"))
        {
            if(!readFile(str, macroContainer)){
                std::cerr << "Couldn't read/open file : " << str << endl;
            }


        }

        #ifdef ENABLE_FILE_LOADING_BAR
        localNbFile++;
        #endif

        #ifdef ENABLE_FILE_LOADING_BAR
            #ifdef ENABLE_MUTEX_LOADINGBAR
            mymutex.lock();
            #endif
            nbFiles = localNbFile;
            #ifdef ENABLE_MUTEX_LOADINGBAR
            mymutex.unlock();
            #endif
        #endif
    }

    #ifdef ENABLE_FILE_LOADING_BAR
    mymutex.lock();
    ended = true;
    mymutex.unlock();
    tr.join();
    #endif
    #ifdef DISPLAY_FOLDER_IMPORT_TIME
    auto importTime = (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count()-now);
    cout << "Import time: " << importTime << " ms.\n";
    #endif // DISPLAY_FOLDER_IMPORT_TIME


    return true;
}

