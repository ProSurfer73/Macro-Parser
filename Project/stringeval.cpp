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
            result /= num;
            break;
        case '%':
            if(num<=0)
            {
                std::cout << "error: %0" << endl;
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

        // It contains # => Incorrect
        // Our program doesn't deal with ## yet.
        if(expr[i] == '#')
            return false;
    }

    return true;
}

// The arithmetic expression must be correct and must NOT contain spaces.
double evaluateSimpleArithmeticExpr(const string& expr)
{
    istringstream mathStrm(expr);
    double result = (-3);
    mathStrm >> result;
    char op;
    double num;
    while (mathStrm >> op >> num)
    {
        func(result, op, num);

        #ifdef DEBUG_LOG_STRINGEVAL
            std::cout << "==op=" << op << endl;
            std::cout << "==num=" << num << endl;
        #endif
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
enum CalculationStatus calculateExpression(string& expr, const MacroContainer& macroContainer, const Options& config)
{
    CalculationStatus status = CalculationStatus::EVAL_OKAY;

    const auto& dictionary = macroContainer.getDefines();
    const auto& redefinedMacros = macroContainer.getRedefinedMacros();
    const auto& incorrectMacros = macroContainer.getIncorrectMacros();

    unsigned counter = 0;


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
        size_t maxSizeReplace = 0;
        size_t maxSizeReplaceSig = 0;

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

                if(p.first.size() >= maxSizeReplace){
                    maxSizeReplace = p.first.size();
                }
            }
            else if(mac[mac.size()-1] == ')' && mac[mac.size()-2] == 'x' && mac[mac.size()-3] == '('
                && expr.find(mac.substr(0,mac.size()-3)) != string::npos)
            {
                maxSizeReplaceSig = mac.size();
            }
        }

        #ifdef DEBUG_LOG_STRINGEVAL
        cout << "g:" << maxSizeReplace << endl;
        #endif

        if(maxSizeReplace != 0)
        {

        // Replace it
        for(pair<string,string> p: dictionary)
        {
            if(p.first.size() == maxSizeReplace && expr.find(p.first) != string::npos && doesExprLookOk(p.second))
            {
                if(config.doesPrintReplacements()){
                    std::cout << "replaced '" << p.first << "' by '" << p.second << "'" << endl;
                }


                // If replacement strings aren't correct
                if((p.first.find('(') != string::npos && p.first.find(')') == string::npos)
                || (p.first.find('(') == string::npos && p.first.find(')') != string::npos)){
                    // Then there is a problem, we skip.

                    //return CalculationStatus::EVAL_ERROR;
                    continue;
                }


                simpleReplace(expr, p.first, p.second);

                if(std::find(redefinedMacros.begin(), redefinedMacros.end(), p.first) != redefinedMacros.end()){
                    cout << "/!\\ Warning: the macro " << p.first << " you are using have been redefined /!\\" << endl;
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
        else if(maxSizeReplaceSig != 0)
        {
            // Look for single parameter macro
            for(pair<string,string> p: dictionary)
            {
                string& mac = p.first;

                // if the string has at the end "(x)", then it's a single param macro
                if(maxSizeReplaceSig == mac.size()
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
                            cout << "replaced '" << p.first << "' by '" << p.second << '\'' << endl;
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


    unsigned searchedCharacter;

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

        // Treat AND boolean operations
        || simpleReplace(subExpr, "false&&false", "false")
        || simpleReplace(subExpr, "true&&true", "true")
        || simpleReplace(subExpr, "true&&false", "false")
        || simpleReplace(subExpr, "false&&true", "true")

        // Treat OR boolean operations
        || simpleReplace(subExpr, "false||false", "false")
        || simpleReplace(subExpr, "||true", "")
        || simpleReplace(subExpr, "true||", ""))
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
