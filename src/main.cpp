#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <unistd.h>
#include <sys/wait.h>
#include <functional>
#include <unordered_map>

// method to tokenize user inputs into words
std::vector<std::string> tokenize(const std::string &input)
{
  std::istringstream iss(input);
  std::vector<std::string> tokens;
  std::string token;
  while (iss >> token)
  {
    tokens.push_back(token);
  }
  return tokens;
}

// take the tokenized args and call the appropriate handler
using CmdHandler = std::function<void(const std::vector<std::string> &)>;

// global dispatch table
std::unordered_map<std::string, CmdHandler> builtins;

// register the built in methods
void init_builtins()
{
  builtins["exit"] = [&](const std::vector<std::string> &args)
  {
    int code = (args.size() > 1) ? std::stoi(args[1]) : 0;
    std::exit(code);
  };
  builtins["echo"] = [&](const std::vector<std::string> &args)
  {
    for (size_t i = 1; i < args.size(); ++i)
    {
      std::cout << args[i] << (i + 1 < args.size() ? " " : "");
    };
    std::cout << "\n";
  };
};

int main()
{
  init_builtins();
  // Flush after every std::cout / std:cerr
  std::cout << std::unitbuf;
  std::cerr << std::unitbuf;

  while (true)
  {
    std::cout << "$ ";
    std::string input;
    std::getline(std::cin, input);
    auto args = tokenize(input);
    if (args.empty())
      continue;
    auto it = builtins.find(args[0]);
    if (it != builtins.end())
    {
      it->second(args);
    }
    else {
      std::cout << input << ": command not found\n";
    }
  }
  return 0;
}
