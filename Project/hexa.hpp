#ifndef HEXA_HPP
#define HEXA_HPP

#include <math.h>
#include <string.h>
#include <iostream>
#include <string>

#include "options.hpp"

void locateAndReplaceHexa(std::string& str, const class Options& options);

long long convertHexaToDeci(const std::string& hex);

std::string convertDeciToHexa(long int num_decimal);

bool isHexaLetter(char c);


#endif // HEXA_HPP
