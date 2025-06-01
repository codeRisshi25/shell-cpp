#include "tokenize.hpp"
#include <sstream>
#include <iostream>
#include <cctype>

// Finite State Machine 
enum class ParseMode {
    NORMAL,
    IN_SINGLE_QUOTE,
    IN_DOUBLE_QUOTE,
    IN_ESCAPE
};

// custom tokenizer for quoting
std::vector<std::string> tokenize(const std::string &input)
{
    std::string token;
    std::vector<std::string> tokens;
    ParseMode mode = ParseMode::NORMAL;

    // looping through the arguments for tokens
    for (char i : input ){
        switch (mode) {
            case ParseMode:: NORMAL :
            // case for normal 
            if (i==' '){if (!token.empty()) tokens.push_back(token); token.clear(); }
            else if (i == '\'') mode = ParseMode::IN_SINGLE_QUOTE;
            else if (i == '\"') mode = ParseMode::IN_DOUBLE_QUOTE;
            else if (i == '\\') mode = ParseMode::IN_ESCAPE;
            else {token.push_back(i);};

            break;


            case  ParseMode:: IN_SINGLE_QUOTE:
            // case for single quotes
            if (i=='\'') mode = ParseMode::NORMAL;
            else if (i == '\\') mode = ParseMode::IN_ESCAPE;
            else token.push_back(i);
            break;

            case ParseMode:: IN_DOUBLE_QUOTE :
            // case for double quotes 
            if (i=='\"') mode = ParseMode::NORMAL;
            else token.push_back(i);
            break;

            case ParseMode:: IN_ESCAPE :
            if (i=='\\') token.push_back(' ');
            else {
             mode = ParseMode::NORMAL;
             token.push_back(i);
            }    
        }
    }
    if (!token.empty()) tokens.push_back(token);
    if (mode != ParseMode::NORMAL) { std::cout << "invalid command\n" ; tokens = {}; };
    return tokens;
};