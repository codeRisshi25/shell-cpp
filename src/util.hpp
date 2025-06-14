#pragma once
#include "enum.hpp"
#include <string>
#include <vector>

std::string execSearch(const std::string exec);
void execCmd(const std::vector<std::string> cmd);
void execCmdWithRedirect(const std::vector<std::string> &cmd,
                         const std::string &filename, write_mode mode);
void execStderrCmdWithRedirect(const std::vector<std::string> &cmd,
                               const std::string &filename, write_mode mode);
void initializeCmdTrie();                 
void initWelcome();