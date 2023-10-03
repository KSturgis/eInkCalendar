#include "parser.h"

std::vector<String> splitString(
    const String str,
    const char* delimeter)
{
    std::vector<String> splittedStrings = {};
    int pos = 0;
    int last_pos = 0;


    while ((pos = str.indexOf(delimeter, last_pos)) != -1)
    {
        String token = str.substring(last_pos, pos);
        splittedStrings.push_back(token);
        last_pos = pos + 1;
    }

    String lastToken = str.substring(last_pos);
    splittedStrings.push_back(lastToken);
    return splittedStrings;
}

std::vector<int> parseInts(
    const String str,
    const char* delimeter)
{
    std::vector<int> splittedStrings = {};
    int pos = 0;
    int last_pos = 0;

    while ((pos = str.indexOf(delimeter, last_pos)) != -1)
    {
        String token = str.substring(last_pos, pos);
        splittedStrings.push_back(token.toInt());
        last_pos = pos + 1;
    }

    String lastToken = str.substring(last_pos);
    splittedStrings.push_back(lastToken.toInt());
    return splittedStrings;
}
