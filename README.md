# Macro-Parser

Have you ever faced the situation in which you have to know the value of a complex macro in C/C++ inside of a project ?\
But this macro is defined by so much many other macros and hexadecimal values are used ?\
Calculating it manually would take too much time and energy, and you wouldn't be sure of the result.\
I have the solution for you: Macro Parser.\
Using a few command line you can calculate the value of almost any macro.\
This tool is only working on Windows for now, but ports can be easily created.

# About the tool

It's a tool I made during my apprenticeship at ST-Microelectronics.\
I had to go through very complex macros defined over hundreds of headers.

# Features
- Hexadecimal value conversion
- Incorrect macro detection
- Redefined macro detection
- Customizable options
- Clear and easy-to-use command line interface
- Warnings and errors
- Silent local conditional expression evaluation

# How to run for the first time
Go to the [release page](https://github.com/ProSurfer73/Macro-Parser/releases).\
Download the latest stable version.\
Double-click on the executable and you're done !

# How to build for the first time
Run make inside the folder Project of the repository.\
OR run "g++ -std=c++11 command.cpp container.cpp filesystem.cpp hexa.cpp main.cpp options.cpp stringeval.cpp -o appli.exe" inside the folder Project of the repo.\
OR you can download Code::Blocks https://www.codeblocks.org/downloads/binaries/ (version with MinGW installed), create a new project, add the files to it, rebuild everything from scratch, and run.

# Screenshots
![Screenshot1_v2](https://raw.githubusercontent.com/ProSurfer73/Macro-Parser/main/Screenshots/MacroParser1.png)
![Screenshot2_v2](https://raw.githubusercontent.com/ProSurfer73/Macro-Parser/main/Screenshots/MacroParser2.png)
