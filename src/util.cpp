#include "util.hpp"
#include "enum.hpp"
#include "search.hpp"
#include <cstdlib>
#include <fcntl.h>
#include <filesystem>
#include <iostream>
#include <sstream>
#include <sys/wait.h>
#include <unistd.h>

namespace fs = std::filesystem;
void initWelcome() {
  std::cout << R"(
┌─────────────────────────────────────────────────────────────────┐
│                                                                 │
│    ███╗   ███╗      ███████╗██╗  ██╗███████╗██╗     ██╗         │
│    ████╗ ████║      ██╔════╝██║  ██║██╔════╝██║     ██║         │
│    ██╔████╔██║█████╗███████╗███████║█████╗  ██║     ██║         │
│    ██║╚██╔╝██║╚════╝╚════██║██╔══██║██╔══╝  ██║     ██║         │
│    ██║ ╚═╝ ██║      ███████║██║  ██║███████╗███████╗███████╗    │
│    ╚═╝     ╚═╝      ╚══════╝╚═╝  ╚═╝╚══════╝╚══════╝╚══════╝    │
│                                                                 │
│                        A POSIX Shell                            │
│                        Made by Risshi                           │
└─────────────────────────────────────────────────────────────────┘

)" << std::endl;
}

void initializeCmdTrie() {
  if (globalCommandTrie == nullptr) {
    return;
  }

  const char *value = std::getenv("PATH");
  if (value == nullptr) {
    std::cout << "PATH env not found" << std::endl;
    throw std::runtime_error("PATH environment variable not found");
  }
  std::istringstream iss(value);
  std::string path;
  char delimiter = ':';

  while (std::getline(iss, path, delimiter)) {
    try {
      for (const auto &entry : fs::directory_iterator(path)) {
        // if (entry.path().filename() == exec) { //! Add the commands the the
        // insertSearchNode one by one
        //   return entry.path().generic_string();
        //   break;
        // }
        insertSearchNode(globalCommandTrie, entry.path().filename().string());
      }
    } catch (fs::filesystem_error) {
      continue;
    }
  }
}

std::string execSearch(std::string exec) {
  const char *value = std::getenv("PATH");
  if (value == nullptr) {
    std::cout << "PATH env not found" << std::endl;
    throw std::runtime_error("PATH environment variable not found");
  }
  std::istringstream iss(value);
  std::string path;
  char delimiter = ':';

  while (std::getline(iss, path, delimiter)) {
    try {
      for (const auto &entry : fs::directory_iterator(path)) {
        if (entry.path().filename() == exec) {
          return entry.path().generic_string();
          break;
        }
      }
    } catch (fs::filesystem_error) {
      continue;
    }
  }
  return "";
};

void execCmd(std::vector<std::string> cmd) {
  // execv require char pointers to C string style array null terminated
  std::vector<char *> argv;
  argv.reserve(cmd.size() + 1);
  for (const auto &s : cmd) {
    argv.push_back(const_cast<char *>(s.c_str()));
  }
  argv.push_back(nullptr);

  // fork a new process and execv
  pid_t pid = fork();
  if (pid == -1) {
    perror("fork");
    return;
  }
  if (pid == 0) {
    execvp(argv[0], argv.data());
    perror("execv");
    _exit(1);
  } else {
    int status;
    waitpid(pid, &status, 0);
  }
};

void execCmdWithRedirect(const std::vector<std::string> &cmd,
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

  pid_t pid = fork();
  if (pid == 0) {
    // Child: redirect stdout to file
    dup2(fd, STDOUT_FILENO);
    close(fd);

    std::vector<char *> argv;
    argv.reserve(cmd.size() + 1);
    for (const auto &s : cmd) {
      argv.push_back(const_cast<char *>(s.c_str()));
    }
    argv.push_back(nullptr);

    execvp(argv[0], argv.data());
    perror("execvp");
    _exit(1);
  } else {
    close(fd);
    int status;
    waitpid(pid, &status, 0);
  }
}

void execStderrCmdWithRedirect(const std::vector<std::string> &cmd,
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
  pid_t pid = fork();
  if (pid == 0) {
    // Child: redirect stdout to file
    dup2(fd, STDERR_FILENO);
    close(fd);

    std::vector<char *> argv;
    argv.reserve(cmd.size() + 1);
    for (const auto &s : cmd) {
      argv.push_back(const_cast<char *>(s.c_str()));
    }
    argv.push_back(nullptr);

    execvp(argv[0], argv.data());
    perror("execvp");
    _exit(1);
  } else {
    close(fd);
    int status;
    waitpid(pid, &status, 0);
  }
}