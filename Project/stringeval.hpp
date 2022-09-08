#ifndef STRINGEVAL_HPP
#define STRINGEVAL_HPP


#include <vector>
#include <string>
#include <sstream>
#include <cassert>

#include "container.hpp"
#include "stringeval.hpp"
#include "hexa.hpp"

using namespace std;


enum class CalculationStatus { EVAL_ERROR, EVAL_WARNING, EVAL_OKAY };

bool isMacroCharacter(char c);

void lowerString(std::string& str);

void searchAndReplaceMacros(std::string& expr);

void clearSpaces(string& str);

void splitString(std::vector<std::string>& outputVector, std::string initialStr);

bool doesExprLookOk(const string& expr);

double evaluateSimpleArithmeticExpr(const std::string& expr);

bool simpleReplace(std::string& str, const std::string& from, const std::string& to);



enum CalculationStatus calculateExpression(string& expr, const MacroContainer& macroContainer, const Options& config, bool printWarnings=true);


#endif // STRINGEVAL_HPP
