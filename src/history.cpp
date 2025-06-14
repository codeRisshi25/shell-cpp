#include "history.hpp"
#include <iostream>
#include <iomanip>

std::vector<std::string> commandHistory;
int historyIndex = -1;  // Initialize to -1 (no history selected)

void addToHistory(const std::string& command) {
    if (!command.empty()) {
        commandHistory.push_back(command);
        historyIndex = commandHistory.size();  // Reset to end of history
    }
}

std::string handleUpArrow() {
    if (commandHistory.empty()) {
        return "";  // No history available
    }
    
    if (historyIndex > 0) {
        historyIndex--;
        return commandHistory[historyIndex];
    }
    
    return "";  // Already at the beginning
}

std::string handleDownArrow() {
    if (commandHistory.empty()) {
        return "";  // No history available
    }
    
    if (historyIndex < static_cast<int>(commandHistory.size()) - 1) {
        historyIndex++;
        return commandHistory[historyIndex];
    } else if (historyIndex == static_cast<int>(commandHistory.size()) - 1) {
        historyIndex = commandHistory.size();  // Move past end
        return "";  // Return empty string for "current" line
    }
    
    return "";  // Already at the end
}

void displayHistory(int restrict) {
    int start = (restrict == -1 ? 0 : std::max(0, static_cast<int>(commandHistory.size()) - restrict));
    for (size_t i = start; i < commandHistory.size(); ++i) {
        std::cout << std::setw(4) << (i + 1) << "  " << commandHistory[i] << "\n";
    }
}

std::vector<std::string> getHistory() {
    return commandHistory;
}