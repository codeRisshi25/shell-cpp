#include "util.hpp"
#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <sstream>
#include <unistd.h>
#include <sys/wait.h> 

namespace fs = std::filesystem;

std::string execSearch(std::string exec)
{
  const char *value = std::getenv("PATH");
  if (value == nullptr)
  {
    std::cout << "PATH env not found" << std::endl;
    throw std::runtime_error("PATH environment variable not found");
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
};

void execCmd(std::vector<std::string> cmd){
  // execv require char pointers to C string style array null terminated
  std::vector<char*> argv;
  argv.reserve(cmd.size() +1);
  for (const auto &s : cmd) {
    argv.push_back(const_cast<char*>(s.c_str()));
  }
  argv.push_back(nullptr);

  // fork a new process and execv
  pid_t pid = fork();
  if (pid == -1){
    perror("fork");
    return;
  }
  if (pid == 0) {
    execvp(argv[0],argv.data());
    perror("execv");
    _exit(1);
  }else {
    int status;
    waitpid(pid, &status, 0);
  }
};