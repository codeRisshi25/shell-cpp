#include "tokenize.hpp"
#include <cctype>
#include <iostream>
#include <sstream>

// Finite State Machine
enum class ParseMode {
  NORMAL,
  IN_SINGLE_QUOTE,
  IN_DOUBLE_QUOTE,
  IN_ESCAPE,
};

// custom tokenizer for quoting
std::vector<std::string> tokenize(const std::string &input) {
  std::string token;
  std::vector<std::string> tokens;
  ParseMode mode = ParseMode::NORMAL;
  ParseMode prevMode = ParseMode::NORMAL;

  // looping through the arguments for tokens
  for (char i : input) {
    switch (mode) {
    case ParseMode::NORMAL:
      // case for normal
      if (i == ' ') {
        if (!token.empty())
          tokens.push_back(token);
        token.clear();
      } else if (i == '\'')
        mode = ParseMode::IN_SINGLE_QUOTE;
      else if (i == '\"')
        mode = ParseMode::IN_DOUBLE_QUOTE;
      else if (i == '\\') {
        mode = ParseMode::IN_ESCAPE;
        prevMode = ParseMode::NORMAL;
      } else {
        token.push_back(i);
      }
      break;

    case ParseMode::IN_SINGLE_QUOTE:
      // case for single quotes
      if (i == '\'')
        mode = ParseMode::NORMAL;
      else
        token.push_back(i);
      break;

    case ParseMode::IN_DOUBLE_QUOTE:
      // case for double quotes
      if (i == '\"')
        mode = ParseMode::NORMAL;
      else if (i == '\\') {
        mode = ParseMode::IN_ESCAPE;
        prevMode = ParseMode::IN_DOUBLE_QUOTE;
      } else
        token.push_back(i);
      break;

    case ParseMode::IN_ESCAPE:
      if (prevMode == ParseMode::IN_DOUBLE_QUOTE) {
        if (i == '\\' || i == '$' || i == '\"') {
          token.push_back(i);
        } else {
          token.push_back('\\');
          token.push_back(i);
        }
        mode = prevMode; // Reset to previous mode
      } else {
        if (i == '\\')
          token.push_back(' ');
        else {
          token.push_back(i);
        }
        mode = prevMode; // Reset to previous mode
      }
      break;
    }
  }

  // Add any remaining token
  if (!token.empty())
    tokens.push_back(token);

  // Check for unclosed quotes or incomplete escape sequences
  if (mode != ParseMode::NORMAL) {
    std::cout << "invalid command\n";
    return {};
  }

  return tokens;
}