#include "builtins.hpp"
#include "enum.hpp"
#include "search.hpp"
#include "tokenize.hpp"
#include "util.hpp"
#include "history.hpp"
#include "sighandler.hpp"
#include <algorithm>
#include <cstdio>
#include <iostream>
#include <termios.h>
#include <unistd.h>

class TerminalManager {
private:
  struct termios orig_termios;
  bool raw_mode_enabled = false;

public:
  void enableRawMode() {
    if (tcgetattr(STDIN_FILENO, &orig_termios) == -1)
      return;

    struct termios raw = orig_termios;
    raw.c_lflag &= ~(ECHO | ICANON); // Disable echo and canonical mode
    raw.c_cc[VMIN] = 1;              // Minimum chars to read
    raw.c_cc[VTIME] = 0;             // Timeout in deciseconds

    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1)
      return;
    raw_mode_enabled = true;
  }

  void disableRawMode() {
    if (raw_mode_enabled) {
      tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
      raw_mode_enabled = false;
    }
  }

  ~TerminalManager() { disableRawMode(); }
};

std::string readLineWithCompletion() {
  std::string line;
  int c;
  int tabCnt = 0;

  while ((c = getchar()) != EOF) {
    if (c == '\r' || c == '\n') {
      std::cout << "\n";
      break;
    } else if (c == '\t') {
      // Handle tab completion
      if (line.empty()) {
        std::cout << "\a" << std::flush; // dont suggest on empty line
        continue;
      }
      auto suggestions = getAuthCompleteSuggestions(globalCommandTrie, line);
      if (suggestions.empty()) {
        // No suggestions, just beep or do nothing
        std::cout << "\a" << std::flush; // Bell character
      } else if (suggestions.size() == 1) {
        std::cout << "\r$ ";
        for (size_t i = 0; i < line.length(); ++i) {
          std::cout << " ";
        }
        std::cout << "\r$ ";

        line = suggestions[0] + " ";
        std::cout << line;
      } else {
        std::string commonPrefix = searchLongestPrefix(suggestions);
        // Multiple completions - show them
        if (commonPrefix.length() > line.length()) {
          std::cout << "\r$ ";
          for (size_t i = 0; i < line.length(); ++i) {
            std::cout << " ";
          }
          std::cout << "\r$ ";

          line = commonPrefix;
          std::cout << line;
          tabCnt = 0; // Reset tab counter
        }
        tabCnt++;
        if (tabCnt < 2) {
          std::cout << "\a" << std::flush;
          continue;
        }
        std::cout << "\n";
        for (const auto &suggestion : suggestions) {
          std::cout << suggestion << "  ";
        }
        std::cout << "\n$ " << line;
        tabCnt = 0;
      }
    } else if (c == 127 || c == '\b') {
      // Backspace
      if (!line.empty()) {
        line.pop_back();
        std::cout << "\b \b";
      }
    } else if (c >= 32 && c <= 126) {
      // Printable character
      line += c;
      std::cout << (char)c;
    } else if (c == 27) {
      // Handle escape sequences (arrow keys)
      int next1 = getchar();
      if (next1 == '[') {
        int next2 = getchar();
        if (next2 == 'A') {
          // Up arrow - replace current line
          std::string historyLine = handleUpArrow();
          if (!historyLine.empty()) {
            // Clear current line
            std::cout << "\r$ ";
            for (size_t i = 0; i < line.length(); ++i) {
              std::cout << " ";
            }
            std::cout << "\r$ ";

            // Set new line and display it
            line = historyLine;
            std::cout << line;
          }
        } else if (next2 == 'B') {
          // Down arrow - replace current line
          std::string historyLine = handleDownArrow();
          // Clear current line
          std::cout << "\r$ ";
          for (size_t i = 0; i < line.length(); ++i) {
            std::cout << " ";
          }
          std::cout << "\r$ ";

          // Set new line and display it
          line = historyLine;  // This might be empty for "current" line
          std::cout << line;
        }
      }
    }
  }

  return line;
}

int main() {
  initWelcome();
  init_builtins();
  initializeBuiltInTrie();
  initializeCmdTrie();
  std::cout << std::unitbuf;
  std::cerr << std::unitbuf;

  TerminalManager terminal;
  terminal.enableRawMode();

  signal(SIGINT, handle_sigint);
  signal(SIGTSTP, handle_sigtstp);
  signal(SIGTERM, handle_sigterm);

  out_type redirect_type = out_type::STDOUT;
  write_mode mode = write_mode::TRUNCATE;

  while (true) {
    std::cout << "$ ";
    std::string input = readLineWithCompletion();
    addToHistory(input);

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
          executeBuiltinWithStderrRedirect(args, filename, mode);
        } else {
          executeBuiltinWithRedirect(args, filename, mode);
        }
      } else {
        std::string execPath = execSearch(args[0]);
        if (!execPath.empty()) {
          if (redirect_type == out_type::STDERR) {
            execStderrCmdWithRedirect(args, filename, mode);
          } else {
            execCmdWithRedirect(args, filename, mode);
          }
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
        } else {
          std::cout << input << ": command not found\n";
        }
      }
    }
  }

  cleanupCommandTrie();
  return 0;
}
