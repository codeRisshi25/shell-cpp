#pragma once
#include <vector>
#include <string>

extern std::vector<std::string> commandHistory;

void addToHistory(const std::string& command);
void displayHistory(int restrict);
std::vector<std::string> getHistory();