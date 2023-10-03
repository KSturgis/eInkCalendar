#ifndef PARSER_H
#define PARSER_H

#include <Arduino.h>
#include <vector>

std::vector<String> splitString(const String str, const char* delimeter);
std::vector<int> parseInts(const String str, const char* delimeter);

#endif //PARSER_H