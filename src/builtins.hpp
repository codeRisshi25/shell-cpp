#pragma once
#include <vector>
#include <string>
#include <unordered_map>
#include <functional>

using CmdHandler = std::function<void(const std::vector<std::string>&)>;
extern std::unordered_map<std::string, CmdHandler> builtins;

void init_builtins();
void executeBuiltinWithRedirect(const std::vector<std::string>& args, const std::string& filename);