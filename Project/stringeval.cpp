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

#include "stringeval.hpp"

#include <iostream>

void clearSpaces(string& str)
{
    str.erase(std::remove(str.begin(), str.end(), ' '), str.end());
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

void lowerString(std::string& str)
{
    for(unsigned i=0;i<str.size();++i)
        str[i] = std::tolower(str[i]);
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
            {
                throw std::runtime_error("modulo < 0");
                break;
            }

            while(result>num){
                result -= num;
            }
            break;
        default:
            std::cout << "Unrecognized character: " << op << std::endl;
            break;
    }
}

static string extractRightPart(string expr, size_t pos)
{
    return expr.substr(pos);
}

static string extractLeftPart(string expr, size_t pos)
{
    std::size_t i=0;
    for(; pos-i >= 0 && isMacroCharacter(expr[pos-i-1]); ++i);
    return expr.substr(pos-i, i);
}


static bool evaluateSimpleBooleanExpr(string& expr)
{
    size_t initialExprSize = expr.size();

    // We don't deal with parenthesis here
    assert(expr.find('(')==string::npos);

    // First, let's treat AND operator
    std::size_t searchedOperator;
    while( (searchedOperator=expr.find("&&")) != string::npos)
    {
        string leftPart = extractLeftPart(expr,searchedOperator);
        string rightPart = extractRightPart(expr,searchedOperator+2);

        if(leftPart=="false" || rightPart=="false")
            expr = "false";
        else if(leftPart=="true")
            expr = rightPart;
        else if(rightPart=="true")
            expr = leftPart;
    }

    // Secondly, let's treat OR operator
    while( (searchedOperator=expr.find("||")) != string::npos )
    {
        string leftPart = extractLeftPart(expr,searchedOperator);
        string rightPart = extractRightPart(expr,searchedOperator+2);

        if(leftPart=="true" || rightPart=="true")
            expr = "true";
        else if(leftPart=="false")
            expr = rightPart;
        else if(rightPart=="false")
            expr = leftPart;
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
    std::vector<double> numbers;
    std::vector<char> operators;

    istringstream mathStrm(expr);
    double result = (-3);
    if(!(mathStrm >> result))
        throw std::runtime_error("getting first argument simple arthmetic expression.");
    numbers.push_back(result);
    char op;
    double num;
    while (mathStrm >> op >> num)
    {
        operators.push_back(op);
        numbers.push_back(num);
    }

    while(!operators.empty())
    {
        unsigned curPos = 0;

        /// Look for the best position

        // If there are * or / or % operators
        for(unsigned i=1;i<operators.size();++i){
            if(operators[i] == '*' || operators[i]=='/' || operators[i]=='%')
                curPos=i;
        }

        result = numbers[curPos];
        func(result, operators[curPos], numbers[1+curPos]);
        numbers.erase(numbers.begin()+curPos);
        numbers.erase(numbers.begin()+curPos);
        numbers.insert(numbers.begin()+curPos, result);
        operators.erase(operators.begin()+curPos);
    }

    return result;
}


bool simpleReplace(std::string& str, const std::string& from, const std::string& to)
{
    size_t start_pos = str.find(from);
    if(start_pos == std::string::npos)
        return false;
    str.replace(start_pos, from.length(), to);
    return true;
}


static bool treatOperationDouble(std::string& str, std::string operation, bool (*operateur)(double, double))
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
                 ||(str1=="true" && str2=="true")
                 ||(str1=="false" && str2=="false"))
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

/** \brief
 *
 * \param expr: macro given in input
 * \param macroContainer
 * \return status
 *
 */
enum CalculationStatus calculateExpression(string& expr, const MacroContainer& macroContainer, const Options& config, bool printWarnings)
{
    CalculationStatus status = CalculationStatus::EVAL_OKAY;

    const auto& dictionary = macroContainer.getDefines();
    const auto& redefinedMacros = macroContainer.getRedefinedMacros();
    const auto& incorrectMacros = macroContainer.getIncorrectMacros();

    unsigned counter = 0;

    std::vector<std::string> macroRedefinedWarning;

    try {

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
        for(const pair<string,string>& p: dictionary)
        {
            const string& mac = p.first;

            if(expr.find(p.first) != string::npos && doesExprLookOk(p.second))
            {
                #ifdef DEBUG_LOG_STRINGEVAL
                    cout << "found\n";
                    cout << p.first.size() << " --- " << maxSizeReplace << endl;
                #endif // DEBUG_LOG_STRINGEVAL

                if(p.first.size() >= maxSizeReplace.size()){
                    maxSizeReplace = p.first;
                }
            }
            else if(mac[mac.size()-1] == ')' && mac[mac.size()-2] == 'x' && mac[mac.size()-3] == '('
                && expr.find(mac.substr(0,mac.size()-3)) != string::npos)
            {
                maxSizeReplaceSig = mac;
            }
        }

        #ifdef DEBUG_LOG_STRINGEVAL
        cout << "g:" << maxSizeReplace << endl;
        #endif

        if(!maxSizeReplace.empty())
        {



        // Replace it
        for(pair<string,string> p: dictionary)
        {
            if(p.first == maxSizeReplace /*&& expr.find(p.first) != string::npos*/ && doesExprLookOk(p.second))
            {

                // If replacement strings aren't correct
                if((p.first.find('(') != string::npos && p.first.find(')') == string::npos)
                || (p.first.find('(') == string::npos && p.first.find(')') != string::npos)
                || (!doesExprLookOk(p.second))){
                    // Then there is a problem, we skip.

                    //return CalculationStatus::EVAL_ERROR;
                    continue;
                }

                if(config.doesPrintReplacements()){
                    std::cout << "replaced '" << p.first << "' by '" << p.second << "'" << endl;
                }

                auto searched = expr.find(p.first);
                if(searched != string::npos && !isMacroCharacter(expr[searched+1]) && searched>=1 && !isMacroCharacter(expr[searched+1]));
                    simpleReplace(expr, p.first, p.second);

                if(std::find(redefinedMacros.begin(), redefinedMacros.end(), p.first) != redefinedMacros.end()){
                    if(printWarnings){
                        if(macroRedefinedWarning.empty()
                        || std::find(macroRedefinedWarning.begin(), macroRedefinedWarning.end(), p.first) == macroRedefinedWarning.end()){
                            cout << "/!\\ Warning: the macro " << p.first << " you are using has been redefined /!\\" << endl;
                            macroRedefinedWarning.emplace_back(p.first);
                        }
                    }

                    status = CalculationStatus::EVAL_WARNING;
                }

                if(std::find(incorrectMacros.begin(), incorrectMacros.end(), p.first) != incorrectMacros.end()){
                    cout << "/!\\ Warning: the macro " << p.first << "you are using seem incorrect. /!\\" << endl;
                    status = CalculationStatus::EVAL_WARNING;
                }

                if(config.doesPrintExprAtEveryStep())
                    cout << expr << endl;


                break;
            }
        }

        #ifdef READ_HEXADECIMAL
            // Look and replace hexa
            locateAndReplaceHexa(expr, config);
        #endif

        }
        else if(!maxSizeReplaceSig.empty())
        {
            // Look for single parameter macro
            for(pair<string,string> p: dictionary)
            {
                string& mac = p.first;

                // if the string has at the end "(x)", then it's a single param macro
                if(maxSizeReplaceSig == mac
                && mac[mac.size()-1] == ')'
                && mac[mac.size()-2] == 'x'
                && mac[mac.size()-3] == '(')
                {
                    // If it's the same macro
                    if(expr.find(mac.substr(0,mac.size()-3)) != string::npos
                    &&(expr[expr.find(mac.substr(0,mac.size()-3))+(mac.size()-3)] == ' '
                    || expr[expr.find(mac.substr(0,mac.size()-3))+(mac.size()-3)] == '('))
                    {
                        // We delete the mac from the expr string
                        string cop1 = expr;

                        simpleReplace(cop1, mac.substr(0,mac.size()-3), "");

                        // In string p.second ; replace '(x)' <= expr
                        string cop2 = p.second;

                        simpleReplace(cop2, "(x)", cop1); // replace 'def' -> 'klm'

                        if(config.doesPrintReplacements()){
                            cout << "rreplaced '" << p.first << "' by '" << p.second << '\'' << endl;
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
            }


        }

        if(save_expr != expr)
        {
            #ifdef DEBUG_LOG_STRINGEVAL
                cout << "S&R:" << expr << endl;
            #endif
            if(config.doesPrintExprAtEveryStep()){
                cout << expr << endl;
            }

            repeat = true;

        }


        clearSpaces(expr);

        ++counter;
        if(counter > 1000){
            status = CalculationStatus::EVAL_ERROR;
            break;
        }
    }
    while(repeat);



    restartArithmeticEval:

    /// 2. Can the expression be evaluated ?

    #ifdef DEBUG_LOG_STRINGEVAL
    cout << "blv:" << expr << endl;
    #endif

    bool thereIsLetter = false;
    for(unsigned i=0; i<expr.size(); ++i){
        if(isalpha(expr[i]) && expr[i] != 'x')
            thereIsLetter = true;
    }

    if(thereIsLetter)
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
        for(;expr[posOpenPar]!='('; posOpenPar--);

        string toBeCalculated = expr.substr(posOpenPar+1, (posClosePar-1)-(posOpenPar+1) +1 )  ;

        //cout << "toBeCalculated: " << toBeCalculated << endl;

        bool thereIsOperation=false;
        for(unsigned m=0;m<toBeCalculated.size();++m){
            if(isOperationCharacter(toBeCalculated[m]))
                thereIsOperation=true;
        }

        #ifdef DEBUG_LOG_STRINGEVAL
            cout << "thereisop: " << thereIsOperation << endl;
        #endif


        if(thereIsOperation)
        {
            double value = evaluateSimpleArithmeticExpr(toBeCalculated);

            expr = (expr.substr(0,posOpenPar) + to_string(value)) + expr.substr(posClosePar+1);
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
                // We've got to remove the parenthesis around the number, for instance : (14.2) => 14.2

                string begStr = expr.substr(0,posOpenPar);
                string midStr = expr.substr(posOpenPar+1, (posClosePar-1)-(posOpenPar+1) +1 ) ;
                string endStr = &expr[posClosePar+1];

                expr = begStr + midStr + endStr;
            }
            else{
                //cout << "is not number" << endl;
                break;
            }


        }

        #ifdef DEBUG_ENABLE_ASSERTIONS
            ++counter;
            assert(counter < 100);
        #endif // DEBUG_ENABLE_ASSERTIONS

        clearSpaces(expr);

        if(config.doesPrintExprAtEveryStep())
            cout << expr << endl;
    }

    //if(!doesExprLookOk(expr))return CalculationStatus::EVAL_ERROR;




    skipArithmeticOperations:

    #define DEV
    #ifdef DEV

    // 4. Conditional operations


    counter=0;


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
        if(expr.find('(')!=std::string::npos
        && expr.find(')')!=std::string::npos)
        {
            size_t posClosePar = expr.find_first_of(')');
            size_t posOpenPar = posClosePar;

            for(;expr[posOpenPar]!='('; posOpenPar--);

            begStr = expr.substr(0,posOpenPar);
            subExpr = expr.substr(posOpenPar+1, (posClosePar-1)-(posOpenPar+1) +1 )  ;
            endStr = &expr[posClosePar+1];
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
        )

        {
            expr = begStr+subExpr+endStr;
            repeat=true;
        }

        if(repeat)
        {
            counter++;

            if( config.doesPrintExprAtEveryStep()){
                cout << expr << endl;
            }
        }
    }
    while(repeat && counter < 100);

    if(counter > 0) goto restartArithmeticEval;


    #endif // DEV

    #ifdef DEBUG_LOG_STRINGEVAL
    std::cout << "final:" << expr+"+0" << endl;
    #endif

    double result;

    try
    {
        result = std::stod(expr);
    }
    catch(const std::exception& ex)
    {
        // Then it is not a numerical expression

        if(expr != "true" && expr != "false")
            status = CalculationStatus::EVAL_ERROR;

        return status;
    }




    // 5. If it is an integer, convert it to an integer.

    if(status != CalculationStatus::EVAL_ERROR)
    {
        if(result == static_cast<double>(static_cast<int>(result)) )
            expr = to_string(static_cast<int>(result));
    }

    return status;


    }
    catch(std::exception const& ex)
    {
        if(printWarnings)
            std::cout << "Eval error: " << ex.what() << endl;

        //else throw;

        return CalculationStatus::EVAL_ERROR;
    }
}
