#include "stringeval.hpp"

#include <iostream>

void clearSpaces(string& str)
{
    str.erase(std::remove(str.begin(), str.end(), ' '), str.end());
}

static bool isOperationCharacter(char c)
{
    return (c=='+'||c=='-'||c=='*'||c=='/');
}

static bool isMacroCharacter(char c)
{
    return (isalpha(c)||(c>='0' && c<='9')||(c=='_'));
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
        default:
            std::cout << "Unrecognized character: " << op << std::endl;
            break;
    }
}

bool doesExprLookOk(const string& expr)
{
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
double evaluateArithmeticExpr(const string& expr)
{
    stringstream mathStrm(expr);
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



/** \brief
 *
 * \param expr: macro given in input
 * \param macroContainer
 * \return true is the expression has been calculated, or false
 *
 */

bool calculateExpression(string& expr, const MacroContainer& macroContainer, bool& shouldDisplayPbInfo, const Options& config)
{
    const auto& dictionary = macroContainer.getDefines();
    const auto& redefinedMacros = macroContainer.getRedefinedMacros();
    const auto& incorrectMacros = macroContainer.getIncorrectMacros();


    /// 0. Is the expression okay ?

    if(expr[0] == '?')
        return false;

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
                    // Then there is a problem, we stop the evaluation here.

                    return false;
                }

                expr = std::regex_replace(expr, std::regex(p.first), p.second); // replace 'def' -> 'klm'

                if(std::find(redefinedMacros.begin(), redefinedMacros.end(), p.first) != redefinedMacros.end()){
                    cout << "/!\\ Warning: the macro " << p.first << " you are using have been redefined /!\\" << endl;
                    shouldDisplayPbInfo = true;

                    //
                }

                if(std::find(incorrectMacros.begin(), incorrectMacros.end(), p.first) != incorrectMacros.end()){
                    cout << "/!\\ Warning: the macro " << p.first << "you are using seem incorrect. /!\\" << endl;
                    shouldDisplayPbInfo = true;
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
                        cop1 = std::regex_replace(cop1, std::regex(mac.substr(0,mac.size()-3)), ""); // replace 'def' -> 'klm'

                        // In string p.second ; replace '(x)' <= expr
                        string cop2 = p.second;
                        //cop2 = std::regex_replace(cop2, std::regex("(x)"), cop1); // replace 'def' -> 'klm'
                        cop1 = std::regex_replace(cop2, std::regex("(x)"), cop1); // replace 'def' -> 'klm'

                        if(config.doesPrintReplacements()){
                            cout << "replaced '" << p.first << "' by '" << p.second << '\'' << endl;
                        }

                        #ifdef DEBUG_LOG_STRINGEVAL
                        cout << "before: " << expr << "==" << mac << endl;
                        #endif
                        expr = cop1;

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




    // 2. Can the expression be evaluated ?

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
        return false;
    }



    // 3. Arithmetic operation

    #ifdef PARENTHESIS_EVALUATION

    #ifdef DEBUG_LOG_STRINGEVAL
    cout << "c:" << expr << endl;
    #endif

    #ifdef DEBUG_ENABLE_ASSERTIONS
    unsigned counter = 0;
    #endif

        while(expr.find('(') != string::npos && expr.find(')') != string::npos)
        {
            // n'importe quoi formule a revoir
            //string toBeCalculated = expr.substr(expr.find_last_of('(')+1, expr.find_first_of(')')-(expr.find_last_of('(')) );



            size_t posClosePar = expr.find_first_of(')');
            size_t posOpenPar = posClosePar;

            for(;expr[posOpenPar]!='('; posOpenPar--);

            string toBeCalculated = expr.substr(posOpenPar+1, (posClosePar-1)-(posOpenPar+1) +1 )  ;


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
                double value = evaluateArithmeticExpr(toBeCalculated);

                string afterDeletion = (expr.substr(0,posOpenPar) + to_string(value)) + expr.substr(posClosePar+1);

                expr = afterDeletion;



            }
            else
            {
                // We've got to remove the parenthesis around the number, for instance : (14.2) => 14.2
                //toBeCalculated = expr.substr(expr.find_last_of('(')+1, expr.find_last_of(')')-(expr.find_last_of('(')+1) );

                string begStr = expr.substr(0,posOpenPar);
                string midStr = toBeCalculated;
                string endStr = &expr[posClosePar+1];

                string afterDeletion = begStr + midStr + endStr;

                #ifdef DEBUG_LOG_STRINGEVAL
                cout << "after:" << afterDeletion << endl;
                cout << "b:" << begStr << ":" << midStr << ":" << endStr << endl;
                #endif

                expr = afterDeletion;
            }

            #ifdef DEBUG_ENABLE_ASSERTIONS
                ++counter;
                assert(counter < 100);
            #endif // DEBUG_ENABLE_ASSERTIONS


            clearSpaces(expr);

            if(config.doesPrintExprAtEveryStep())
                cout << expr << endl;
        }

        if(!doesExprLookOk(expr))
            return false;


    #endif

    #ifdef DEBUG_LOG_STRINGEVAL
    std::cout << "final:" << expr+"+0" << endl;
    #endif

    double result=evaluateArithmeticExpr(expr+"+0");

    if(result != -3)
    {
        if(result == static_cast<double>(static_cast<int>(result)) )
            expr = to_string(static_cast<int>(result));
        else
            expr = to_string(result);
    }

    return true;
}
