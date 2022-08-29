#include "hexa.hpp"

using namespace std;



// This function looks for hexadecimal numbers and try to replace them by decimal number
void locateAndReplaceHexa(std::string& str, const Options& options)
{
    auto search = str.find('x');

    // As long as there is an hexadecimal number ( we locate it by the 'x' character and the 2 digits around it)
    while(search != std::string::npos
    && isdigit(str[search-1]) && isdigit(str[search+1]) )
    {

        unsigned k=search;
        while(isHexaLetter(str[++k]));

        string endStr=string(&str[k]);
        string begStr=str.substr(0, search-1);
        string midStr=str.substr(begStr.size(), str.size()-begStr.size()-endStr.size());

        /* cout << "endStr:" << endStr << "'" << endl;
        cout << "begStr:" << begStr << "'" << endl;
        cout << "midStr:" << midStr << "'" << endl; */

        str = begStr + to_string(convertHexaToDeci(midStr)) + endStr;

        if(options.doesPrintExprAtEveryStep())
        {
            cout << str << endl;
        }




        search = str.find('x');

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
        else if(hex[i] != 'x') {
            std::cerr << "/!\\\n";
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


bool isHexaLetter(char c)
{
    return ( (c>='0' && c<='9') || (c>='a' && c<='f') || (c>='A' && c<='F') );
}

string convertDeciToHexa(long int num)
{
   char arr[100];
   int i = 0;
   while(num!=0) {
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

