/**
  ******************************************************************************
  * @file    stringeval.cpp
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
#include <sstream>
#include <cassert>
#include <algorithm>
#include <memory>
#include <cstring>

#include "stringeval.hpp"
#include "container.hpp"
#include "options.hpp"
#include "config.hpp"
#include "vector.hpp"
#include "strings.hpp"

using std::string;

static bool thereIsMacroLetter(const std::string& str)
{
    for(unsigned i=0; i<str.size(); ++i){
        if(isalpha(str[i]) && str[i] != 'x')
            return true;
    }
    return false;
}

static bool isOperationCharacter(char c)
{
    return (c=='+'||c=='-'||c=='*'||c=='/'||c=='%');
}

bool isMacroCharacter(char c)
{
    return (isalpha(c)||(c>='0' && c<='9')||(c=='_'));
}

bool isNumberCharacter(char c)
{
    return (isdigit(c)||c=='.');
}

static bool containsAlpha(const std::string& str)
{
    for(char c: str){
        if(isalpha(c))
            return true;
    }
    return false;
}

static bool containsOperation(const std::string& str)
{
    // Let's try to calculate the arithmetic expression without parenthesis
    for(char c: str){
        if(isOperationCharacter(c))
            return true;
    }
    return false;
}

static bool seemLikeNumber(const std::string& str)
{
    // If there are letters behind it then it is not a number
    for(char c: str){
        if(!(isdigit(c)||c=='.'))
            return false;
    }
    return true;
}

static void func(double &result, char op, double num)
{
    switch (op)
    {
        case '+':
            result += num;
            break;
        case '-':
            result -= num;
            break;
        case '*':
            result *= num;
            break;
        case '/':
            if(num==0)
                throw std::runtime_error("division by 0");
            result /= num;
            break;
        case '%':
            if(num<=0)
                throw std::runtime_error("modulo < 0");
            while(result>num) {
                result -= num;
            }
            break;
        default:
            throw std::runtime_error( std::string("Unrecognized character: ")+=op+='\n' );
            break;
    }
}

static string extractVeryRightPart(const std::string& expr, size_t pos)
{
    std::size_t i=pos;
    for(; i < expr.size() && expr[i]!='&' && expr[i]!='|'; ++i);
    return expr.substr(i);
}

static string extractVeryLeftPart(const string& expr, size_t pos)
{
    std::size_t i=0;
    for(; pos-i > 0 && (i==0 || (expr[pos-i-1]!='&' && expr[pos-i-1]!='|')); ++i);
    return expr.substr(0, pos-i);
}

static string extractRightPart(const string& expr, size_t pos)
{
    std::size_t i=pos;
    for(; i < expr.size() && expr[i]!='&' && expr[i]!='|'; ++i);
    return expr.substr(pos, i-pos);
}

static string extractLeftPart(const string& expr, size_t pos)
{
    std::size_t i=0;
    for(; pos-i > 0 && (i==0 || (expr[pos-i-1]!='&' && expr[pos-i-1]!='|')); ++i);
    return expr.substr(pos-i, i);
}

/** \brief suppress the paremetered macro with value inside an expression by its definition.
 *
 * \param expr the expression
 * \param pfirst the value of the macro.
 * \param psecond the definition of a macro.
 */
static void replaceParamMacro(string& expr, string pfirst, const string& psecond)
{
    // Let's remove some part of each expression.
    pfirst = pfirst.substr(0, pfirst.find('('));

    // For debugging purposes, let's print pfirst.
    /*std::cout << "iexpr: " << expr << std::endl;
    std::cout << "pfirst: " << pfirst << std::endl;
    std::cout << "psecond: " << psecond << std::endl;*/

    // Let's replace the string by another.
    size_t pos = expr.find(pfirst);

    if(pos == std::string::npos)
        std::cout << "!!!!!!!!!!!!!!!" << std::endl;

    //std::cout << "1:" << expr << std::endl;
    expr.erase(pos, expr.find(')',pos)-pos+1);
    //std::cout << "2:" << expr << std::endl;
    expr.insert(pos, psecond);

    //std::cout << "expr: " << expr << std::endl;
}


static bool evaluateSimpleBooleanExpr(string& expr)
{
    size_t initialExprSize = expr.size();

    // We don't deal with parentheses here
    if(!(expr.find(')')==string::npos && expr.find('(')==string::npos)){
        throw std::runtime_error("Internal error: Can't evaluate simple boolean expr with parenthesis.");
    }

    // First, let's treat AND operator
    std::size_t searchedOperator;
    while( (searchedOperator=expr.find("&&")) != string::npos)
    {
        string leftPart = extractLeftPart(expr,searchedOperator);
        string rightPart = extractRightPart(expr,searchedOperator+2);

        /*std::cout << "&&veryLeft: " << veryLeftPart << "'" << std::endl;
        std::cout << "&&veryRight: " << veryRightPart << "'" << std::endl;
        std::cout << "&&leftPart: " << leftPart << "'" << endl;
        std::cout << "&&rightPart: " << rightPart << "'" << endl;*/

        if(leftPart=="false" || rightPart=="false")
        {
            expr = (extractVeryLeftPart(expr, searchedOperator)+="false")+=extractVeryRightPart(expr, searchedOperator+2);
        }
        else if(leftPart=="true")
            expr = rightPart;//veryLeftPart+rightPart+veryRightPart;
        else if(rightPart=="true")
            expr = leftPart;//veryLeftPart+leftPart+veryRightPart;
        else
            break;

        //std::cout << "expr: " << expr << "'" << std::endl;
    }

    // Secondly, let's treat OR operator
    while( (searchedOperator=expr.find("||")) != string::npos )
    {
        string leftPart = extractLeftPart(expr,searchedOperator);
        string rightPart = extractRightPart(expr,searchedOperator+2);

        string veryLeftPart = extractVeryLeftPart(expr, searchedOperator);
        string veryRightPart = extractVeryRightPart(expr, searchedOperator+2);

        /*std::cout << "||veryLeft: " << veryLeftPart << "'" << std::endl;
        std::cout << "||veryRight: " << veryRightPart << "'" << std::endl;
        std::cout << "||leftPart: " << leftPart << "'" << endl;
        std::cout << "||rightPart: " << rightPart << "'" << endl;*/

        if(leftPart=="true" || rightPart=="true")
            expr = veryLeftPart+"true"+veryRightPart;
        else if(leftPart=="false")
            expr = veryLeftPart+rightPart+veryRightPart;
        else if(rightPart=="false")
            expr = veryLeftPart+leftPart+veryRightPart;
        else
            break;
    }

    // If expression does not have the same size then
    return expr.size()!=initialExprSize;
}


bool doesExprLookOk(const string& expr)
{
    // Empty => it doesn't look good
    if(expr.empty())
        return false;

    // Check parenthesis
    int remainingParenthesis=0;
    for(unsigned i=0; i<expr.size(); ++i){
        if(expr[i]=='(')
            remainingParenthesis++;
        else if(expr[i]==')'){
            remainingParenthesis--;
            if(remainingParenthesis<0)
                return false;
            if(i>0 && expr[i-1]=='(')
                return false;
        }
    }
    if(remainingParenthesis!=0)
        return false;

    // Two operator in a row => incorrect
    for(unsigned i=1; i<expr.size(); ++i){
        if(isOperationCharacter(expr[i]) && isOperationCharacter(expr[i-1]))
            return false;
    }

    // It contains # => Incorrect
    // Our program doesn't deal with ## yet.
    if(expr.find('#') != std::string::npos)
        return false;

    return true;
}

// The arithmetic expression must be correct and must NOT contain spaces.
double evaluateSimpleArithmeticExpr(const string& expr)
{
    // Let's allocate manually memory (for a operators[] and numbers[])
    unsigned nbOperators=0;
    for(char c: expr) {
        if(isOperationCharacter(c))
            ++nbOperators;
    }
    if(isOperationCharacter(expr.front()))
        nbOperators--;
    char* operators = (char*)new int8_t[nbOperators*sizeof(char)+(nbOperators+1)*sizeof(double)]; // operators.size => nbOperators
    double *numbers = (double*)((int8_t*)operators+sizeof(char)*nbOperators); // numbers.size => nbOperators+1

    std::istringstream mathStrm(expr);
    if(!(mathStrm >> numbers[0]))
        throw std::runtime_error("getting first argument simple arithmetic expression.");

    unsigned pos=0;
    while (mathStrm >> operators[pos] >> numbers[pos+1])
    { ++pos; }

    while(nbOperators > 0)
    {
        /// Look for the best position

        // If there are * or / or % operators.
        pos=0;
        for(unsigned i=0; i<nbOperators; ++i){
            if(operators[i] == '*' || operators[i]=='/' || operators[i]=='%')
                pos=i;
        }
        func(numbers[pos], operators[pos], numbers[1+pos]);

        // let's move each array, one step to the left
        for(unsigned i=pos; i<nbOperators; ++i)
            operators[i]=operators[i+1];
        for(unsigned i=pos+1; i<=nbOperators; ++i)
            numbers[i]=numbers[i+1];

        nbOperators--;
    }

    double myreturn=numbers[0];
    delete[] operators;
    return myreturn;
}

static bool treatOperationDouble(std::string& str, const std::string& operation, bool (*operateur)(double, double))
{
    std::size_t searchedCharacter = str.find(operation);

    if(searchedCharacter != std::string::npos)
    {
        std::string str1=str.substr(0,searchedCharacter);
        std::string str2=str.substr(searchedCharacter+operation.length());

        double value1=-1, value2=-1;

        try
        {
            value1=std::stod(str1);
            value2=std::stod(str2);
        }
        catch(const std::exception& ex)
        {
            if(operation=="==" && str1==str2)
            {
                str="true";
                return true;
            }
            else if(operation=="=="
                 &&((str1=="true" && str2=="false")
                 || (str1=="false"&& str2=="true")))
            {
                str="false";
                return true;
            }
            else if(operation == "!="
                &&((str1=="true" && str2=="false")
                 ||(str1=="false"&& str2=="true")))
            {
                str="true";
                return true;
            }
            else if(operation=="!="
                 &&((str1=="true" && str2=="true")
                 ||(str1=="false" && str2=="false")))
            {
                str="false";
                return true;
            }

            return false;
        }

        if((*operateur)(value1,value2))
            str = "true";
        else
            str = "false";

        return true;
    }

    return false;
}


static bool treatInterrogationOperator(std::string& expr, const MacroContainer& mc, const Options& config, std::vector<std::pair<std::string,std::string> >& redef, std::vector<std::string>* warnings)
{
    //std::cout << "interrogation:" << expr << std::endl;

    bool didSomething=false;
    std::size_t searchedInterrogation;

    // As long as we find an interrogation operator
    while((searchedInterrogation=expr.find('?')) != std::string::npos)
    {
        // not to much left
        int abcd=0;
        std::size_t thepos=searchedInterrogation;
        for(; thepos>0; --thepos){
            if(expr[thepos] == '(')
            {
                abcd--;
                if(abcd <= 0){
                    break;
                }
            }
            else if(expr[thepos] == ')')
                abcd--;
        }

        std::string theleft = expr.substr(thepos, searchedInterrogation);
        std::string theright = expr.substr(searchedInterrogation+1);

        /*std::cout << "theleft: " << theleft << std::endl;
        std::cout << "theright: " << theright << std::endl;*/

        // Let's evaluate the left part
        auto status = calculateExpression(theleft, mc, config, nullptr, true, nullptr, &redef);

        // If the boolean evaluation went well
        if(status == CalculationStatus::EVAL_OKAY)
        {
            std::size_t kk;

            if( (kk=theright.find(':')) != std::string::npos)
            {
                if(theleft == "false")
                {
                    // not too much right
                    int abcde=0;
                    std::size_t thepos2=kk;
                    for(; thepos2>0; --thepos2){
                        if(expr[thepos2] == ')')
                        {
                            abcde--;
                            if(abcde <= 0){
                                break;
                            }
                        }
                        else if(expr[thepos] == '(')
                            abcde--;
                    }

                    // todo: fix not too much right
                    expr = theright.substr(kk+1, thepos2-(kk+1));
                    didSomething=true;
                }

                else if(theleft == "true"){
                    expr = theright.substr(0, kk);
                    didSomething=true;
                }
                else
                    return didSomething;
            }
            else
            {
                // Let's stop the ? evaluation here
                return didSomething;
            }
        }

        // if we cannot treat this operator so we break
        else
        {
            return didSomething;
        }
    }

    return didSomething;
}


/** \brief Calculate an expression with macrolist
 *
 * \param expr expression
 * \param macroContainer the list of macros loaded
 * \param config parameters of the MacroParser
 * \param printWarnings show warnings with encountering redefined macros for instance
 * \param enableBoolean should boolean be evaluated inside the expression ?
 * \param outputs nullptr=>1 output, it replaces expr ; !0 => multiple outputs written to outputs vector
 * \param redef if you want to replace macros contained in macroContainer during the evaluation process
 * \return status
 *
 */
enum CalculationStatus calculateExpression(string& expr, const MacroContainer& macroContainer, const Options& config,
std::vector<std::string>* printWarnings, bool enableBoolean, std::vector<std::string>* outputs, std::vector< std::pair<std::string, std::string> >* redef)
{
    CalculationStatus status = CalculationStatus::EVAL_OKAY;

    const auto& dictionary = macroContainer.getDefines();

    unsigned replaceCounter = 0;
    unsigned arithmeticCounter = 0;
    unsigned booleanCounter = 0;

    bool thereIsOperation = false;
    bool deleteRedef = false;

    locateAndReplaceEnding(expr, config);

    try {

    if(!printWarnings && !doesExprLookOk(expr)){
        return CalculationStatus::EVAL_ERROR;
    }

    if(!redef){
         redef = new std::vector< std::pair<std::string, std::string> >();
         deleteRedef = true;
    }

    /// 0. Is the expression okay ?

    //if(expr[0] == '?')return CalculationStatus::EVAL_ERROR;

    // Remove spaces
    clearSpaces(expr);


    #ifdef DEBUG_ENABLE_ASSERTIONS
        assert(expr.find(' ') == string::npos);
    #endif

    #ifdef READ_HEXADECIMAL
        // Look and replace hexa
        locateAndReplaceHexa(expr, config);
    #endif


    /// 1. Search and replace macros

    bool repeat;

    do {
        repeat = false;
        string save_expr = expr;
        string maxSizeReplace;
        string maxSizeReplaceSig;


        // Look for the longest word to replace
        /// TO DO: To be replaced by a more efficient implementation.


        std::vector<const std::pair<string,string>* > cutted;

        string currentWord;
        for(unsigned i=0; i<=expr.size();++i)
        {
            char c = expr[i];

            if((isalpha(c)||(c=='_'))||(!currentWord.empty()&&isdigit(c)))
            {
                currentWord += c;
            }
            else if(!currentWord.empty())
            {
                /* Let's look for the word (old implementation).
                // If it is a parametered macro
                if(c=='(') {
                    currentWord += "(x)";
                }

                auto range = dictionary.equal_range(currentWord);

                for(auto it=range.first; it!=range.second; ++it) {
                    if(doesExprLookOk(it->first))
                        cutted.push_back((std::pair<string,string>*)&(*it));
                }*/

                //std::cout << "currentWord: " << currentWord << std::endl;

                /// Let's look for the word (new implementation).
                /// The implementation could be better (we have to go through all the binary tree).
                for(const std::pair<std::string,std::string>& p: dictionary)
                {
                    // if we find the occurence of the word.
                    if(startsWith(p.first,currentWord))
                    {
                        //std::cout << "pushed. first:" << p.first << " p.second:" << p.second << std::endl;
                        cutted.push_back(&p);
                    }
                }

                currentWord.clear();
            }
        }


        for(const std::pair<string,string>* pkpk: cutted)
        {
            auto& p = *pkpk;

            const string& mac = p.first;

            if(p.first.back()!=')')
            {
                #ifdef DEBUG_LOG_STRINGEVAL
                    cout << "found\n";
                    cout << p.first.size() << " --- " << maxSizeReplace << endl;
                #endif // DEBUG_LOG_STRINGEVAL

                if(p.first.size() >= maxSizeReplace.size() /*&& doesExprLookOk(p.first)*/){
                    maxSizeReplace = p.first;
                }
            }
            else
            {
                if(mac.size() > maxSizeReplaceSig.size() /*&& doesExprLookOk(p.first)*/)
                    maxSizeReplaceSig = mac;
            }
        }

        #ifdef DEBUG_LOG_STRINGEVAL
        cout << "g:" << maxSizeReplace << endl;
        #endif

        if(!maxSizeReplace.empty())
        {



        // Replace it
        auto range = dictionary.equal_range(maxSizeReplace);
        for(auto it=range.first; it!=range.second; ++it)
        {
            auto& p = *it;

            if(true/*p.first == maxSizeReplace*/ /*&& expr.find(p.first) != string::npos*/)
            {

                // If replacement strings aren't correct
                if((p.first.find('(') != string::npos && p.first.find(')') == string::npos)
                || (p.first.find('(') == string::npos && p.first.find(')') != string::npos)){
                    // Then there is a problem, we skip.

                    //return CalculationStatus::EVAL_ERROR;
                    continue;
                }

                auto searched = expr.find(p.first);
                if(searched != string::npos
                && (searched == 0 || !isMacroCharacter(expr[searched-1]))
                && (searched+p.first.size()>=expr.size() || !isMacroCharacter(expr[searched+p.first.size()])))
                {
                    const string* replacedBy = &(p.second);

                    // if the macro is redefined
                    if(range.first != range.second)
                    {
                        bool foundRedef=false;

                        for(const auto& pp: *redef)
                        {
                            if(pp.first == p.first)
                            {
                                replacedBy = &(pp.second);
                                foundRedef = true;
                            }
                            //std::cout << "'" << pp.first << "'" << std::endl;
                        }

                        if(printWarnings && std::find(printWarnings->begin(), printWarnings->end(), p.first) == printWarnings->end()){
                            printWarnings->emplace_back(p.first);
                        }

                        if(outputs && !foundRedef)
                        {
                            bool oneThing=false;

                            auto range = dictionary.equal_range(p.first);
                            for(auto it2=range.first; it2!=range.second; it2++)
                            {
                                const auto& pp = *it2;

                                if(pp.first == p.first && p.first != expr &&
                                std::find(outputs->begin(), outputs->end(), expr) == outputs->end())
                                {
                                    std::string anotherExpr = expr;
                                    while(simpleReplace(anotherExpr, p.first, pp.second));
                                    //redef->emplace_back(p.first, pp.second);


                                    bool ffound=false;
                                    for(auto it=redef->begin(); it != redef->end(); ++it){
                                        if(it->first == pp.first && it->second == pp.second){
                                            ffound=true;
                                            break;
                                        }
                                    }


                                    if(!ffound && redef->size()<1000)
                                    {
                                        oneThing=true;
                                        //blacklist.emplace_back(pp.first+' '+pp.second);
                                        redef->emplace_back(pp.first, pp.second);

                                        auto status = calculateExpression(anotherExpr, macroContainer, config, printWarnings, enableBoolean, outputs, redef);
                                        if(!anotherExpr.empty())
                                        {
                                            if(status == CalculationStatus::EVAL_OKAY)
                                                emplaceOnce(*outputs, std::move(anotherExpr) );
                                            else if(status == CalculationStatus::EVAL_ERROR){
                                                std::string sss="undefined:";
                                                sss += anotherExpr;
                                                emplaceOnce(*outputs, std::move(sss) );
                                                status = CalculationStatus::EVAL_WARNING;
                                            }
                                            else if(status == CalculationStatus::EVAL_WARNING){
                                                anotherExpr += '?';
                                                emplaceOnce(*outputs, std::move(anotherExpr) );
                                                status = CalculationStatus::EVAL_WARNING;
                                            }
                                        }
                                    }


                                }
                            }

                            if(oneThing)
                            {
                                expr.clear();

                                if(deleteRedef) delete redef;
                                return status;
                            }


                        }

                    }

                    // Before making a mistake let's check that it is not a parametered macro
                    if(p.first.find('(') == std::string::npos)
                    {


                    if(config.doesPrintReplacements()){
                        std::cout << "replaced '" << p.first << "' by '" << *replacedBy << '\'' << std::endl;
                    }

                    // finally, let's replace-it in the expression
                    simpleReplace(expr, p.first, *replacedBy);

                    }
                }

                if(config.doesPrintExprAtEveryStep())
                    std::cout << expr << std::endl;


                break;
            }
        }

        #ifdef READ_HEXADECIMAL
            // Look and replace hexa
            locateAndReplaceHexa(expr, config);
        #endif

        }
        if(!maxSizeReplaceSig.empty())
        {
            // Look for single parameter macro
            //auto range = dictionary.equal_range(maxSizeReplaceSig);
            auto range = std::make_pair(dictionary.begin(),dictionary.end());
            for(auto it=range.first; it!=range.second; ++it)
            {
                auto& p = *it;
                const string& mac = p.first;


                // Let's detect parameters inside the string
                std::size_t pos = mac.find('(');
                std::vector<char> paramNames;

                // if there seems to be parameters
                if(pos != std::string::npos
                && mac.back()==')')
                {
                    do {
                        if(isalpha(mac[pos+1]))
                            paramNames.push_back(mac[pos+1]);
                        else {
                            paramNames.clear();
                            break;
                        }
                    }
                    // let's look for the next parameter.
                    while(mac[(pos+=2)] == ',');
                }

                // if number of parameter > 0.
                if(!paramNames.empty())
                {
                    // let's print the parameters found for debugging purposes.
                    /*std::cout << '(';
                    for(char c: paramNames)
                        std::cout << c << "; ";
                    std::cout << ')' << std::endl;*/

                    // Let's delete the macro name from the initial expression.
                    std::string initialExpr = expr;
                    size_t ppp = mac.find('(');
                    //std::cout << "sbtr: " << mac.substr(0,ppp-1) << std::endl;
                    //std::cout << "iinitialExpr: " << initialExpr << std::endl;
                    size_t mypos = initialExpr.find(mac.substr(0,ppp-1));
                    if(mypos == std::string::npos){
                        continue;
                        //throw std::runtime_error("nope!");
                    }

                    initialExpr.erase(mypos, pos);

                    //std::cout << "initialExpr: " << initialExpr << std::endl;

                    // Let's detect parameters value from the string.
                    std::vector<std::string> paramValues;
                    //std::cout << "yuyu: " << expr << std::endl;
                    initialExpr = expr;
                    for(unsigned i=mypos+ppp; i<initialExpr.size(); ++i)
                    {
                        std::string value;

                        //
                        if(initialExpr[i] != ',' && initialExpr[i]!='(')
                        {
                            if(initialExpr[i] == ')')
                                break;

                            value += initialExpr[i];

                            // let's add a value to the expression.
                            paramValues.push_back(value);
                        }

                    }

                    // Let's print parameter values for debugging purposes.
                    /*std::cout << "*(";
                    for(const std::string& s: paramValues) {
                        std::cout << s << ';';
                    }
                    std::cout << ").\n";*/

                    // Let's replace the paramaterized macro with values by
                    // the parameterized macro with letters inside the expression.
                    initialExpr = expr;
                    replaceParamMacro(initialExpr, p.first, p.second);

                    // Let's replace for each parameter.
                    string klkl = "(x)";
                    for(unsigned i=0; i<paramValues.size() && i<paramNames.size(); ++i)
                    {
                        // let's define the string to be replaced.
                        klkl[1] = paramNames[i];

                        // let's replace it.
                        //std::cout << "y: " << klkl << " -> " << paramValues[i] << " inside " << initialExpr << std::endl;
                        while(simpleReplace(initialExpr, klkl, paramValues[i]));
                    }

                    // finally, let's replace the main expression.
                    expr = initialExpr;

                    //std::cout << "yes: " << initialExpr << std::endl;
                }

                #if 0

                // if the string has at the end "(x)", then it's a single param macro.
                if(mac[mac.size()-1] == ')'
                && isalpha(mac[mac.size()-2])
                && mac[mac.size()-3] == '(')
                {
                    // If it's the same macro.
                    if(expr.find(mac.substr(0,mac.size()-3)) != string::npos
                    &&(expr[expr.find(mac.substr(0,mac.size()-3))+(mac.size()-3)] == ' '
                    || expr[expr.find(mac.substr(0,mac.size()-3))+(mac.size()-3)] == '('))
                    {
                        // We delete the mac from the expr string.
                        string cop1 = expr;

                        simpleReplace(cop1, mac.substr(0,mac.size()-3), "");

                        // In string p.second ; replace '(x)' <= expr.
                        string cop2 = p.second;

                        // let's replace the middle string.
                        string klkl = "(x)";
                        klkl[1] = mac[mac.size()-2];

                        simpleReplace(cop2, klkl, cop1); // replace 'def' -> 'klm'.

                        if(config.doesPrintReplacements()){
                            std::cout << "rreplaced '" << p.first << "' by '" << p.second << '\'' << std::endl;
                        }

                        #ifdef DEBUG_LOG_STRINGEVAL
                        cout << "before: " << expr << "==" << mac << endl;
                        #endif
                        expr = cop2;

                        #ifdef DEBUG_LOG_STRINGEVAL
                        cout << "after: " << expr << endl;
                        #endif
                    }
                }

                #endif
            }


        }

        if(save_expr != expr)
        {
            #ifdef DEBUG_LOG_STRINGEVAL
                std::cout << "S&R:" << expr << std::endl;
            #endif
            if(config.doesPrintExprAtEveryStep()){
                std::cout << expr << std::endl;
            }

            repeat = true;

        }


        clearSpaces(expr);

        ++replaceCounter;
        if(replaceCounter > 500){
            throw std::runtime_error("Counter pb: please post an issue on Github");
        }
    }
    while(repeat);


    //restartArithmeticEval:

    /// 2. Can the expression be evaluated ?

    #ifdef DEBUG_LOG_STRINGEVAL
    cout << "blv:" << expr << endl;
    #endif



    if(thereIsMacroLetter(expr))
    {
        //cout << "thereisLetter" << endl;
        goto skipArithmeticOperations;
    }



    /// 3. Arithmetic operations


    #ifdef DEBUG_LOG_STRINGEVAL
    cout << "c:" << expr << endl;
    #endif



    while(expr.find('(') != string::npos && expr.find(')') != string::npos)
    {
        size_t posClosePar = expr.find_first_of(')');
        size_t posOpenPar = posClosePar;
        for(;posOpenPar>=0 && expr[posOpenPar]!='('; posOpenPar--);

        if(posOpenPar < 0){
            throw std::runtime_error("')' found but '(' not found.");
        }

        string toBeCalculated = expr.substr(posOpenPar+1, (posClosePar-1)-(posOpenPar+1) +1 )  ;

        //if(printWarnings)cout << "toBeCalculated: " << toBeCalculated << endl;

        thereIsOperation = containsOperation(toBeCalculated);
        /*thereIsOperation=false;
        for(unsigned m=0;m<toBeCalculated.size();++m){
            if(isOperationCharacter(toBeCalculated[m]))
                thereIsOperation=true;
        }*/

        #ifdef DEBUG_LOG_STRINGEVAL
            cout << "thereisop: " << thereIsOperation << endl;
        #endif


        if(thereIsOperation)
        {
            double value = evaluateSimpleArithmeticExpr(toBeCalculated);

            //std::cout << "there is op" << std::endl;
            expr = (expr.substr(0,posOpenPar) += std::to_string(value)) += expr.substr(posClosePar+1);
        }
        else
        {
            // If it is a number,
            bool isNumber=true;
            for(char c: toBeCalculated){
                if(!isdigit(c)&&c!='.')
                    isNumber=false;
            }

            if(isNumber)
            {
                // If it is a parametered macro just before the parenthesis, we skip
                if(posOpenPar > 0 && isMacroCharacter(expr[posOpenPar-1])){
                    //std::cout << "ALERTE" << std::endl;
                    break;
                }

                // We've got to remove the parenthesis around the number, for instance : (14.2) => 14.2

                /*string begStr = expr.substr(0,posOpenPar);
                string midStr = expr.substr(posOpenPar+1, (posClosePar-1)-(posOpenPar+1) +1 ) ;
                string endStr = &expr[posClosePar+1];*/

                string begStr = expr.substr(0,posOpenPar);
                string midStr = expr.substr(posOpenPar+1, (posClosePar-1)-(posOpenPar+1) +1 ) ;
                string endStr = &expr[posClosePar+1];

                expr = begStr + midStr + endStr;

                //std::cout << "after: " << expr << std::endl;
            }
            else{
                break;
            }


        }

            if(arithmeticCounter++ > 1000){
                throw std::runtime_error("counter error (arithmetic operations).");
            }

        clearSpaces(expr);

        if(config.doesPrintExprAtEveryStep())
            std::cout << expr << std::endl;
    }

    // If there is an operation character, the expr look good, and no parentheses
    if(containsOperation(expr) && doesExprLookOk(expr) && !containsAlpha(expr) && expr.find('(')==std::string::npos && expr.find(')')==std::string::npos)
    {
        //std::cout << "aa" << std::endl;
        expr = std::to_string(evaluateSimpleArithmeticExpr(expr));
        if(config.doesPrintExprAtEveryStep()){
            std::cout << expr << '.' << std::endl;
        }
    }



    //if(!doesExprLookOk(expr))return CalculationStatus::EVAL_ERROR;




    skipArithmeticOperations:


    // 4. Conditional operations

    if(enableBoolean)
    {


    do
    {

        string begStr;
        string subExpr;
        string endStr;

        repeat=false;

        // Treat the '!' binary operator

        if(simpleReplace(expr, "!false", "true")
        ||simpleReplace(expr, "!true", "false")
        ||simpleReplace(expr, "(true)", "true")
        ||simpleReplace(expr, "(false)", "false"))
            repeat=true;

        // Let's delete parenthesis
        else if(expr.find('(')!=std::string::npos
        && expr.find(')')!=std::string::npos)
        {
            size_t posClosePar = expr.find_first_of(')');
            size_t posOpenPar = posClosePar;

            for(;expr[posOpenPar]!='('; posOpenPar--);

            // lets here treat the case of the ? operator.
            size_t ppp = expr.find('?', posOpenPar);
            if(ppp != std::string::npos
            && ppp < posClosePar
            &&!(ppp>4 && strncmp(&expr[ppp-4], "true", 4)==0)
            &&!(ppp>5 && strncmp(&expr[ppp-5], "false", 5)==0)) {
                posClosePar = ppp-1;
                posOpenPar++;
            }

            begStr = expr.substr(0,posOpenPar);
            subExpr = expr.substr(posOpenPar+1, (posClosePar-1)-(posOpenPar+1) +1 )  ;
            endStr = &expr[posClosePar+1];

            // If it is a number, let's just remove the parentheses for now
            /*bool isOnlyNumbers=true;
            for(char c: subExpr){
                if(!(c == '.' || isdigit(c)))
                    isOnlyNumbers=false;
            }
            if(isOnlyNumbers)
            {
                expr = begStr+subExpr+endStr;
                repeat=true;
                goto restartArithmeticEval;
            }*/

            // Let's to reevaluate what is in the parenthesis
            std::string subExpr2 = subExpr;
            //std::cout << "entry1" << std::endl;
            auto status2 = calculateExpression(subExpr2, macroContainer, config, nullptr, enableBoolean, nullptr, redef);
            //std::cout << "end1" << std::endl;
            if(status2 == CalculationStatus::EVAL_OKAY
            && (begStr.empty() || !isMacroCharacter(begStr.back())))
            {
                //std::cout << "shorter" << std::endl;
                expr = begStr+subExpr2+endStr;
                repeat=true;
            }
        }
        else
        {
            subExpr = expr;
        }

        if(
        // Treat double comparaison operations
           treatOperationDouble(subExpr, ">", [](double d1, double d2){ return d1>d2; } )
        || treatOperationDouble(subExpr, "<", [](double d1, double d2){ return d1<d2; } )
        || treatOperationDouble(subExpr, "==", [](double d1, double d2){ return d1==d2; } )
        || treatOperationDouble(subExpr, "!=", [](double d1, double d2){ return d1!=d2; } )
        || treatOperationDouble(subExpr, ">=", [](double d1, double d2){ return d1>=d2; } )
        || treatOperationDouble(subExpr, "<=", [](double d1, double d2){ return d1<=d2; } )

        // Treat AND, OR boolean operations
        || evaluateSimpleBooleanExpr(subExpr)
        /*|| simpleReplace(subExpr, "true&&true", "true")
        || simpleReplace(subExpr, "true&&false", "false")
        || simpleReplace(subExpr, "false&&true", "false")
        || simpleReplace(subExpr, "false&&false", "false")

        || simpleReplace(subExpr, "true||true", "true")
        || simpleReplace(subExpr, "true||false", "true")
        || simpleReplace(subExpr, "false||true", "true")
        || simpleReplace(subExpr, "false||false", "false")*/
        )

        {
            expr = begStr+subExpr+endStr;
            repeat=true;
        }

        else if(treatInterrogationOperator(subExpr, macroContainer, config, *redef, printWarnings))
        {
            expr = begStr+subExpr+endStr;
            repeat=true;
            //goto restartArithmeticEval;
        }

        if(repeat)
        {
            booleanCounter++;

            if( config.doesPrintExprAtEveryStep()){
                std::cout << expr << std::endl;
            }
        }
    }
    while(repeat && booleanCounter < 100);

    //if(counter > 0) goto restartArithmeticEval;

    }


    #ifdef DEBUG_LOG_STRINGEVAL
    std::cout << "final:" << expr << endl;
    #endif

    double result;

    // Let's try to convert it to a number
    try
    {
        result = std::stod(expr);
    }
    catch(const std::exception& ex)
    {
        // Then it is not a numerical expression

        if(expr != "true" && expr != "false")
            status = CalculationStatus::EVAL_ERROR;

        if(deleteRedef) delete redef;
        return status;
    }




    // 5. If it is an integer, convert it to an integer.

    if(seemLikeNumber(expr))
    {
        if(status != CalculationStatus::EVAL_ERROR)
        {
            if(result == static_cast<double>(static_cast<int>(result)) )
                expr = std::to_string(static_cast<int>(result));
        }
    }
    else
    {
        // If it is not a number, error, error, error
        status = CalculationStatus::EVAL_ERROR;
    }



    if(deleteRedef) delete redef;
    return status;


    }
    catch(std::exception const& ex)
    {
        if(printWarnings)
            std::cout << "Eval error: " << ex.what() << std::endl;

        //else throw;

        if(deleteRedef) delete redef;
        return CalculationStatus::EVAL_ERROR;
    }
}


void calculateExprWithStrOutput(string& expr, const MacroContainer& macroContainer, const Options& options, bool expand, std::vector<std::pair<std::string,std::string> >* redef)
{
    std::vector<std::string> output;
                //std::cout << "Source 3" << std::endl;
                auto status = calculateExpression(expr, macroContainer, options, nullptr, true, &output, redef);
    auto& results = output;



    // Sort and remove duplicates
    /*auto& v = output;
    std::sort(v.begin(), v.end());
    v.erase(std::unique(v.begin(), v.end()), v.end());*/

    if(!results.empty())
    {
        expr.clear();

        // Sort and remove duplicates
        //auto& v = results;
        //std::sort(v.begin(), v.end());
        //v.erase(std::unique(v.begin(), v.end()), v.end());

        //
        for(unsigned i=0; i<results.size(); ++i){
            tryConvertToHexa(results[i]);
                expr += results[i];
                if(i<results.size()-1)
                    expr += ", ";
        }

        if(results.size()>1)
            expr += " ?";
    }
    else
    {
        tryConvertToHexa(expr);

        if(status == CalculationStatus::EVAL_ERROR)
            expr = "unknown:"+expr;
        else if(status == CalculationStatus::EVAL_WARNING)
        {
            expr += "??";
        }

    }

}

void listUndefinedFromExpr(std::vector<std::string>& missingMacros, const std::string& expr)
{
    std::string word;

    for(char c: expr)
    {
        if(std::isalpha(c) || c=='_')
            word += c;

        else if(std::isdigit(c) && !word.empty())
            word += c;

        else if(!word.empty())
        {
            missingMacros.push_back( std::move(word) );
            word.clear();
        }
    }

    if(!word.empty())
        missingMacros.push_back( std::move(word) );
}
