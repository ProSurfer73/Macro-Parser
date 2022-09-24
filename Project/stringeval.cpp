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
#include "container.hpp"
#include "options.hpp"


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
    for(unsigned m=0;m<str.size();++m){
        if(isOperationCharacter(str[m]))
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
            throw std::runtime_error( std::string("Unrecognized character: ")+op+'\n' );
            break;
    }
}

static string extractVeryRightPart(string expr, size_t pos)
{
    std::size_t i=pos;
    for(; i < expr.size() && expr[i]!='&' && expr[i]!='|'; ++i);
    return expr.substr(i);
}

static string extractVeryLeftPart(string expr, size_t pos)
{
    std::size_t i=0;
    for(; pos-i > 0 && (i==0 || (expr[pos-i-1]!='&' && expr[pos-i-1]!='|')); ++i);
    return expr.substr(0, pos-i);
}

static string extractRightPart(string expr, size_t pos)
{
    std::size_t i=pos;
    for(; i < expr.size() && expr[i]!='&' && expr[i]!='|'; ++i);
    return expr.substr(pos, i-pos);
}

static string extractLeftPart(string expr, size_t pos)
{
    std::size_t i=0;
    for(; pos-i > 0 && (i==0 || (expr[pos-i-1]!='&' && expr[pos-i-1]!='|')); ++i);
    return expr.substr(pos-i, i);
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

        string veryLeftPart = extractVeryLeftPart(expr, searchedOperator);
        string veryRightPart = extractVeryRightPart(expr, searchedOperator+2);

        /*std::cout << "&&veryLeft: " << veryLeftPart << "'" << std::endl;
        std::cout << "&&veryRight: " << veryRightPart << "'" << std::endl;
        std::cout << "&&leftPart: " << leftPart << "'" << endl;
        std::cout << "&&rightPart: " << rightPart << "'" << endl;*/

        if(leftPart=="false" || rightPart=="false")
            expr = (veryLeftPart+"false")+veryRightPart;
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

static bool emplaceOnce(std::vector< std::string >& v, const std::string& macroName)
{
    if(v.empty()){
        v.emplace_back(macroName);
        return true;
    }

    if(std::find(v.begin(), v.end(), macroName)==v.end())
    {
        v.emplace_back(macroName);
        return true;
    }

    return false;
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
    const auto& redefinedMacros = macroContainer.getRedefinedMacros();
    //const auto& incorrectMacros = macroContainer.getIncorrectMacros();

    unsigned replaceCounter = 0;
    unsigned arithmeticCounter = 0;
    unsigned booleanCounter = 0;

    bool thereIsOperation = false;
    bool deleteRedef = false;

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
        for(const pair<string,string>& p: dictionary)
        {
            const string& mac = p.first;

            if(expr.find(p.first) != string::npos)
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
        for(const pair<string,string>& p: dictionary)
        {
            if(p.first == maxSizeReplace /*&& expr.find(p.first) != string::npos*/)
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
                    string replacedBy = p.second;

                    if(std::find(redefinedMacros.begin(), redefinedMacros.end(), p.first) != redefinedMacros.end())
                    {
                        bool foundRedef=false;

                        for(const auto& pp: *redef)
                        {
                            if(pp.first == p.first)
                            {
                                replacedBy = pp.second;
                                foundRedef = true;
                            }
                            //std::cout << "'" << pp.first << "'" << std::endl;
                        }

                        if(printWarnings)
                        {
                            if(std::find(printWarnings->begin(), printWarnings->end(), p.first) == printWarnings->end()){
                                cout << "/!\\ Warning: the macro " << p.first << " you are using has been redefined /!\\" << endl;
                                printWarnings->emplace_back(p.first);
                            }
                        }

                        if(outputs && !foundRedef)
                        {
                            bool oneThing=false;

                            for(const auto& pp: dictionary)
                            {
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
                                        if(status == CalculationStatus::EVAL_OKAY)
                                            emplaceOnce(*outputs, anotherExpr);
                                        else if(status == CalculationStatus::EVAL_ERROR){
                                            std::string sss="undefined:";
                                            sss += anotherExpr;
                                            emplaceOnce(*outputs, sss);
                                            status = CalculationStatus::EVAL_WARNING;
                                        }
                                        else if(status == CalculationStatus::EVAL_WARNING){
                                            emplaceOnce(*outputs, anotherExpr+'?');
                                            status = CalculationStatus::EVAL_WARNING;
                                        }
                                    }


                                }
                            }

                            if(oneThing)
                            {
                                expr = "multiple";

                                if(deleteRedef) delete redef;
                                return status;
                            }


                        }

                    }

                    if(config.doesPrintReplacements()){
                        std::cout << "replaced '" << p.first << "' by '" << p.second << "'" << endl;
                    }

                    // finally, let's replace-it in the expression
                    simpleReplace(expr, p.first, replacedBy);
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
            for(const pair<string,string>& p: dictionary)
            {
                const string& mac = p.first;

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

        ++replaceCounter;
        if(replaceCounter > 500){
            throw std::runtime_error("Counter pb: please post an issue on Github");
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

                /*string begStr = expr.substr(0,posOpenPar);
                string midStr = expr.substr(posOpenPar+1, (posClosePar-1)-(posOpenPar+1) +1 ) ;
                string endStr = &expr[posClosePar+1];*/

                string begStr = expr.substr(0,posOpenPar);
                string midStr = expr.substr(posOpenPar+1, (posClosePar-1)-(posOpenPar+1) +1 ) ;
                string endStr = &expr[posClosePar+1];

                expr = begStr + midStr + endStr;
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
            cout << expr << endl;
    }

    // If there is an operation character, the expr look good, and no parentheses
    if(containsOperation(expr) && doesExprLookOk(expr) && !containsAlpha(expr) && expr.find('(')==std::string::npos && expr.find(')')==std::string::npos)
    {
        expr = std::to_string(evaluateSimpleArithmeticExpr(expr));
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

            begStr = expr.substr(0,posOpenPar);
            subExpr = expr.substr(posOpenPar+1, (posClosePar-1)-(posOpenPar+1) +1 )  ;
            endStr = &expr[posClosePar+1];

            // If it is a number, let's just remove the parentheses for now
            bool isOnlyNumbers=true;
            for(char c: subExpr){
                if(!(c == '.' || isdigit(c)))
                    isOnlyNumbers=false;
            }
            if(isOnlyNumbers)
            {
                expr = begStr+subExpr+endStr;
                repeat=true;
                goto restartArithmeticEval;
            }

            // Let's to reevaluate what is in the parenthesis
            std::string subExpr2 = subExpr;
            auto status2 = calculateExpression(subExpr2, macroContainer, config, nullptr, enableBoolean, nullptr, redef);
            if(status2 == CalculationStatus::EVAL_OKAY)
            {
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
                cout << expr << endl;
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
                expr = to_string(static_cast<int>(result));
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
            std::cout << "Eval error: " << ex.what() << endl;

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

    if(!results.empty() || results.size()>=1)
    {
        expr.clear();

        // Let's remove multiple from the list of possible outputs
        for(auto it=results.begin(); it!=results.end();){
            if(*it == "multiple")
                it = results.erase(it);
            else
                ++it;
        }

        // Sort and remove duplicates
        auto& v = results;
        std::sort(v.begin(), v.end());
        v.erase(std::unique(v.begin(), v.end()), v.end());

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
