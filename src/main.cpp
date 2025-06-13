#include "builtins.hpp"
#include "tokenize.hpp"
#include "util.hpp"
#include <iostream>
#include "enum.hpp"

int main() {
  init_builtins();
  // Flush after every std::cout / std:cerr
  std::cout << std::unitbuf;
  std::cerr << std::unitbuf;

  out_type redirect_type = out_type::STDOUT; // Track what is being redirected
  write_mode mode = write_mode::TRUNCATE;    // Track the write mode

  while (true) {
    std::cout << "$ ";
    std::string input;
    std::getline(std::cin, input);
    auto args = tokenize(input);

    if (args.empty())
      continue;

    // search for [">","1>","2>",">>","1>>","2>>"] in the args
    auto redirect = std::find(args.begin(), args.end(), ">>");
    redirect_type = out_type::STDOUT;
    mode = write_mode::APPEND;

    //? Find 1>> if >> not found
    if (redirect == args.end()) {
      redirect = std::find(args.begin(), args.end(), "1>>");
      redirect_type = out_type::STDOUT;
      mode = write_mode::APPEND;
    }

    //? Find 2>> if none found
    if (redirect == args.end()) {
      redirect = std::find(args.begin(), args.end(), "2>>");
      redirect_type = out_type::STDERR;
      mode = write_mode::APPEND;
    }

    //? Search for truncating redirects
    if (redirect == args.end()) {
      redirect = std::find(args.begin(), args.end(), ">");
      redirect_type = out_type::STDOUT;
      mode = write_mode::TRUNCATE;
    }
    if (redirect == args.end()) {
      redirect = std::find(args.begin(), args.end(), "1>");
      redirect_type = out_type::STDOUT;
      mode = write_mode::TRUNCATE;
    }
    if (redirect == args.end()) {
      redirect = std::find(args.begin(), args.end(), "2>");
      redirect_type = out_type::STDERR;
      mode = write_mode::TRUNCATE;
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
          executeBuiltinWithStderrRedirect(args, filename , mode);
        } else
          executeBuiltinWithRedirect(args, filename , mode);
      } else {
        std::string execPath = execSearch(args[0]);
        if (!execPath.empty()) {
          if (redirect_type == out_type::STDERR) {
            execStderrCmdWithRedirect(args, filename , mode);
          } else
            execCmdWithRedirect(args, filename , mode);
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
