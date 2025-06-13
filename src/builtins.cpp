#include "builtins.hpp"
#include "enum.hpp"
#include "search.hpp"
#include "util.hpp"
#include <cstdlib>
#include <fcntl.h>
#include <iostream>
#include <limits.h>
#include <unistd.h>

std::unordered_map<std::string, CmdHandler> builtins;
SearchNode *globalCommandTrie = nullptr;

void initializeBuiltInTrie() {
  if (globalCommandTrie != nullptr) {
    cleanupCommandTrie();
  }

  globalCommandTrie = new SearchNode();
  // Insert built-in commands
  insertSearchNode(globalCommandTrie, "echo");
  insertSearchNode(globalCommandTrie, "exit");
  insertSearchNode(globalCommandTrie, "pwd");
  insertSearchNode(globalCommandTrie, "cd");
  insertSearchNode(globalCommandTrie, "type");
  insertSearchNode(globalCommandTrie, "meow");
}

void executeBuiltinWithRedirect(const std::vector<std::string> &args,
                                const std::string &filename, write_mode mode) {
  int fd;
  if (mode == write_mode::TRUNCATE)
    fd = open(filename.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
  else
    fd = open(filename.c_str(), O_WRONLY | O_CREAT | O_APPEND, 0644);
  if (fd == -1) {
    perror("open");
    return;
  }

  int saved_stdout = dup(STDOUT_FILENO); // Save original stdout
  dup2(fd, STDOUT_FILENO);               // Redirect to file
  close(fd);

  // Execute builtin
  auto it = builtins.find(args[0]);
  if (it != builtins.end()) {
    it->second(args);
  }

  dup2(saved_stdout, STDOUT_FILENO); // Restore stdout
  close(saved_stdout);
}

void executeBuiltinWithStderrRedirect(const std::vector<std::string> &args,
                                      const std::string &filename,
                                      write_mode mode) {
  int fd;
  if (mode == write_mode::TRUNCATE)
    fd = open(filename.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
  else
    fd = open(filename.c_str(), O_WRONLY | O_CREAT | O_APPEND, 0644);
  if (fd == -1) {
    perror("open");
    return;
  }

  int saved_stderr = dup(STDERR_FILENO);
  dup2(fd, STDERR_FILENO);
  close(fd);

  auto it = builtins.find(args[0]);
  if (it != builtins.end()) {
    it->second(args);
  };

  dup2(saved_stderr, STDERR_FILENO);
  close(saved_stderr);
}

void init_builtins() {
  builtins["exit"] = [](const std::vector<std::string> &args) {
    int code = (args.size() > 1) ? std::stoi(args[1]) : 0;
    std::exit(code);
  };
  builtins["echo"] = [](const std::vector<std::string> &args) {
    for (size_t i = 1; i < args.size(); ++i)
      std::cout << args[i] << (i + 1 < args.size() ? " " : "");
    std::cout << "\n";
  };
  builtins["type"] = [](const std::vector<std::string> &args) {
    std::string cmd = (args.size() > 1) ? args[1] : "";
    if (builtins.count(cmd))
      std::cout << cmd << " is a shell builtin\n";
    else {
      std::string path = execSearch(cmd);
      if (!path.empty())
        std::cout << cmd << " is " << path << "\n";
      else
        std::cout << cmd << ": not found\n";
    }
  };
  builtins["pwd"] = [](const std::vector<std::string> &args) {
    char buffer[PATH_MAX];
    if (getcwd(buffer, PATH_MAX) != nullptr) {
      std::cout << buffer << "\n";
    } else {
      std::cerr << "pwd: error getting current directory\n";
    }
  };
  builtins["meow"] = [](const std::vector<std::string> &args) {
    std::cout << "🐱 Meow! Welcome to MewoShell! 🐾\n";
    std::cout << "   /\\_/\\  \n";
    std::cout << "  ( o.o ) \n";
    std::cout << "   > ^ <  \n";
  };
  builtins["cd"] = [](const std::vector<std::string> &args) {
    std::string dir = (args.size() > 1 ? args[1] : "~");
    char *point = const_cast<char *>(dir.c_str());
    if (dir[0] == '~') {
      const char *home = std::getenv("HOME");
      int rs = chdir(home);
      if (rs == -1)
        std::cout << "cd: " << dir << ": No such file or directory"
                  << std::endl;
    } else {
      int rs = chdir(point);
      if (rs == -1)
        std::cout << "cd: " << dir << ": No such file or directory"
                  << std::endl;
    }
  };
}

// cleanup code
void cleanupCommandTrie() {
  if (globalCommandTrie != nullptr) {
    delete globalCommandTrie;
    globalCommandTrie = nullptr;
  }
}