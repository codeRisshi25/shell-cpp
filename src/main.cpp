#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <unistd.h>
#include <sys/wait.h>
#include <functional>
#include <unordered_map>
// for file searches
#include <cstdlib>
#include <filesystem>
namespace fs = std::filesystem;

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

// method to search for a executable file
std::string execSearch(std::string exec)
{
  const char *value = std::getenv("PATH");
  if (value == nullptr)
  {
    std::cout << "PATH env not found" << std::endl;
    return "";
  }
  std::istringstream iss(value);
  std::string path;
  char delimiter = ':';

  while (std::getline(iss, path, delimiter))
  {
    try {
    for(const auto& entry : fs::directory_iterator(path)){
      if (entry.path().filename() == exec) {
        return entry.path().generic_string();
        break;
      }
    }
  } catch (fs::filesystem_error){
    continue;
  }
  }
  return "";
}

// take the tokenized args and call the appropriate handler
using CmdHandler = std::function<void(const std::vector<std::string> &)>;

// global dispatch table
std::unordered_map<std::string, CmdHandler> builtins;

// register the built in methods
void init_builtins()
{

  // exit command to break the REPL
  builtins["exit"] = [&](const std::vector<std::string> &args)
  {
    int code = (args.size() > 1) ? std::stoi(args[1]) : 0;
    std::exit(code);
  };

  // echo command to give back whatever the user is typing
  builtins["echo"] = [&](const std::vector<std::string> &args)
  {
    for (size_t i = 1; i < args.size(); ++i)
    {
      std::cout << args[i] << (i + 1 < args.size() ? " " : "");
    };
    std::cout << "\n";
  };

  // type command to define command types
  builtins["type"] = [&](const std::vector<std::string> &args)
  {
    std::string cmd = (args.size() > 1) ? args[1] : "";
    bool cmdexists = builtins.count(cmd);
    if (cmdexists)
      std::cout << cmd << " is a shell builtin\n";
    else {
      std::string path = execSearch(cmd);
      if (path != "") std::cout << cmd << " is " << path << " \n";
      else std::cout << cmd << ": not found\n";
    }
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
    else
    {
      std::cout << input << ": command not found\n";
    }
  }
  return 0;
}
