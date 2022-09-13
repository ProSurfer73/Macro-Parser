#ifndef STRINGEVAL_HPP
#define STRINGEVAL_HPP


#include <vector>
#include <string>
#include <sstream>
#include <regex>
#include <cassert>

#include "config.hpp"
#include "hexa.hpp"
#include "command.hpp"
#include "options.hpp"

using namespace std;

struct MacroContainer;

bool isMacroCharacter(char c);

void clearSpaces(string& str);

bool doesExprLookOk(const string& expr);

double evaluateArithmeticExpr(const std::string& expr);

bool calculateExpression(string& expr, const MacroContainer& macroContainer, bool& shouldDisplayPbInfo, const Options& config);

enum CalculationStatus calculateExpressionWithPossibilities(string& expr, const MacroContainer& macroContainer, const Options& config, bool printWarnings, bool enableBoolean, const std::vector<std::pair<std::string,std::string> >& redefinitions);

#endif // STRINGEVAL_HPP
