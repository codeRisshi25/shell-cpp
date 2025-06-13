#include "builtins.hpp"
#include "tokenize.hpp"
#include "util.hpp"
#include <iostream>

enum out_type { STDOUT, STDERR };

int main() {
  init_builtins();
  // Flush after every std::cout / std:cerr
  std::cout << std::unitbuf;
  std::cerr << std::unitbuf;

  while (true) {
    std::cout << "$ ";
    std::string input;
    std::getline(std::cin, input);
    auto args = tokenize(input);

    if (args.empty())
      continue;

    // search for [">","1>","2>"] in the args
    auto redirect = std::find(args.begin(), args.end(), ">");
    out_type redirect_type = out_type::STDOUT; // Track what is being redirected

    //? Find 1> if > not found
    if (redirect == args.end()) {
      redirect = std::find(args.begin(), args.end(), "1>");
      redirect_type = out_type::STDOUT;
    }

    //? Find 2> if none found
    if (redirect == args.end()) {
      redirect = std::find(args.begin(), args.end(), "2>");
      redirect_type = out_type::STDERR;
    }

    //* IMP : MAIN LOGIC TO HANDLE EXECUTION !!!
    if (redirect != args.end()) {
      if (redirect + 1 == args.end()) {
        std::cout << "syntax error: filename after '>'\n";
        continue;
      }

      std::string filename = *(redirect + 1);
      args.erase(redirect, args.end());

      // Now execute with redirection
      auto it = builtins.find(args[0]);
      if (it != builtins.end()) {
        if (redirect_type == out_type::STDERR) {
          executeBuiltinWithStderrRedirect(args,filename);
        } else
        executeBuiltinWithRedirect(args, filename);
      } else {
        std::string execPath = execSearch(args[0]);
        if (!execPath.empty()) {
          if (redirect_type == out_type::STDERR) {
            execStderrCmdWithRedirect(args, filename);
          }
          else execCmdWithRedirect(args, filename);
        } else {
          std::cout << args[0] << ": command not found\n";
        }
      }
    } else {
      auto it = builtins.find(args[0]);
      if (it != builtins.end()) {
        it->second(args);
      } else {
        std::string execPath = execSearch(args[0]);
        if (execPath != "") {
          execCmd(args);
        } else
          std::cout << input << ": command not found\n";
      }
    }
  }
  return 0;
}
