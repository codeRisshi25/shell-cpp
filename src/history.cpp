#include "history.hpp"
#include <iostream>
#include <iomanip>

std::vector<std::string> commandHistory;

void addToHistory(const std::string& command) {
    if (!command.empty()) {
        commandHistory.push_back(command);
    }
}

void displayHistory(int restrict) {
    int start = (restrict == -1 ? 0 : commandHistory.size()-restrict);
    for (size_t i = start; i < commandHistory.size(); ++i) {
        std::cout << std::setw(4) << (i + 1) << "  " << commandHistory[i] << "\n";
    }
}

std::vector<std::string> getHistory() {
    return commandHistory;
}