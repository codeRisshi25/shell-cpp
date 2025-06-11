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
    auto redirect = std::find(args.begin(), args.end(), ">");
    if (redirect == args.end())
    {
      redirect = std::find(args.begin(), args.end(), "1>");
    }
    if (redirect != args.end())
    {
      if (redirect + 1 == args.end())
      {
        std::cout << "syntax error: filename after '>'\n";
        continue;
      }

      std::string filename = *(redirect + 1);
      args.erase(redirect, args.end());

      // Now execute with redirection
      auto it = builtins.find(args[0]);
      if (it != builtins.end())
      {
        executeBuiltinWithRedirect(args, filename);
      }
      else
      {
        std::string execPath = execSearch(args[0]);
        if (!execPath.empty())
        {
          execCmdWithRedirect(args, filename);
        }
        else
        {
          std::cout << args[0] << ": command not found\n";
        }
      }
    }
    else
    {
      auto it = builtins.find(args[0]);
      if (it != builtins.end())
      {
        it->second(args);
      }
      else
      {
        std::string execPath = execSearch(args[0]);
        if (execPath != "")
        {
          execCmd(args);
        }
        else
          std::cout << input << ": command not found\n";
      }
    }
  }
  return 0;
}
