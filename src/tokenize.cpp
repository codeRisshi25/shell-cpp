#include "tokenize.hpp"
#include <sstream>
#include <iostream>
#include <cctype>

// std::vector<std::string> tokenize(const std::string& input) {
//     std::istringstream iss(input);
//     std::vector<std::string> tokens;
//     std::string token;
//     while (iss >> token) tokens.push_back(token);
//     return tokens;
// }

// custom tokenizer for quoting
std::vector<std::string> tokenize(const std::string &input)
{
    bool in_quote_state = false;
    bool prev_white_space = false;  // check if previous char was white space to omit it if not in quotes
    std::string token;
    std::vector<std::string> tokens;
    for (int i  = 0 ; i < input.size() ; i++)
    {
        if (input[i] == '\'')
        {
            in_quote_state = !in_quote_state;
            if (!in_quote_state && input[i+1] != '\'' && input[i+1] != ' ') // if next char is a quote after quote end , count it as the same entitiy
            {
                tokens.push_back(token);
                token.clear();
            } else continue ;
        }
        else if (isspace(input[i]))
        {
            if (!in_quote_state && !prev_white_space)
            {   
                tokens.push_back(token);
                token.clear();
                prev_white_space = true;
            }else if (in_quote_state) token.push_back(' ');
            else continue;
        }
        else
        {
            token.push_back(input[i]);
            prev_white_space = false;
        }
    };
    if (!token.empty()) tokens.push_back(token);
    return tokens;
};