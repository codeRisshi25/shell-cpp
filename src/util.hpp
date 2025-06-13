#pragma once
#include <string>
#include <vector>

std::string execSearch(const std::string exec);
void execCmd(const std::vector<std::string> cmd);
void execCmdWithRedirect(const std::vector<std::string> &cmd,
                         const std::string &filename);
void execStderrCmdWithRedirect(const std::vector<std::string> &cmd,
                         const std::string &filename);