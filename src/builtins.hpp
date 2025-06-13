#pragma once
#include "enum.hpp"
#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

using CmdHandler = std::function<void(const std::vector<std::string> &)>;
extern std::unordered_map<std::string, CmdHandler> builtins;

void init_builtins();
void executeBuiltinWithRedirect(const std::vector<std::string> &args,
                                const std::string &filename, write_mode mode);
void executeBuiltinWithStderrRedirect(const std::vector<std::string> &args,
                                      const std::string &filename,
                                      write_mode mode);
void initializeBuiltInTrie();
void cleanupCommandTrie();