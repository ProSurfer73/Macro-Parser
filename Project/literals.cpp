/**
  ******************************************************************************
  * @file    hexa.cpp
  * @author  MCD Application Team
  * @brief   Macro-Parser
  *
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

#include <iostream>
#include <cctype>
#include <cmath>
#include <cstring>

#include "literals.hpp"

using std::string;

/// HEXADECIMAL CONVERSIONS. ///

static bool isStrictHexaLetter(char c)
{
    return ( (c>='0' && c<='9') || (c>='a' && c<='f') || (c>='A' && c<='F') );
}

bool isHexaLetter(char c)
{
    return isStrictHexaLetter(c)||c=='x'||c=='X';
}

static std::size_t findTheX(const std::string& str)
{
    // lets first try to look for a small x.
    auto searchedX = str.find('x');

    // if we didnt found anything, lets look for a big X.
    if(searchedX == std::string::npos)
        searchedX = str.find('X');

    // lets return the result of our search.
    return searchedX;
}


// This function looks for hexadecimal numbers and try to replace them by decimal number
void locateAndReplaceHexa(std::string& str, const Options& options)
{
    auto searchedX = findTheX(str);

    // As long as there is an hexadecimal number ( we locate it by the 'x' character and the 2 digits around it)
    while(searchedX != std::string::npos
    //&& isdigit(str[searchedX-1]) && isdigit(str[searchedX+1]) )
    && isStrictHexaLetter(str[searchedX-1]) && isStrictHexaLetter(str[searchedX+1]))
    {
        unsigned k = searchedX;
        while(isStrictHexaLetter(str[++k]));

        string endStr=string(&str[k]);
        string begStr=str.substr(0, searchedX-1);
        string midStr=str.substr(begStr.size(), str.size()-begStr.size()-endStr.size());

        /*cout << "endStr:" << endStr << "'" << endl;
        cout << "begStr:" << begStr << "'" << endl;
        cout << "midStr:" << midStr << "'" << endl;*/

        str = begStr + std::to_string(convertHexaToDeci(midStr)) + endStr;

        if(options.doesPrintExprAtEveryStep())
        {
            std::cout << str << std::endl;
        }

        searchedX = findTheX(str);
    }

}

long long convertHexaToDeci(const std::string& hex)
{
    long long decimal;
    int i = 0, val = 0, len;

    decimal = 0;

    /* Find the length of total number of hex digit */
    len = hex.size();
    len--;

    /*
     * Iterate over each hex digit
     */
    for(i=0; hex[i]!='\0'; i++)
    {

        /* Find the decimal representation of hex[i] */
        if(hex[i]>='0' && hex[i]<='9')
        {
            val = hex[i] - 48;
        }
        else if(hex[i]>='a' && hex[i]<='f')
        {
            val = hex[i] - 97 + 10;
        }
        else if(hex[i]>='A' && hex[i]<='F')
        {
            val = hex[i] - 65 + 10;
        }
        else if(hex[i] != 'x' && hex[i] != 'X') {
            std::cerr << "/!\\ Unknown character: " << hex[i] << ". /!\\\n";
            break;
        }


        decimal += val * pow(16, len);
        len--;
    }

    #ifdef DEBUG_LOG_STRINGEVAL
    printf("Hexadecimal number = %s\n", hex.c_str());
    printf("Decimal number = %d\n", decimal);
    #endif

    return decimal;
}

string convertDeciToHexa(long int num)
{
   char arr[100];
   int i = 0;
   while(num!=0 && i<100) {
      int temp = 0;
      temp = num % 16;
      if(temp < 10) {
         arr[i] = temp + 48;
         i++;
      } else {
         arr[i] = temp + 55;
         i++;
      }
      num = num/16;
   }

   string myreturn;

   for(int j=i-1; j>=0; j--)
        myreturn += arr[j];

    return myreturn;
}

bool tryConvertToHexa(std::string& deciStr)
{
    long int myint = std::atol(deciStr.c_str());

    // Conversion to long int: okay
    // We only convert numbers > 250, otherwise they should be written as decimal numbers
    if(myint > 250)
    {
        // Let's reconvert it to hexa.
        deciStr = "0x";
        deciStr += convertDeciToHexa(myint);
        return true;
    }

    return false;
}


/// INTEGER SUFFIXES DELETION. ///

// lets delete integer suffixes from number contained inside of a string.
void locateAndReplaceEnding(std::string& str, const Options& options)
{

    restart:

    int startpos = -1;

    for(unsigned i=0; i<str.size(); ++i)
    {
        // Let's note it when we reach a digit
        if(startpos == (-1) && isdigit(str[i]))
        {
            startpos = i;
        }
        else if(startpos != (-1) && (str[i] == 'U' || str[i]=='L' || str[i]=='l' || str[i]=='u'))
        {
            //std::cout << "RRR:" << str.substr(startpos, i-startpos) << std::endl;

            //std::cout << "BBEF:" << str << std::endl;

            str = str.substr(0, i) + str.substr(i+1);

            //std::cout << "AFTE:" << str << std::endl;

            goto restart;
        }
        else if(!isdigit(str[i]))
            startpos = (-1);
    }


}


/// OCTAL CONVERSIONS. ///

static unsigned octalToDecimal(const string& s)
{
    unsigned occ=0;

    for(unsigned i=0; i<s.size(); ++i)
    {
        occ = occ + (s[i]-'0')*pow(8, s.size()-1-i);
    }

    return occ;
}

static bool isOctal(char c)
{
    return (c >= '0' && c <= '7');
}

void locateAndReplaceOctal(std::string& str, const Options& options)
{
    string s;

    for(unsigned i=1; i<str.size()+1; ++i)
    {
        if((i<2 || !isdigit(str[i-2])) && str[i-1]=='0' && isOctal(str[i]) && (i<2 || str[i-2]!='\'' || i<3 || !isdigit(str[i-3])))
        {
            s = str[i];
        }
        else if(!s.empty() && isdigit(str[i]))
        {
            if(isOctal(str[i]))
            {
                s += str[i];
            }
            else
            {
                s.clear();
            }
        }
        else if(!s.empty())
        {
            str.replace(i-s.size()-1, s.size()+1, std::to_string(octalToDecimal(s)));
            locateAndReplaceOctal(str, options);
            break;
        }
    }
}


/// BINARY CONVERSIONS. ///

void locateAndReplaceBinary(std::string& str, const Options& options)
{
    string binaryString;

    for(unsigned i=2; i<str.size()+1; ++i)
    {
        if(str[i-2]=='0' && (str[i-1]=='b' || str[i-1]=='B') && (str[i]=='0' || str[i]=='1') )
        {
            binaryString = str[i];
        }
        else if(!binaryString.empty() && (str[i]=='0' || str[i]=='1'))
        {
            binaryString += str[i];
        }
        else if(!binaryString.empty())
        {
            unsigned result = strtol(binaryString.c_str(), nullptr, 2);
            str.replace(i-binaryString.size()-2, binaryString.size()+2, std::to_string(result));
            locateAndReplaceOctal(str, options);
            break;
        }
    }
}


/// APOSTROPHES DELETION. ///

void removeApostrophes(std::string& str)
{
    for(unsigned i=1; i<str.size(); ++i)
    {
        if(isdigit(str[i-1]) && str[i]=='\'' && isdigit(str[i+1]) )
        {
            str.erase(str.begin()+i);
            removeApostrophes(str);
            break;
        }
    }
}
