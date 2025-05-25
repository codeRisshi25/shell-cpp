#include <iostream>
#include "builtins.hpp"
#include "util.hpp"
#include "tokenize.hpp"

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
      std::string execPath = execSearch(args[0]);
      if (execPath != ""){
        execCmd(args);
      }else std::cout << input << ": command not found\n";
    }
  }
  return 0;
}
